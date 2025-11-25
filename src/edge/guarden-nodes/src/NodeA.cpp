#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid = "endr-hidden";
const char *password = "Endr07022024$$";
const char *mqtt_server = "192.168.1.116";
const int mqtt_port = 1883;
const char *mqtt_topic = "guarden/node-a/status";

uint32_t last_ota_time = 0;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void reconnectMQTT() {
  if (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32-NodeA-" + String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" will try again later");
    }
  }
}

bool publishWiFiStatus() {
  StaticJsonDocument<256> doc;

  if (WiFi.status() == WL_CONNECTED) {
    doc["status"] = "connected";
    doc["ssid"] = WiFi.SSID();
    doc["ip"] = WiFi.localIP().toString();
    doc["rssi"] = WiFi.RSSI();
    doc["mac"] = WiFi.macAddress();
  } else {
    doc["status"] = "disconnected";
  }

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  if (mqttClient.publish(mqtt_topic, jsonBuffer)) {
    Serial.println("MQTT: Published successfully");
    return true;
  } else {
    Serial.println("MQTT: Publish failed");
    return false;
  }
}

void printWiFiStatus() {
  Serial.println("\n=== WiFi Status ===");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Status: Connected");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
  } else {
    Serial.println("Status: Disconnected");
  }

  Serial.println("==================\n");

  // Publish to MQTT
  publishWiFiStatus();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Setup MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  reconnectMQTT();

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      if (millis() - last_ota_time > 500) {
        Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
        last_ota_time = millis();
      }
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check for serial input
  if (Serial.available() > 0) {
    char inChar = Serial.read();
    if (inChar == 's' || inChar == 'S') {
      printWiFiStatus();
    }
  }

  // // Maintain MQTT connection
  // if (!mqttClient.connected()) {
  //   reconnectMQTT();
  // }
  // mqttClient.loop();

  ArduinoOTA.handle();
}
