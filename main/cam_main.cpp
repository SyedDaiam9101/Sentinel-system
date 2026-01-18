#include "esp_camera.h"
#include <vector>
#include <memory>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WebSocketsClient.h> 
#include <esp_now.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "fd_forward.h" 
#include "fr_forward.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

/**
 * 🏔️ PYRAMID SENTINEL PRO - TITANIUM CAMERA UNIT
 * Version: 4.1.0 "NEURAL ARMOR & ANALYTICS"
 * 
 * Hardware: AI-Thinker ESP32-CAM
 */

// ==========================================================
// 📍 HARDWARE PINS (STRICT AI-THINKER STANDARD)
// ==========================================================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define LED_FLASH          4
#define LED_STATUS         33

// ==========================================================
// 📡 NETWORK & IDENTITY
// ==========================================================
const char* WIFI_SSID   = "PyramidNet";
const char* WIFI_PASS   = "pointbreak";
const char* BRAIN_IP    = "192.168.4.1";
const int CAM_ID        = 1;
const char* SECTOR      = "NORTH";

IPAddress local_IP(192, 168, 4, 2);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// ==========================================================
// 🛠️ SYSTEM STATE & TYPES
// ==========================================================
enum CamState { IDLE, ANALYZING, STREAMING, COOLING, ERROR };
CamState currentState = IDLE;

struct MachineHealth {
    float temperature;
    size_t freeHeap;
    size_t minFreeHeap;
    uint32_t framesProcessed;
    uint32_t alertsSent;
    uint32_t uptime;
} health;

// ==========================================================
// 📡 ESP-NOW PROTOCOL (ULTRA-LOW LATENCY)
// ==========================================================
typedef struct {
    int id;
    int type; // 0=HEARTBEAT, 1=ALERT
    float temp;
    uint32_t heap;
} esp_now_data_t;

uint8_t brain_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Broadcast

void sendEspNow(int type) {
    esp_now_data_t data;
    data.id = CAM_ID;
    data.type = type;
    data.temp = health.temperature;
    data.heap = health.freeHeap;
    
    esp_now_send(brain_mac, (uint8_t *) &data, sizeof(data));
}

// Global Objects
AsyncWebServer server(80);
WebSocketsClient webSocket;
TaskHandle_t AI_Task_Handle;
volatile int activeStreams = 0; 
int humanVerificationCounter = 0; 

// ==========================================================
// 🌡️ THERMAL GOVERNOR
// ==========================================================
extern "C" uint8_t temprature_sens_read();

void runThermalCheck() {
    health.temperature = temprature_sens_read();
    health.freeHeap = ESP.getFreeHeap();
    health.minFreeHeap = ESP.getMinFreeHeap();
    
    if (health.temperature > 82.0) {
        currentState = COOLING;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } else if (health.temperature > 72.0) {
        // Throttling handled by delay
    } else {
        if (currentState == COOLING) currentState = IDLE;
    }
}

// ==========================================================
// 📹 MJPEG STREAMING SUBSYSTEM
// ==========================================================
#define BOUNDARY "pyramid_frame"
static const char* _STREAM_HEADER = "multipart/x-mixed-replace;boundary=" BOUNDARY;

struct StreamState {
    camera_fb_t * fb = nullptr;
    size_t offset = 0;
    bool header_sent = false;
    ~StreamState() {
        if (fb) esp_camera_fb_return(fb);
    }
};

void streamService(AsyncWebServerRequest *request) {
    activeStreams++;
    request->onDisconnect([](){
        if(activeStreams > 0) activeStreams--;
    });
    
    std::shared_ptr<StreamState> ctx = std::make_shared<StreamState>();

    AsyncWebServerResponse *response = request->beginChunkedResponse(_STREAM_HEADER, [ctx](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
        size_t written = 0;
        if (!ctx->fb) {
             ctx->fb = esp_camera_fb_get();
             if (!ctx->fb) return 0; 
             ctx->offset = 0;
             ctx->header_sent = false;
        }

        while (written < maxLen && ctx->fb) {
            if (!ctx->header_sent) {
                 char header[128];
                 int hlen = snprintf(header, sizeof(header), "--" BOUNDARY "\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", ctx->fb->len);
                 if (maxLen - written >= hlen) {
                     memcpy(buffer + written, header, hlen);
                     written += hlen;
                     ctx->header_sent = true;
                 } else break; 
            } else {
                size_t remaining = ctx->fb->len - ctx->offset;
                size_t space = maxLen - written;
                if (remaining == 0) {
                     if (space >= 2) {
                         memcpy(buffer + written, "\r\n", 2);
                         written += 2;
                         esp_camera_fb_return(ctx->fb);
                         ctx->fb = nullptr;
                         break; 
                     } else break;
                }
                size_t to_copy = (remaining < space) ? remaining : space;
                memcpy(buffer + written, ctx->fb->buf + ctx->offset, to_copy);
                written += to_copy;
                ctx->offset += to_copy;
            }
        }
        return written;
    });
    
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

// ==========================================================
// 🧠 NEURAL CORE
// ==========================================================
bool detectNeuralTarget(camera_fb_t * fb) {
    if (!fb) return false;
    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
    if(!image_matrix) return false;
    if(!fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item)) {
        dl_matrix3du_free(image_matrix);
        return false;
    }
    mtmn_config_t config = mtmn_init_config();
    box_array_t *boxes = face_detect(image_matrix, &config);
    bool targetFound = (boxes != NULL);
    if(boxes) {
        free(boxes->box);
        free(boxes->landmark);
        free(boxes);
    }
    dl_matrix3du_free(image_matrix);
    return targetFound;
}

