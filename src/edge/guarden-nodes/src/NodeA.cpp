  ///////////////////////////////////////
 ///---------- NODE A ---------------///
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

// Connection retry settings
const int WIFI_RETRY_DELAY = 500;        // ms between WiFi attempts
const int WIFI_MAX_ATTEMPTS = 120;       // 60 seconds total
const int MQTT_RETRY_INTERVAL = 5000;    // ms between MQTT attempts
unsigned long lastMqttAttempt = 0;       // Track last MQTT connection attempt


// Unique Client ID for the ESP32
const char* mqtt_client_id = "ESP32_XIAO_Publisher"; 
const char *mqtt_topic = "guarden"; // Topic to publish the message
const char* test_message = "MQTT PASS: ESP32-C3 XIAO";

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

  // CRITICAL: Wait for power to stabilize on standalone boot
  delay(3000);

  Serial.println("\n\n========================================");
  Serial.println("=== ESP32 XIAO NodeA Starting ===");
  Serial.println("========================================");
  Serial.print("Chip Model: ");
  Serial.println(ESP.getChipModel());
  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.println();

  // Initialize MQTT settings
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.setKeepAlive(30);  // Keep connection alive
  client.setSocketTimeout(15);  // Socket timeout

  // Initial WiFi connection
  setup_wifi();

  // Only try to sync time if WiFi connected
  if (WiFi.status() == WL_CONNECTED) {
    setup_time();
  } else {
    Serial.println("Skipping time sync - no WiFi connection");
  }

  Serial.println("========================================");
  Serial.println("=== Setup Complete - Entering Loop ===");
  Serial.println("========================================\n");
}

void setup_wifi() {
  Serial.println("\n--- WiFi Connection Attempt ---");
  Serial.print("Target SSID: ");
  Serial.println(ssid);
  Serial.print("Target Gateway/MQTT Server: ");
  Serial.println(mqtt_server);

  // Full WiFi reset
  WiFi.persistent(false);  // Don't save WiFi config to flash
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(500);

  // Configure WiFi
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);  // Enable auto-reconnect
  WiFi.setMinSecurity(WIFI_AUTH_WPA_PSK);

  // Start connection
  WiFi.begin(ssid, password);
  Serial.println("WiFi.begin() called...");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < WIFI_MAX_ATTEMPTS) {
    delay(WIFI_RETRY_DELAY);
    Serial.print(".");
    attempts++;

    // Detailed status every 10 attempts
    if (attempts % 10 == 0) {
      Serial.println();
      Serial.print("  Attempt: ");
      Serial.print(attempts);
      Serial.print("/");
      Serial.print(WIFI_MAX_ATTEMPTS);
      Serial.print(" | Status: ");

      // Decode WiFi status
      switch(WiFi.status()) {
        case WL_IDLE_STATUS:
          Serial.println("IDLE (waiting)");
          break;
        case WL_NO_SSID_AVAIL:
          Serial.println("NO_SSID (network not found!)");
          // Retry connection
          WiFi.disconnect();
          delay(100);
          WiFi.begin(ssid, password);
          break;
        case WL_CONNECT_FAILED:
          Serial.println("CONNECT_FAILED (wrong password?)");
          break;
        case WL_CONNECTION_LOST:
          Serial.println("CONNECTION_LOST");
          break;
        case WL_DISCONNECTED:
          Serial.println("DISCONNECTED");
          break;
        default:
          Serial.print("UNKNOWN(");
          Serial.print(WiFi.status());
          Serial.println(")");
      }
      Serial.print("  ");
    }

    // Aggressive retry every 20 attempts
    if (attempts % 20 == 0) {
      Serial.println("\n  --> Hard retry...");
      WiFi.disconnect();
      delay(1000);
      WiFi.begin(ssid, password);
    }
  }

  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("*** WiFi CONNECTED! ***");
    Serial.print("  IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("  Gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("  Subnet: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("  DNS: ");
    Serial.println(WiFi.dnsIP());
    Serial.print("  Signal (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("  MAC Address: ");
    Serial.println(WiFi.macAddress());
  } else {
    Serial.println("*** WiFi FAILED! ***");
    Serial.print("  Final Status: ");
    Serial.println(WiFi.status());
    Serial.println("  Will retry in main loop...");
  }
  Serial.println("--- End WiFi Attempt ---\n");
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
  // Throttle MQTT connection attempts
  unsigned long now = millis();
  if (now - lastMqttAttempt < MQTT_RETRY_INTERVAL) {
    return;  // Too soon, skip this attempt
  }

  lastMqttAttempt = now;

  if (!client.connected()) {
    Serial.print("[MQTT] Connecting to ");
    Serial.print(mqtt_server);
    Serial.print(":");
    Serial.print(mqtt_port);
    Serial.print(" as '");
    Serial.print(mqtt_client_id);
    Serial.print("'...");

    if (client.connect(mqtt_client_id)) {
      Serial.println(" SUCCESS!");
      Serial.print("[MQTT] Subscribed to topic: ");
      Serial.println(mqtt_topic);
    } else {
      Serial.print(" FAILED! rc=");
      int state = client.state();
      Serial.print(state);
      Serial.print(" (");

      // Decode MQTT error codes
      switch(state) {
        case -4: Serial.print("TIMEOUT"); break;
        case -3: Serial.print("CONNECTION_LOST"); break;
        case -2: Serial.print("CONNECT_FAILED"); break;
        case -1: Serial.print("DISCONNECTED"); break;
        case 1: Serial.print("BAD_PROTOCOL"); break;
        case 2: Serial.print("BAD_CLIENT_ID"); break;
        case 3: Serial.print("UNAVAILABLE"); break;
        case 4: Serial.print("BAD_CREDENTIALS"); break;
        case 5: Serial.print("UNAUTHORIZED"); break;
        default: Serial.print("UNKNOWN");
      }
      Serial.println(") - retry in 5s");
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
  doc["data_source"] = "NodeA : ESP32-C3 XIAO";

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
  static unsigned long lastWifiCheck = 0;
  static bool wasConnected = false;
  unsigned long now = millis();

  // Check WiFi status every second
  if (now - lastWifiCheck > 1000) {
    lastWifiCheck = now;

    bool isConnected = (WiFi.status() == WL_CONNECTED);

    // Log state changes
    if (isConnected != wasConnected) {
      if (isConnected) {
        Serial.println("\n[WiFi] Connection restored!");
        Serial.print("  IP: ");
        Serial.println(WiFi.localIP());
      } else {
        Serial.println("\n[WiFi] Connection LOST! Reconnecting...");
        setup_wifi();
        wasConnected = false;
        return;
      }
      wasConnected = isConnected;
    }
  }

  // Only proceed if WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    delay(100);
    return;
  }

  // Check and reconnect MQTT if needed
  if (!client.connected()) {
    reconnect();
  }

  // Process MQTT messages
  client.loop();

  // Publish data at regular intervals
  if (now - lastMsg > publishInterval) {
    lastMsg = now;

    if (client.connected()) {
      publishData();
    } else {
      Serial.println("[Publish] Skipped - MQTT not connected");
    }
  }
}