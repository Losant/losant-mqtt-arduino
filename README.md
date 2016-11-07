# Losant Arduino MQTT Client

The Losant Arduino MQTT client provides a simple way for your Arduino-based
things to connect and communicate with
the [Losant IoT developer platform](https://www.losant.com).

## Installation for [PlatformIO](http://platformio.org/lib/show/277/Losant/installation)

The latest stable version is available in PlatformIO and can be installed adding dependency (`lib_deps`) to your project configuration file `platformio.ini`

```ini
[env:myenv]
platform = ...
board = ...
framework = arduino
lib_deps = Losant
```

Using [PlatformIO IDE](http://platformio.org/platformio-ide) requires a single include directive:

```arduino
#include <Losant.h>
```

## Installation for the Arduino IDE

The Losant Arduino MQTT client is distributed as an Arduino library.
It can be installed in two ways:

1.  Download a zip of this repository and include it into your Arduino Sketch.
Select `Sketch -> Include Library -> Add .ZIP` library from the Arduino menu.

2.  Clone the contents of the repository into Arduino's library folder on your
system. This location changes based on OS, but on Mac it's typically at
`Documents / Arduino / libraries`.

Once installed, using the library requires a single include directive.

```arduino
#include <Losant.h>
```

## Dependencies

The Losant Arduino MQTT client depends on
[ArduinoJson](https://github.com/bblanchon/ArduinoJson)
and [PubSubClient](https://github.com/knolleary/pubsubclient). These libraries
must be installed before using the Losant MQTT client.  When using Platform.io,
these dependencies are installed automatically, but when using the Arduino IDE
they must be installed manually.  Please refer to their documentation for
specific installation instructions.

## Example

Below is a basic example of using the Losant Arduino MQTT client. For specific
examples for various boards, please refer to the [`examples`](https://github.com/Losant/losant-mqtt-arduino/tree/master/examples)
folder.

```arduino
#include <WiFi101.h>
#include <Losant.h>

// WiFi credentials.
const char* WIFI_SSID = "WIFI_SSID";
const char* WIFI_PASS = "WIFI_PASS";

// Losant credentials.
const char* LOSANT_DEVICE_ID = "my-device-id";
const char* LOSANT_ACCESS_KEY = "my-app-key";
const char* LOSANT_ACCESS_SECRET = "my-app-secret";

const int BUTTON_PIN = 14;
const int LED_PIN = 12;

bool ledState = false;

WiFiSSLClient wifiClient;

LosantDevice device(LOSANT_DEVICE_ID);

// Toggles and LED on or off.
void toggle() {
  Serial.println("Toggling LED.");
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
}

// Called whenever the device receives a command from the Losant platform.
void handleCommand(LosantCommand *command) {
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

  // Connect to Losant.
  device.connectSecure(wifiClient, LOSANT_ACCESS_KEY, LOSANT_ACCESS_SECRET);

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
  // from the Losant platform.
  device.onCommand(&handleCommand);

  connect();
}

void buttonPressed() {
  Serial.println("Button Pressed!");

  // Losant uses a JSON protocol. Construct the simple state object.
  // { "button" : true }
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["button"] = true;

  // Send the state to Losant.
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
    Serial.println("Disconnected from Losant");
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

## Losant State and Commands

State and commands are the main two communication constructs of the Losant
platform. A state represents a current snapshot of the device at a moment in
time. On typical IoT devices, attributes of a device's state typically
correspond to individual sensors (e.g. "temperature", "light level", or
"sound level"). States can be reported to Losant as often as needed.

Commands allow you to control your device remotely. What commands a device
supports is entirely up to the device's firmware. A command is comprised of
a name and an optional payload. The name indicates what command the device
should invoke (e.g. "start recording") and the payload provide parameters
to the command (e.g. `{ "resolution": 1080 }`).

## API Documentation

*   [`LosantDevice`](#losantdevice)

    *   [`LosantDevice::LosantDevice()`](#losantdevice-losantdevice)
    *   [`LosantDevice::getId()`](#losantdevice-getId)
    *   [`LosantDevice::setId()`](#losantdevice-setId)
    *   [`LosantDevice::connect()`](#losantdevice-connect)
    *   [`LosantDevice::connectSecure()`](#losantdevice-connectsecure)
    *   [`LosantDevice::onCommand()`](#losantdevice-oncommand)
    *   [`LosantDevice::sendState()`](#losantdevice-sendstate)
    *   [`LosantDevice::loop()`](#losantdevice-loop)

<a name="losantdevice"></a>

## LosantDevice

The LosantDevice class represents a single connection to the Losant platform.
Use this class to report state information and subscribe to commands.

<a name="losantdevice-losantdevice"></a>

### LosantDevice::LosantDevice(const char\* id)

Losant device constructor. The only parameter is the device ID. A Losant device
ID can be obtained by registering your device using the Losant dashboard.

```arduino
LosantDevice device('my-device-id');
```

### LosantDevice::LosantDevice()

Losant device constructor that does not require a device ID. The ID can
be set at a later time using [setId()](#losantdevice-setId) The ID must be
set before [connect()](#losantdevice-connect) can be called.

```arduino
LosantDevice device;
```

<a name="losantdevice-getId"></a>

### LosantDevice.getId()

Gets the device ID that was set through the constructor or
[setId()](#losantdevice-setId) call.

```arduino
LosantDevice device('my-device-id');
const *char deviceId = device.getId();
```

<a name="losantdevice-setId"></a>

### LosantDevice.setId()

Sets the device ID. Typically called if the ID was not passed through
the constructor.

```arduino
LosantDevice device;
device.setId('my-device-id');
```

```arduino
LosantDevice device('my-device-id');
const *char deviceId = device.getId();
```

<a name="losantdevice-connect"></a>

### LosantDevice::connect(Client& client, const char\* key, const char\* secret)

Creates an unsecured connection to the Losant platform.

```arduino
WiFiClient client;

...

LosantDevice device('my-device-id');
device.connect(client, 'my-access-key', 'my-access-secret');
```

<a name="losantdevice-connectsecure"></a>

### LosantDevice::connectSecure(Client& client, const char\* key, const char\* secret)

Creates a TLS encrypted connection to the Losant platform.

```arduino
WiFiSSLClient client;

...

LosantDevice device('my-device-id');
device.connectSecure(client, 'my-access-key', 'my-access-secret');
```

<a name="losantdevice-oncommand"></a>

### LosantDevice::onCommand(CommandCallback callback)

Registers a function that will be called whenever a command is received
from the Losant platform.

```arduino
void handleCommand(LosantCommand *command) {
  Serial.print("Command received: ");
  Serial.println(command->name);
  Serial.println(command->time);

  // { "foo" : 10 }
  JsonObject& payload = *command->payload;
  long bar = payload["foo"];
}

LosantDevice device('my-device-id');
device.connectSecure(client, 'my-access-key', 'my-access-secret');
device.onCommand(&handleCommand);
```

The command callback function is passed a `LosantCommand` object with details
about the command. These include `name`, `time`, and `payload`. `name` is a
string containing the command's name. `time` is the UTC ISO string of the date
and time when the command was received by the Losant platform. `payload` is a
JsonObject with whatever arguments was passed to the command when it was sent.

<a name="losantdevice-sendstate"></a>

### LosantDevice::sendState(JsonObject& state)

Sends a state update to Losant. The state of an object is defined as a simple
Json object with keys and values. Refer to the [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
library for detailed documentation.

```arduino
StaticJsonBuffer<100> jsonBuffer;
JsonObject& state = jsonBuffer.createObject();
state["temperature"] = 72;

// Send the state to Losant.
device.sendState(state);
```

<a name="losantdevice-loop"></a>

### LosantDevice::loop()

Loops the underlying Client to perform any required MQTT communication. Must
be called periodically, no less than once every few seconds.

```arduino
device.loop();
```

<br/>

*****

Copyright (c) 2016 Losant IoT, Inc

<https://www.losant.com>
