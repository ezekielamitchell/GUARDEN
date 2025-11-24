#include <Arduino.h>

void setup() {
  Serial.begin(115200);

  Serial.println("XIAO ESP32-S3 Starting...");
  delay(3000);
  Serial.print("Chip Model: ");
  Serial.println(ESP.getChipModel());
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
  delay(3000);
}

void loop() {
  Serial.println("Running...");
  delay(2000);
}