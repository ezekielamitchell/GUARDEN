#include <Arduino.h>
#include "config.h"
#include "camera.h"
#include "detector.h"
#include "mqtt_client.h"

// ─── Deep sleep wake stub ─────────────────────────────────────────────────────
RTC_DATA_ATTR int boot_count = 0;

void setup() {
    Serial.begin(115200);
    boot_count++;
    Serial.printf("[GUARDEN] Boot #%d | Node: %s\n", boot_count, NODE_ID);

    // Read battery voltage
    int raw = analogRead(BATTERY_PIN);
    int battery_mv = (int)((raw / 4095.0f) * 3300.0f * BATTERY_SCALE);

    // Init camera
    if (!camera_init()) {
        Serial.println("[CAM] Init failed — sleeping");
        esp_deep_sleep(DEEP_SLEEP_US);
    }

    // Capture frame
    camera_fb_t* fb = camera_capture();
    if (!fb) {
        Serial.println("[CAM] Capture failed — sleeping");
        esp_deep_sleep(DEEP_SLEEP_US);
    }

    // Run inference
    float confidence = detector_run(fb->buf, fb->len);
    camera_return(fb);

    Serial.printf("[DET] Confidence: %.2f\n", confidence);

    if (confidence >= CONFIDENCE_THRESHOLD) {
        // Connect WiFi + MQTT and publish
        if (mqtt_connect()) {
            mqtt_publish_detection(confidence, battery_mv);
            mqtt_disconnect();
        } else {
            // Fallback: write to SD card
            Serial.println("[MQTT] Failed — writing to SD");
            // TODO: sd_write_detection(confidence, battery_mv);
        }
    }

    // Done — back to deep sleep
    Serial.printf("[SLEEP] Going to sleep for %llu ms\n", DEEP_SLEEP_US / 1000);
    esp_deep_sleep(DEEP_SLEEP_US);
}

void loop() {
    // Never reached — deep sleep handles cycling
}
