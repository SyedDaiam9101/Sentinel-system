idf_component_register(SRCS "core_main.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES arduino ESP_Async_WebServer ArduinoJson Adafruit_GFX_Library Adafruit_ILI9341 WebSockets Async_TCP esp-face esp32-camera)
