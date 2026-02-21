#include "camera.h"
#include "config.h"
#include <Arduino.h>

bool camera_init() {
    camera_config_t config = {};

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0       = CAM_PIN_D0;
    config.pin_d1       = CAM_PIN_D1;
    config.pin_d2       = CAM_PIN_D2;
    config.pin_d3       = CAM_PIN_D3;
    config.pin_d4       = CAM_PIN_D4;
    config.pin_d5       = CAM_PIN_D5;
    config.pin_d6       = CAM_PIN_D6;
    config.pin_d7       = CAM_PIN_D7;
    config.pin_xclk     = CAM_PIN_XCLK;
    config.pin_pclk     = CAM_PIN_PCLK;
    config.pin_vsync    = CAM_PIN_VSYNC;
    config.pin_href     = CAM_PIN_HREF;
    config.pin_sscb_sda = CAM_PIN_SIOD;
    config.pin_sscb_scl = CAM_PIN_SIOC;
    config.pin_pwdn     = CAM_PIN_PWDN;
    config.pin_reset    = CAM_PIN_RESET;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_GRAYSCALE; // Grayscale saves RAM + faster inference
    config.frame_size   = FRAMESIZE_96X96;     // Match model input size directly
    config.fb_count     = 1;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("[CAM] Init error: 0x%x\n", err);
        return false;
    }

    // Sensor tuning for low-light outdoor use
    sensor_t* s = esp_camera_sensor_get();
    s->set_brightness(s, 1);
    s->set_contrast(s, 1);
    s->set_agc_gain(s, 30);     // Auto gain ceiling
    s->set_aec_value(s, 300);   // Auto exposure

    Serial.println("[CAM] Init OK");
    return true;
}

camera_fb_t* camera_capture() {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("[CAM] Frame buffer get failed");
        return nullptr;
    }
    Serial.printf("[CAM] Captured %zu bytes (%dx%d)\n", fb->len, fb->width, fb->height);
    return fb;
}

void camera_return(camera_fb_t* fb) {
    if (fb) esp_camera_fb_return(fb);
}
