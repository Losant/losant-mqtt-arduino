Structure Arduino SDK
============

The Structure Arduino SDK provides a simple way for your Arduino-based things to connect and communicate with the [Structure IoT developer platform](https://getstructure.io).

## Installation
The Structure Arduino SDK is distributed as an Arduino library. It can be installed in two ways:

1. Download a zip of this repository and include it into your Arduino Sketch. Select `Sketch -> Include Library -> Add .ZIP` library from the Arduino menu.

2. Clone the contents of the repository into Arduino's library folder on your system. This location changes based on OS, but on Mac it's typically at `Documents / Arduino / libraries`.

Once installed, using the library requires a single include directive.

```arduino
#include <Structure.h>
```

## Dependencies

The Structure Arduino SDK depends on [ArduinoJson](https://github.com/bblanchon/ArduinoJson) and [PubSubClient](https://github.com/knolleary/pubsubclient). These libraries must be installed before using the Structure SDK. Please refer to their documentation for specific installation instructions.

## Example

Below is a basic example of using the Structure Arduino SDK. For specific examples for various boards, please refer to the [`examples`](https://github.com/GetStructure/structure-sdk-arduino/tree/master/examples) folder.

```arduino
#include <WiFi101.h>
#include <Structure.h>

// WiFi credentials.
const char* WIFI_SSID = "WIFI_SSID";
const char* WIFI_PASS = "WIFI_PASS";

// Structure credentials.
const char* STRUCTURE_DEVICE_ID = "my-device-id";
const char* STRUCTURE_ACCESS_KEY = "my-app-key";
const char* STRUCTURE_ACCESS_SECRET = "my-app-secret";

const int BUTTON_PIN = 14;
const int LED_PIN = 12;

bool ledState = false;

WiFiSSLClient wifiClient;

StructureDevice device(STRUCTURE_DEVICE_ID);

// Toggles and LED on or off.
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

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Connect to Structure.
  device.connectSecure(wifiClient, STRUCTURE_ACCESS_KEY, STRUCTURE_ACCESS_SECRET);

  while(!device.connected()) {
    delay(500);
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
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
```

## Structure State and Commands
State and commands are the main two communication constructs of the Structure platform. A state represents a current snapshot of the device at a moment in time. On typical IoT devices, attributes of a device's state typically correspond to individual sensors (e.g. "temperature", "light level", or "sound level"). States can be reported to Structure as often as needed.

Commands allow you to control your device remotely. What commands a device supports is entirely up to the device's firmware. A command is comprised of a name and an optional payload. The name indicates what command the device should invoke (e.g. "start recording") and the payload provide parameters to the command (e.g. `{ "resolution": 1080 }`).

## API Documentation
* [`StructureDevice`](#structuredevice)
  * [`StructureDevice::StructureDevice()`](#structuredevice-structuredevice)
  * [`StructureDevice::connect()`](#structuredevice-connect)
  * [`StructureDevice::connectSecure()`](#structuredevice-connectsecure)
  * [`StructureDevice::onCommand()`](#structuredevice-oncommand)
  * [`StructureDevice::sendState()`](#structuredevice-sendstate)

<a name="structuredevice"></a>
## StructureDevice
The StructureDevice class represents a single connection to the Structure platform. Use this class to report state information and subscribe to commands.

<a name="structuredevice-structuredevice"></a>
### StructureDevice::StructureDevice(const char\* id)
Structure device constructor. The only parameter is the device ID. A Structure device ID can be obtained by registering your device using the Structure dashboard.

```arduino
StructureDevice device('my-device-id');
```

<a name="structuredevice-connect"></a>
### StructureDevice::connect(Client& client, const char\* key, const char\* secret)
Creates an unsecured connection to the Structure platform.

```arduino
WiFiClient client;

...

StructureDevice device('my-device-id');
device.connect(client, 'my-access-key', 'my-access-secret');
```

<a name="structuredevice-connectsecure"></a>
### StructureDevice::connectSecure(Client& client, const char\* key, const char\* secret)
Creates a TLS encrypted connection to the Structure platform.

```arduino
WiFiSSLClient client;

...

StructureDevice device('my-device-id');
device.connectSecure(client, 'my-access-key', 'my-access-secret');
```

<a name="structuredevice-oncommand"></a>
### StructureDevice::onCommand(CommandCallback callback)
Registers a function that will be called whenever a command is received from the Structure platform.

```arduino
void handleCommand(StructureCommand *command) {
  Serial.print("Command received: ");
  Serial.println(command->name);
  Serial.println(command->time);
  JsonObject& payload = command->payload;
}

StructureDevice device('my-device-id');
device.connectSecure(client, 'my-access-key', 'my-access-secret');
device.onCommand(&handleCommand);
```

The command callback function is passed a `StructureCommand` object with details about the command. These include `name`, `time`, and `payload`. `name` is a string containing the command's name. `time` is the UTC ISO string of the date and time when the command was received by the Structure platform. `payload` is a JsonObject with whatever arguments was passed to the command when it was sent.

<a name="structuredevice-sendstate"></a>
### StructureDevice::sendState(JsonObject& state)
Sends a state update to Structure. The state of an object is defined as a simple Json object with keys and values. Refer to the [ArduinoJson](https://github.com/bblanchon/ArduinoJson) library for detailed documentation.

```arduino
StaticJsonBuffer<100> jsonBuffer;
JsonObject& state = jsonBuffer.createObject();
state["temperature"] = 72;

// Send the state to Structure.
device.sendState(state);
```
