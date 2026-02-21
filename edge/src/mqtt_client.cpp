#include "mqtt_client.h"
#include "config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

static WiFiClient   wifi_client;
static PubSubClient mqtt(wifi_client);

static bool wifi_connect() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.printf("[WIFI] Connecting to %s", WIFI_SSID);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - start > WIFI_TIMEOUT_MS) {
            Serial.println("\n[WIFI] Timeout");
            return false;
        }
        delay(250);
        Serial.print(".");
    }
    Serial.printf("\n[WIFI] Connected | IP: %s\n", WiFi.localIP().toString().c_str());
    return true;
}

bool mqtt_connect() {
    if (!wifi_connect()) return false;

    mqtt.setServer(MQTT_BROKER, MQTT_PORT);
    mqtt.setKeepAlive(10);

    if (!mqtt.connect(MQTT_CLIENT_ID)) {
        Serial.printf("[MQTT] Connect failed | State: %d\n", mqtt.state());
        return false;
    }

    Serial.println("[MQTT] Connected");
    return true;
}

void mqtt_publish_detection(float confidence, int battery_mv) {
    JsonDocument doc;
    doc["node_id"]    = NODE_ID;
    doc["timestamp"]  = millis(); // Replace with NTP time if available
    doc["confidence"] = confidence;
    doc["location"]["lat"] = NODE_LAT;
    doc["location"]["lng"] = NODE_LNG;
    doc["battery_mv"] = battery_mv;

    char payload[256];
    serializeJson(doc, payload, sizeof(payload));

    if (mqtt.publish(TOPIC_DETECTION, payload)) {
        Serial.printf("[MQTT] Published: %s\n", payload);
    } else {
        Serial.println("[MQTT] Publish failed");
    }
}

void mqtt_publish_status() {
    JsonDocument doc;
    doc["node_id"] = NODE_ID;
    doc["status"]  = "alive";
    doc["uptime"]  = millis();

    char payload[128];
    serializeJson(doc, payload, sizeof(payload));
    mqtt.publish(TOPIC_STATUS, payload);
}

void mqtt_disconnect() {
    mqtt.disconnect();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}
