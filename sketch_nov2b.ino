#include <WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

// Define GPIO pins for the relays
#define Device1 2
#define Device2 4
#define Device3 16
#define Device4 17

// Wi-Fi credentials
const char* ssid = "HCMUT03";
const char* password = "123456789";

// MQTT Broker details
const char* mqttServer = "192.168.54.94";
const int mqttPort = 1883;

// Initialize the WiFi and MQTT client objects
WiFiClient espClient;
PubSubClient client(espClient);

// Variables to store the current state of each relay (ON/OFF)
bool Device1State = false;
bool Device2State = false;
bool Device3State = false;
bool Device4State = false;

// Variable to enable or disable state saving
const bool saveState = true;

// EEPROM address to store the states
const int eepromSize = 8;

// Callback function for MQTT subscription
void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  Serial.println(messageTemp);

  // Control relays based on the topic
  if (String(topic) == "esp32/control/relay1") {
    Device1State = (messageTemp == "true");
    digitalWrite(Device1, Device1State ? LOW : HIGH);
    if (saveState) EEPROM.write(0, Device1State);
  } else if (String(topic) == "esp32/control/relay2") {
    Device2State = (messageTemp == "true");
    digitalWrite(Device2, Device2State ? LOW : HIGH);
    if (saveState) EEPROM.write(1, Device2State);
  } else if (String(topic) == "esp32/control/relay3") {
    Device3State = (messageTemp == "true");
    digitalWrite(Device3, Device3State ? LOW : HIGH);
    if (saveState) EEPROM.write(2, Device3State);
  } else if (String(topic) == "esp32/control/relay4") {
    Device4State = (messageTemp == "true");
    digitalWrite(Device4, Device4State ? LOW : HIGH);
    if (saveState) EEPROM.write(3, Device4State);
  }

  if (saveState) {
    EEPROM.commit();
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize EEPROM
  if (saveState) {
    EEPROM.begin(eepromSize);
  }

  // Initialize GPIO pins as outputs
  pinMode(Device1, OUTPUT);
  pinMode(Device2, OUTPUT);
  pinMode(Device3, OUTPUT);
  pinMode(Device4, OUTPUT);

  // Load saved states from EEPROM
  if (saveState) {
    Device1State = EEPROM.read(0);
    Device2State = EEPROM.read(1);
    Device3State = EEPROM.read(2);
    Device4State = EEPROM.read(3);
  }

  // Set initial relay states
  digitalWrite(Device1, Device1State ? LOW : HIGH);
  digitalWrite(Device2, Device2State ? LOW : HIGH);
  digitalWrite(Device3, Device3State ? LOW : HIGH);
  digitalWrite(Device4, Device4State ? LOW : HIGH);

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

  // Connect to MQTT Broker
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

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

  // Subscribe to topics for each relay
  client.subscribe("esp32/control/relay1");
  client.subscribe("esp32/control/relay2");
  client.subscribe("esp32/control/relay3");
  client.subscribe("esp32/control/relay4");
}

void loop() {
  client.loop();
}
