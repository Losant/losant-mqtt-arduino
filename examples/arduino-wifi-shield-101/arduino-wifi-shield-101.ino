/**
 * Example that connects an Arduino Zero with Arduino WiFi Shield 101 to the
 * Structure IoT platform. This example reports state to Structure whenever
 * a button is pressed. It also listens for the "toggle" command to turn the
 * LED on and off.
 *
 * This example assumes the following connections:
 * Button connected to pin 14.
 * LED connected to pin 12.
 *
 * Copyright (c) 2016 Structure. All rights reserved.
 * http://www.getstructure.io
 */

#include <WiFi101.h>
#include <Structure.h>

// WiFi credentials.
const char* WIFI_SSID = "my-wifi-ssid";
const char* WIFI_PASS = "my-wifi-pass";

// Structure credentials.
const char* STRUCTURE_DEVICE_ID = "my-device-id";
const char* STRUCTURE_ACCESS_KEY = "my-app-key";
const char* STRUCTURE_ACCESS_SECRET = "my-app-secret";

const int BUTTON_PIN = 14;
const int LED_PIN = 12;

bool ledState = false;

WiFiSSLClient wifiClient;

// For an unsecured connection to Structure.
// WiFiClient wifiClient;

StructureDevice device(STRUCTURE_DEVICE_ID);

void toggle() {
  Serial.println("Toggling LED.");
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
}

// Called whenever the device receives a command from the Structure platform.
void handleCommand(StructureCommand *command) {
  Serial.print("Command received: ");
  Serial.println(command->name);

  if(strcmp(command->name, "toggle") == 0) {
    toggle();
  }
}

void connect() {

  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to Structure.
  Serial.println();
  Serial.print("Connecting to Structure...");

device.connectSecure(wifiClient, STRUCTURE_ACCESS_KEY, STRUCTURE_ACCESS_SECRET);

  // For an unsecured connection.
  //device.connect(wifiClient, ACCESS_KEY, ACCESS_SECRET);

  while(!device.connected()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected!");
}

void setup() {
  Serial.begin(115200);
  while(!Serial) { }
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Register the command handler to be called when a command is received
  // from the Structure platform.
  device.onCommand(&handleCommand);

  connect();
}

void buttonPressed() {
  Serial.println("Button Pressed!");

  // Structure uses a JSON protocol. Construct the simple state object.
  // { "button" : true }
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["button"] = true;

  // Send the state to Structure.
  device.sendState(root);
}

int buttonState = 0;

void loop() {

  bool toReconnect = false;

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Disconnected from WiFi");
    toReconnect = true;
  }

  if(!device.connected()) {
    Serial.println("Disconnected from Structure");
    Serial.println(device.mqttClient.state());
    toReconnect = true;
  }

  if(toReconnect) {
    connect();
  }

  device.loop();

  int currentRead = digitalRead(BUTTON_PIN);

  if(currentRead != buttonState) {
    buttonState = currentRead;
    if(buttonState) {
      buttonPressed();
    }
  }

  delay(100);
}
