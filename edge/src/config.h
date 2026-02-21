#pragma once

// ─── Node Identity ────────────────────────────────────────────────────────────
#define NODE_ID         "node_01"
#define NODE_LAT        47.5980f
#define NODE_LNG        -122.3180f

// ─── WiFi ─────────────────────────────────────────────────────────────────────
#define WIFI_SSID       "GUARDEN_AP"
#define WIFI_PASSWORD   "guarden2025"
#define WIFI_TIMEOUT_MS 10000

// ─── MQTT ─────────────────────────────────────────────────────────────────────
#define MQTT_BROKER     "192.168.4.1"   // Raspberry Pi AP address
#define MQTT_PORT       1883
#define MQTT_CLIENT_ID  NODE_ID
#define TOPIC_DETECTION "guarden/nodes/" NODE_ID "/detection"
#define TOPIC_STATUS    "guarden/nodes/" NODE_ID "/status"

// ─── Camera (OV2640) ──────────────────────────────────────────────────────────
#define CAM_PIN_PWDN    -1
#define CAM_PIN_RESET   -1
#define CAM_PIN_XCLK    10
#define CAM_PIN_SIOD    40
#define CAM_PIN_SIOC    39
#define CAM_PIN_D7      48
#define CAM_PIN_D6      11
#define CAM_PIN_D5      12
#define CAM_PIN_D4      14
#define CAM_PIN_D3      16
#define CAM_PIN_D2      18
#define CAM_PIN_D1      17
#define CAM_PIN_D0      15
#define CAM_PIN_VSYNC   38
#define CAM_PIN_HREF    47
#define CAM_PIN_PCLK    13

// ─── PIR Sensor ───────────────────────────────────────────────────────────────
#define PIR_PIN         3
#define PIR_WARMUP_MS   2000

// ─── TFLite ───────────────────────────────────────────────────────────────────
#define MODEL_INPUT_W   96          // Input width (downsized from 640)
#define MODEL_INPUT_H   96          // Input height
#define CONFIDENCE_THRESHOLD 0.65f  // Minimum confidence to report

// ─── Power / Sleep ────────────────────────────────────────────────────────────
#define DEEP_SLEEP_US   30000000ULL // 30s deep sleep between checks
#define BATTERY_PIN     1           // ADC pin for battery voltage divider
#define BATTERY_SCALE   2.0f        // Voltage divider ratio

// ─── SD Card ──────────────────────────────────────────────────────────────────
#define SD_CS_PIN       4
#define SD_FALLBACK     true        // Buffer detections to SD if WiFi fails
