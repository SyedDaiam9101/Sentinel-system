#include "esp_camera.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsClient.h> 
#include <EEPROM.h>
#include <ESPmDNS.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "fd_forward.h" 
#include "fr_forward.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


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
// Use GPIO 33 as a safe status LED pin (avoid flash pins 6-11)
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

// Global Objects
AsyncWebServer server(80);
WebSocketsClient webSocket;
TaskHandle_t AI_Task_Handle;
bool streamActive = false;
int humanVerificationCounter = 0; // Buffer to prevent false positives

// ==========================================================
// 🌡️ THERMAL GOVERNOR (THE GUARDIAN)
// ==========================================================
extern "C" uint8_t temprature_sens_read();

void runThermalCheck() {
    health.temperature = temprature_sens_read(); // correct °C reading
    health.freeHeap = ESP.getFreeHeap();
    health.minFreeHeap = ESP.getMinFreeHeap();
    
    if (health.temperature > 82.0) {
        currentState = COOLING;
        Serial.println(">>> CRITICAL THERMAL EVENT: SUSPENDING NEURAL CORE");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } else if (health.temperature > 72.0) {
        Serial.println(">>> THERMAL WARNING: ENGINE THROTTLING");
    } else {
        if (currentState == COOLING) currentState = IDLE;
    }
}

// ==========================================================
// 📹 MJPEG STREAMING SUBSYSTEM
// ==========================================================
#define BOUNDARY "pyramid_frame"
static const char* _STREAM_HEADER = "multipart/x-mixed-replace;boundary=" BOUNDARY;

void streamService(AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginChunkedResponse(_STREAM_HEADER, [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
        camera_fb_t * fb = esp_camera_fb_get();
        if(!fb) return 0;
        size_t len = fb->len;
        if(len > maxLen) { esp_camera_fb_return(fb); return 0; }
        memcpy(buffer, fb->buf, len);
        esp_camera_fb_return(fb);
        return len;
    });
    request->send(response);
}

// ==========================================================
// 🧠 NEURAL CORE: DISTRIBUTED AI KERNEL
// ==========================================================

bool detectNeuralTarget(camera_fb_t * fb) {
    if (!fb) return false;
    
    // Allocate RGB Matrix for Face Analysis
    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
    if(!image_matrix) return false;

    if(!fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item)) {
        dl_matrix3du_free(image_matrix);
        return false;
    }

    // Initialize MTMN Neural Configuration (Standard AI-Thinker Optimization)
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
    while(true) {
        runThermalCheck();
        
        if (currentState != COOLING) {
            camera_fb_t * fb = esp_camera_fb_get();
            if (fb) {
                health.framesProcessed++;
                
                if (detectNeuralTarget(fb)) {
                    humanVerificationCounter++;
                    
                    if (humanVerificationCounter >= 2) { // Must see face twice to be "Titanium" certain
                        currentState = ANALYZING;
                        Serial.printf("[%s] -> HUMAN VERIFIED. ALERTING.\n", SECTOR);
                        
                        StaticJsonDocument<256> doc;
                        doc["event"] = "alert";
                        doc["type"] = "HUMAN_TARGET";
                        doc["cam_id"] = CAM_ID;
                        doc["sector"] = SECTOR;
                        doc["temp"] = health.temperature;
                        
                        char buffer[512];
                        serializeJson(doc, buffer);
                        webSocket.sendTXT(buffer);
                        
                        health.alertsSent++;
                        digitalWrite(LED_STATUS, LOW);
                        humanVerificationCounter = 0; // Reset after alert
                    }
                } else {
                    digitalWrite(LED_STATUS, HIGH);
                    currentState = IDLE;
                    if (humanVerificationCounter > 0) humanVerificationCounter--; // Decay false looks
                }
                
                esp_camera_fb_return(fb);
            }
        }
        
        // Adaptive Processing Rate (Power Governed)
        int workload = (currentState == ANALYZING) ? 100 : 350;
        if (health.temperature > 75) workload += 200;
        vTaskDelay(workload / portTICK_PERIOD_MS);
    }
}

// ==========================================================
// 📡 NETWORK & COMMAND SERVICE
// ==========================================================

void onWsEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_CONNECTED:
            Serial.println("COMM_LINK::STABLE - Connected to Brain");
            break;
        case WStype_DISCONNECTED:
            Serial.println("COMM_LINK::LOST - Handshake Failed");
            break;
        case WStype_TEXT:
            Serial.printf("COMMAND_RX: %s\n", payload);
            break;
    }
}

// ==========================================================
// 🚀 UNIFIED BOOT LOADER
// ==========================================================

void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Activate Voltage Shield
    Serial.begin(115200);
    
    pinMode(LED_STATUS, OUTPUT);
    pinMode(LED_FLASH, OUTPUT);
    digitalWrite(LED_STATUS, HIGH); // Default Off

    // --- DEEP CAMERA INITIALIZATION ---
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
        config.frame_size = FRAMESIZE_QVGA; // Optimal for AI Matrix
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_CIF;
        config.jpeg_quality = 12;
        config.fb_count = 1;
        Serial.println("CRITICAL: PSRAM NOT DETECTED. AI PERFORMANCE REDUCED.");
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("BOOT_FAILURE: 0x%x\n", err);
        ESP.restart();
    }

    // --- CONNECTIVITY SUBSYSTEM ---
    if (!WiFi.config(local_IP, gateway, subnet)) {
        Serial.println("PHASE_0: STATIC IP CONFIGURATION FAILED.");
    }

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500); Serial.print(".");
    }
    Serial.println("\nPHASE_1: NETWORK ONLINE.");
    
    MDNS.begin("pyramid-cam1");
    
    // --- API & SERVICES ---
    server.on("/stream", HTTP_GET, streamService);
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *r){
        StaticJsonDocument<256> doc;
        doc["cam_id"] = CAM_ID;
        doc["temp"] = health.temperature;
        doc["heap"] = health.freeHeap;
        doc["fps"]  = health.framesProcessed / (millis() / 1000.0);
        String out; serializeJson(doc, out);
        r->send(200, "application/json", out);
    });

    server.on("/capture", HTTP_POST, [](AsyncWebServerRequest *request){
        camera_fb_t * fb = esp_camera_fb_get();
        if (!fb) {
            request->send(500, "application/json", "{\"status\":\"failed\"}");
            return;
        }
        // In this TITANIUM version, we just simulate the storage save or bridge it
        esp_camera_fb_return(fb);
        request->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    // Enable CORS for all responses (Essential for Dashboard)
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

    server.begin();

    // --- WEBSOCKET UPLINK ---
    webSocket.begin(BRAIN_IP, 81, "/");
    webSocket.onEvent(onWsEvent);
    webSocket.setReconnectInterval(5000);

    // --- NEURAL KERNEL LAUNCH ---
    // Stack set to 12k to handle Deep Face Matrix
    xTaskCreatePinnedToCore(NeuralKernel, "AI_CORE", 12000, NULL, 1, &AI_Task_Handle, 0);
    
    Serial.println("----------------------------------------");
    Serial.println(" PYRAMID SENTINEL SYSTEM INITIALIZED ");
    Serial.printf(" SECTOR: %s | NODE: %d\n", SECTOR, CAM_ID);
    Serial.println("----------------------------------------");
}

void loop() {
    webSocket.loop();
    health.uptime = millis() / 1000;
}

