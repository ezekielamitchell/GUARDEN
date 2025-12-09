 ///////////////////////////////////////
///---------- NODE B ---------------///
///////////////////////////////////////
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "time.h"

// --- Configuration ---
// Wi-Fi Credentials
const char* ssid = "endr-hidden";
const char* password = "Endr07022024$$";

// MQTT Broker (Raspberry Pi's IP address)
const char *mqtt_server = "192.168.1.148";
const int mqtt_port = 1883; // Default MQTT port


// Unique Client ID for the ESP32
const char* mqtt_client_id = "ESP32_NodeB";
const char *mqtt_topic = "endr/secure"; // Topic to publish the message
const char* test_message = "MQTT PASS: ESP32-C3 NodeB";

// Time Configuration (PST/PDT for Seattle, WA)
const long gmtOffset_sec = -7 * 3600; // -7 hours for PST
const int daylightOffset_sec = 1 * 3600; // +1 hour for PDT (Daylight Savings)
const char* ntpServer = "pool.ntp.org"; // Use a general NTP server pool

// --- Global Objects ---
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0; // For timing the publishing rate
const long publishInterval = 5000;

// --- Function Declarations ---
void setup_wifi();
void setup_time();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
void publishData();

void setup() {
  Serial.begin(115200);
  setup_wifi();
  setup_time();

  // Initialize MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(5);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_time(){
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  time_t now = time(nullptr);
  int attempts = 0;
  while (now < 1672531200 && attempts < 10){
    delay(500);
    Serial.print("#");
    now = time(nullptr);
    attempts++;
  }
  Serial.println("\nTime synchronized.");
}

// Dummy callback function (required by PubSubClient)
void callback(char* topic, byte* payload, unsigned int length) {
  // Not subscribing to anything, so this can be empty
}

void reconnect() {
  while (!client.connected()) {

    Serial.print("Attempting MQTT connection...");

    if (client.connect(mqtt_client_id)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" Try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishData(){
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  char time_str[20];
  strftime(time_str, sizeof(time_str), "%H:%M:%S", &timeinfo);

  StaticJsonDocument<200> doc;
  doc["node_id"] = mqtt_client_id;
  doc["ip_address"] = WiFi.localIP().toString();
  doc["timestamp"] = time_str;
  doc["data_source"] = "NodeB : ESP32-C3 XIAO";

  char jsonBuffer[200];
  size_t n = serializeJson(doc, jsonBuffer);

  client.publish(mqtt_topic, jsonBuffer);

  Serial.print("Published JSON (");
  Serial.print(n);
  Serial.print(" bytes) to ");
  Serial.print(mqtt_topic);
  Serial.print(": ");
  Serial.println(jsonBuffer);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();

  if (now - lastMsg > publishInterval){
    lastMsg = now;
    publishData();
  }
}
