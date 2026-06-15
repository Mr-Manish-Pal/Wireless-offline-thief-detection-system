#include <WiFi.h>

void setup() {
  Serial.begin(115200);

  Serial.println("ESP32 MAC Address:");
  Serial.println(WiFi.macAddress());
}

void loop() {
}