void NeuralKernel(void * p) {
    uint32_t lastHeartbeat = 0;
    while(true) {
        runThermalCheck();
        if (millis() - lastHeartbeat > 5000) {
            sendEspNow(0);
            lastHeartbeat = millis();
        }
        if (currentState != COOLING) {
            camera_fb_t * fb = esp_camera_fb_get();
            if (fb) {
                health.framesProcessed++;
                if (detectNeuralTarget(fb)) {
                    humanVerificationCounter++;
                    if (humanVerificationCounter >= 2) {
                        currentState = ANALYZING;
                        StaticJsonDocument<256> doc;
                        doc["event"] = "alert";
                        doc["type"] = "HUMAN_TARGET";
                        doc["cam_id"] = CAM_ID;
                        doc["sector"] = SECTOR;
                        doc["temp"] = health.temperature;
                        char buffer[512];
                        serializeJson(doc, buffer);
                        webSocket.sendTXT(buffer);
                        sendEspNow(1);
                        health.alertsSent++;
                        digitalWrite(LED_STATUS, LOW);
                        humanVerificationCounter = 0;
                    }
                } else {
                    digitalWrite(LED_STATUS, HIGH);
                    currentState = IDLE;
                    if (humanVerificationCounter > 0) humanVerificationCounter--;
                }
                esp_camera_fb_return(fb);
            }
        }
        int workload = (currentState == ANALYZING) ? 100 : 350;
        if (activeStreams > 0) workload += 150; 
        vTaskDelay(workload / portTICK_PERIOD_MS);
    }
}

void onWsEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_CONNECTED: Serial.println("CONNECTED TO BRAIN"); break;
        case WStype_DISCONNECTED: Serial.println("DISCONNECTED FROM BRAIN"); break;
    }
}

void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    Serial.begin(115200);
    pinMode(LED_STATUS, OUTPUT);
    pinMode(LED_FLASH, OUTPUT);
    digitalWrite(LED_STATUS, HIGH);

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0       = Y2_GPIO_NUM;
    config.pin_d1       = Y3_GPIO_NUM;
    config.pin_d2       = Y4_GPIO_NUM;
    config.pin_d3       = Y5_GPIO_NUM;
    config.pin_d4       = Y6_GPIO_NUM;
    config.pin_d5       = Y7_GPIO_NUM;
    config.pin_d6       = Y8_GPIO_NUM;
    config.pin_d7       = Y9_GPIO_NUM;
    config.pin_xclk     = XCLK_GPIO_NUM;
    config.pin_pclk     = PCLK_GPIO_NUM;
    config.pin_vsync    = VSYNC_GPIO_NUM;
    config.pin_href     = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn     = PWDN_GPIO_NUM;
    config.pin_reset    = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    
    if (psramFound()) {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_CIF;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    esp_camera_init(&config);
    WiFi.config(local_IP, gateway, subnet);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    
    MDNS.begin("pyramid-cam1");
    server.on("/stream", HTTP_GET, streamService);
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *r){
        StaticJsonDocument<256> doc;
        doc["cam_id"] = CAM_ID;
        doc["temp"] = health.temperature;
        String out; serializeJson(doc, out);
        r->send(200, "application/json", out);
    });
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    server.begin();

    webSocket.begin(BRAIN_IP, 80, "/ws");
    webSocket.onEvent(onWsEvent);
    webSocket.setReconnectInterval(5000);

    if (esp_now_init() == ESP_OK) {
        esp_now_peer_info_t peerInfo;
        memcpy(peerInfo.peer_addr, brain_mac, 6);
        peerInfo.channel = 1;
        peerInfo.encrypt = false;
        esp_now_add_peer(&peerInfo);
    }

    xTaskCreatePinnedToCore(NeuralKernel, "AI_CORE", 12000, NULL, 1, &AI_Task_Handle, 0);
}

void loop() {
    webSocket.loop();
}
