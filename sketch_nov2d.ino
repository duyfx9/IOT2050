#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>

// Wi-Fi and MQTT credentials
const char* ssid = "HCMUT03";
const char* password = "123456789";
const char* mqttServer = "192.168.54.94";
const int mqttPort = 1883;

// Define RC522 pins
#define SS_PIN  5   // GPIO5 for SDA (SS)
#define RST_PIN 4   // GPIO4 for RST

// Initialize RFID object
MFRC522 rfid(SS_PIN, RST_PIN);

// Initialize WiFi and MQTT client objects
WiFiClient espClient;
PubSubClient client(espClient);

// Variables
String assignedID = "c11191d";
String personName = "Nguyen Van A";

// Predefined date and time
String date1 = "2/11/2024";
String time1 = "10:22 AM";

// Wi-Fi and MQTT connection setup
void setup() {
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

void loop() {
  client.loop();

  // Check if RFID card is present
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Get the UID from the card
  String cardID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    cardID += String(rfid.uid.uidByte[i], HEX);
  }
  Serial.print("Card ID: ");
  Serial.println(cardID);

  // Check if the card ID matches the assigned ID
  if (cardID == assignedID) {
    // Format message
    String message = "ID: " + assignedID + " - Name: " + personName + " - Date: " + date1 + " - Time: " + time1;

    // Publish message to MQTT
    if (client.publish("esp32/rfid/log", message.c_str())) {
      Serial.println("Message sent: " + message);
    } else {
      Serial.println("Failed to send message");
    }
  }

  // Halt the reading process for a bit to avoid duplicates
  delay(2000);

  // Halt further operations on the card
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
