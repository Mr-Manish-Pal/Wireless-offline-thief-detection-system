#include <WiFi.h>
#include <esp_now.h>

#define IR_PIN 32
#define MQ135_PIN 35
#define SPEAKER_PIN 18

// Receiver MAC Address
uint8_t receiverMAC[] = {0xA8, 0x42, 0xE3, 0xBA, 0xC2, 0xD8};

typedef struct {
  bool irDetected;
  int mq135Value;
} Message;

Message data;
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("SUCCESS");
  } else {
    Serial.println("FAILED");
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize Pins
  pinMode(IR_PIN, INPUT);
  pinMode(MQ135_PIN, INPUT); // Added for explicit analog initialization
  pinMode(SPEAKER_PIN, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  Serial.println("ESP32 Sender Starting...");

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  // Register send callback
  esp_now_register_send_cb(onDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 1;  // Locked to Channel 1 (Ensure receiver matches this!)
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Peer Add Failed");
    return;
  }

  Serial.println("Sender Ready");
}

void loop() {
  // IR Sensor Read (Active LOW logic handled correctly)
  data.irDetected = (digitalRead(IR_PIN) == LOW);

  // MQ135 Read
  data.mq135Value = analogRead(MQ135_PIN);

  // Speaker Alarm
  if (data.irDetected) {
    tone(SPEAKER_PIN, 1000);
  } else {
    noTone(SPEAKER_PIN);
  }

  // Send Data via ESP-NOW
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));

  // Serial Debug Output
  Serial.print("IR: ");
  if (data.irDetected) {
    Serial.print("DETECTED");
  } else {
    Serial.print("CLEAR");
  }

  Serial.print(" | MQ135: ");
  Serial.print(data.mq135Value);

  if (result == ESP_OK) {
    Serial.println(" | PACKET QUEUED");
  } else {
    Serial.println(" | SEND ERROR");
  }

  delay(300); // 300ms is a safe and responsive interval
}