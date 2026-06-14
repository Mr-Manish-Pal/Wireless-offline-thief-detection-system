#include <WiFi.h>
#include <esp_now.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SPEAKER_PIN 18

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

typedef struct
{
  bool irDetected;
  int mq135Value;
} Message;

Message incomingData;

void onReceive(
  const uint8_t *mac,
  const uint8_t *incomingDataBytes,
  int len
)
{
  memcpy(
    &incomingData,
    incomingDataBytes,
    sizeof(incomingData)
  );

  Serial.print("IR: ");
  Serial.print(incomingData.irDetected);

  Serial.print("  MQ135: ");
  Serial.println(incomingData.mq135Value);

  // OLED Update
  display.clearDisplay();

display.setTextColor(WHITE);

// Status
display.setTextSize(2);
display.setCursor(0,0);

if(incomingData.irDetected)
{
    display.println("ALERT");
}
else
{
    display.println("SAFE");
}

// MQ135 Reading
display.setTextSize(2);
display.setCursor(0,30);
display.print("MQ:");
display.println(incomingData.mq135Value);

display.display();

  // Speaker Alarm
  if(incomingData.irDetected)
  {
    for(int f = 800; f <= 1800; f += 100)
    {
      tone(SPEAKER_PIN, f);
      delay(15);
    }

    for(int f = 1800; f >= 800; f -= 100)
    {
      tone(SPEAKER_PIN, f);
      delay(15);
    }

    noTone(SPEAKER_PIN);
  }
  else
  {
    noTone(SPEAKER_PIN);
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(SPEAKER_PIN, OUTPUT);

  Wire.begin(19, 21);

  if(!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C
      ))
  {
    Serial.println("OLED Failed");

    while(true);
  }

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,20);

  display.println("READY");

  display.display();

  WiFi.mode(WIFI_STA);

  if(esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);

  Serial.println("Receiver Ready");
}

void loop()
{
}