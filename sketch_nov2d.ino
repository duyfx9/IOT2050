#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include "time.h"

// Wi-Fi and MQTT credentials
const char* ssid = "PIF_CLUB";
const char* password = "chinsochin";
const char* mqttServer = "192.168.10.3";
const int mqttPort = 1883;

#define SS_PIN  5   // GPIO5 for SDA (SS)
#define RST_PIN 4   // GPIO4 for RST
MFRC522 rfid(SS_PIN, RST_PIN);

WiFiClient espClient;
PubSubClient client(espClient);

struct CardInfo {
  String uid;
  String name;
};

CardInfo validCards[] = {
  {"C111091D", "Nguyen Van A"},
  {"F5052BE2", "Tran Van B"},
  {"11223344", "Card 3"}
};

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;  // GMT+7
const int   daylightOffset_sec = 0;

void setup() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT broker
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print("Date: ");
  Serial.printf("%02d-%02d-%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  Serial.print(" Time: ");
  Serial.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  Serial.println();
}

void loop() {
  client.loop();

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  String cardUid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    cardUid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    cardUid += String(rfid.uid.uidByte[i], HEX);
  }
  cardUid.toUpperCase();

  // Find if card UID exists in validCards array
  String cardName = "Thẻ không hợp lệ";
  for (int i = 0; i < sizeof(validCards) / sizeof(validCards[0]); i++) {
    if (validCards[i].uid == cardUid) {
      cardName = validCards[i].name;
      break;
    }
  }

  Serial.print("ID: ");
  Serial.println(cardUid);
  Serial.print("Name: ");
  Serial.println(cardName);

  // Get local time
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%d/%m/%Y %H:%M:%S", &timeinfo);
    String message = "ID: " + cardUid + " - Name: " + cardName + " - Date: " + timeStr;

    if (client.publish("esp32/rfid/log", message.c_str())) {
      Serial.println("Message sent: " + message);
    } else {
      Serial.println("Failed to send message");
    }
  }

  delay(2000);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
