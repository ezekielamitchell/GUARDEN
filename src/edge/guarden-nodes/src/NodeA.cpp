#include <Arduino.h>
#include <WiFi.h>

// WiFi
const char *ssid = "endr-hidden";
const char *password = "Endr07022024$$";

void nodeInfo(){
  Serial.println("XIAO ESP32-C3 Starting...");
  delay(1000);
  Serial.print("Chip Model: ");
  Serial.println(ESP.getChipModel());
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  delay(1000);
}

void initWifi(){
  WiFi.mode(WIFI_STA); // for hidden network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("\nConnected to the WiFi network");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);

  nodeInfo();

  // Network connection
  initWifi();


}

void loop() {
  Serial.println("[Node A] running...");
  delay(10000);
}