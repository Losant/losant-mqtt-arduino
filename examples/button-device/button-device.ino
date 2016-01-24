#include <PubSubClient.h>
#include <SPI.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Structure.h>

// Structure setup
#define DEVICE_ID "<DEVICE ID>"
#define ACCESS_KEY "<ACCESS KEY>"
#define ACCESS_SECRET "<ACCESS SECRET>"

// WiFi setup.
char ssid[] = "<SSID>";
char pass[] = "<PASSWORD>";
WiFiClient wifiClient;

// Button setup
const int buttonPin = 4;
int buttonState;

StructureDevice myDevice(DEVICE_ID);

void handleCommand (JsonObject& command) {
  Serial.println("Handling incoming command ...");
}

void connect() {
  Serial.print("Connecting to Wifi ... ");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println(" SUCCESS");

  Serial.print("Connecting to Structure ...");
  myDevice.connect(wifiClient, ACCESS_KEY, ACCESS_SECRET);
  while (!myDevice.connected()) {
    delay(500);
  }
  Serial.println(" SUCCESS");
}

void setup() {
  Serial.begin(9600);

  // initialize button
  pinMode(buttonPin, INPUT);

  // initialize device
  myDevice.onCommand(handleCommand);

  // connect to Structure
  connect();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Lost wifi connection!");
    connect();
  }
  myDevice.loop();
  int read = digitalRead(buttonPin);
  if (read != buttonState) {
    buttonState = read;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& state = jsonBuffer.createObject();
    JsonObject& data = state.createNestedObject("data");
    data["pressed"] = buttonState;
    myDevice.sendState(state);
  }
}
