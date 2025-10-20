#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_camera.h"
#include "camera_config.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// WiFi credentials - Update these with your network details
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Roboflow API endpoint (update with your workspace and model details)
const char* roboflow_api_url = "https://detect.roboflow.com/YOUR_MODEL/YOUR_VERSION";
const char* roboflow_api_key = "YOUR_ROBOFLOW_API_KEY";

// Detection interval (milliseconds)
const unsigned long DETECTION_INTERVAL = 5000; // Capture every 5 seconds
unsigned long lastCaptureTime = 0;

// Flash LED control
void flashLED(int duration) {
    digitalWrite(FLASH_LED_PIN, HIGH);
    delay(duration);
    digitalWrite(FLASH_LED_PIN, LOW);
}

// Initialize WiFi connection
void initWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi connection failed");
    }
}

// Capture photo from camera
camera_fb_t* capturePhoto() {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return NULL;
    }
    Serial.printf("Camera capture success: %d bytes\n", fb->len);
    return fb;
}

// Send image to Roboflow API for rodent detection
bool sendToRoboflow(camera_fb_t* fb) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        return false;
    }

    HTTPClient http;

    // Construct the full API URL with parameters
    String url = String(roboflow_api_url) + "?api_key=" + roboflow_api_key;

    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Send the image
    int httpResponseCode = http.POST(fb->buf, fb->len);

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        Serial.println("Response: " + response);

        // Parse response for rodent detection
        if (response.indexOf("predictions") > 0) {
            Serial.println("Detection analysis complete");

            // Check if any rodents were detected
            if (response.indexOf("\"class\":") > 0) {
                Serial.println("ALERT: Potential rodent detected!");
                flashLED(1000); // Flash for 1 second
            }
        }
        http.end();
        return true;
    } else {
        Serial.printf("Error sending image: %s\n", http.errorToString(httpResponseCode).c_str());
        http.end();
        return false;
    }
}

// Save image to SD card (optional - requires SD card module)
bool saveImageToSD(camera_fb_t* fb, const char* filename) {
    // This is a placeholder for SD card functionality
    // Implement if you have an SD card module connected
    Serial.println("SD card save not implemented yet");
    return false;
}

void setup() {
    // Disable brownout detector (helps with camera power draw)
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // Initialize Serial
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println("\n=== GUARDEN ESP32 Camera Starting ===");

    // Initialize flash LED
    pinMode(FLASH_LED_PIN, OUTPUT);
    digitalWrite(FLASH_LED_PIN, LOW);

    // Initialize camera
    Serial.println("Initializing camera...");
    esp_err_t err = init_camera();
    if (err != ESP_OK) {
        Serial.printf("Camera initialization failed: 0x%x\n", err);
        Serial.println("Restarting in 5 seconds...");
        delay(5000);
        ESP.restart();
    }

    // Flash LED to indicate successful camera init
    flashLED(200);
    delay(100);
    flashLED(200);

    // Initialize WiFi
    Serial.println("Initializing WiFi...");
    initWiFi();

    Serial.println("=== Setup Complete ===");
    Serial.println("Starting rodent detection system...");
}

void loop() {
    unsigned long currentTime = millis();

    // Capture and process image at regular intervals
    if (currentTime - lastCaptureTime >= DETECTION_INTERVAL) {
        lastCaptureTime = currentTime;

        Serial.println("\n--- Capturing image ---");

        // Optional: Flash LED during capture
        // flashLED(50);

        camera_fb_t* fb = capturePhoto();
        if (fb) {
            // Send to Roboflow for detection
            Serial.println("Sending to Roboflow for analysis...");
            sendToRoboflow(fb);

            // Optional: Save to SD card
            // saveImageToSD(fb, "/capture.jpg");

            // Return the frame buffer back to the driver
            esp_camera_fb_return(fb);
        }
    }

    // Small delay to prevent watchdog timer issues
    delay(100);
}
