# Losant Arduino MQTT Client

The Losant Arduino MQTT client provides a simple way for your Arduino-based
devices to connect and communicate with
the [Losant Enterprise IoT Platform](https://www.losant.com).

## Installation for [PlatformIO](http://platformio.org/lib/show/277/losant-mqtt-arduino/installation)

The latest stable version is available in PlatformIO and can be installed adding dependency (`lib_deps`) to your project configuration file `platformio.ini`

```ini
[env:myenv]
platform = ...
board = ...
framework = arduino
lib_deps = losant-mqtt-arduino
```

Using [PlatformIO IDE](http://platformio.org/platformio-ide) requires a single include directive:

```C
#include <Losant.h>
```

## Installation for the Arduino IDE

The Losant Arduino MQTT client is distributed as an Arduino library.
It can be installed in two ways:

1. Download a zip of this repository and include it into your Arduino Sketch. Select `Sketch -> Include Library -> Add .ZIP` library from the Arduino menu.
1. Clone the contents of the repository into Arduino's library folder on your system. This location changes based on OS, but on Mac and Windows it's typically at `Documents / Arduino / libraries`.

Once installed, using the library requires a single include directive.

```C
#include <Losant.h>
```

## Dependencies

The Losant Arduino MQTT client depends on
[ArduinoJson](https://github.com/bblanchon/ArduinoJson)
and [MQTT Client](https://github.com/256dpi/arduino-mqtt). These libraries
must be installed before using the Losant MQTT client.  When using Platform.io,
these dependencies are installed automatically, but when using the Arduino IDE
they must be installed manually.  Please refer to their documentation for
specific installation instructions.

### Important Considerations

You’ll possibly hit the default MQTT packet size limit defined in the Losant Arduino MQTT Client library. Unfortunately the packet is blocked before reaching any of your code, so it’s hard to debug. It simply looks like the command was never received. For example:

`{ "foo" : "bar" }` works, whereas `{ "somethingLarger" : "with a longer value" }` doesn’t work. This is because the default packet size is 256, which provides enough room for the command meta info and a small payload. Fortunately, this is easily fixed by defining a larger value for `MQTT_MAX_PACKET_SIZE` in your Arduino code.
  
## Examples

For specific examples for various boards, please refer to the [`examples`](https://github.com/Losant/losant-mqtt-arduino/tree/master/examples) folder.

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

* [`LosantDevice`](#losantdevice)
  * [`LosantDevice::LosantDevice()`](#losantdevice-losantdevice)
  * [`LosantDevice::getId()`](#losantdevice-getId)
  * [`LosantDevice::setId()`](#losantdevice-setId)
  * [`LosantDevice::connect()`](#losantdevice-connect)
  * [`LosantDevice::connectSecure()`](#losantdevice-connectsecure)
  * [`LosantDevice::onCommand()`](#losantdevice-oncommand)
  * [`LosantDevice::sendState()`](#losantdevice-sendstate)
  * [`LosantDevice::loop()`](#losantdevice-loop)

<a name="losantdevice"></a>

## LosantDevice

The LosantDevice class represents a single connection to the Losant platform.
Use this class to report state information and subscribe to commands.

<a name="losantdevice-losantdevice"></a>

### LosantDevice::LosantDevice(const char\* id)

Losant device constructor. The only parameter is the device ID. A Losant device
ID can be obtained by registering your device using the Losant dashboard.

```C
LosantDevice device('my-device-id');
```

### LosantDevice::LosantDevice()

Losant device constructor that does not require a device ID. The ID can
be set at a later time using [setId()](#losantdevice-setId) The ID must be
set before [connect()](#losantdevice-connect) can be called.

```C
LosantDevice device;
```

<a name="losantdevice-getId"></a>

### LosantDevice.getId()

Gets the device ID that was set through the constructor or
[setId()](#losantdevice-setId) call.

```C
LosantDevice device('my-device-id');
const *char deviceId = device.getId();
```

<a name="losantdevice-setId"></a>

### LosantDevice.setId()

Sets the device ID. Typically called if the ID was not passed through
the constructor.

```C
LosantDevice device;
device.setId('my-device-id');
```

```C
LosantDevice device('my-device-id');
const *char deviceId = device.getId();
```

<a name="losantdevice-connect"></a>

### LosantDevice::connect(Client& client, const char\* key, const char\* secret)

Creates an unsecured connection to the Losant platform.

```C
WiFiClient client;

...

LosantDevice device('my-device-id');
device.connect(client, 'my-access-key', 'my-access-secret');
```

<a name="losantdevice-connectsecure"></a>

### LosantDevice::connectSecure(Client& client, const char\* key, const char\* secret)

Creates a TLS encrypted connection to the Losant platform.

```C
WiFiSSLClient client;

...

LosantDevice device('my-device-id');
device.connectSecure(client, 'my-access-key', 'my-access-secret');
```

<a name="losantdevice-oncommand"></a>

### LosantDevice::onCommand(CommandCallback callback)

Registers a function that will be called whenever a command is received
from the Losant platform.

```C
void handleCommand(LosantCommand *command) {
  Serial.print("Command received: ");
  Serial.println(command->name);
  Serial.println(command->time);

  // { "foo" : 10 }
  JsonObject payload = *command->payload;
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

```C
StaticJsonDocument<200> jsonBuffer;
JsonObject state = jsonBuffer.to<JsonObject>();
state["temperature"] = 72;

// Send the state to Losant.
device.sendState(state);
```

<a name="losantdevice-loop"></a>

### LosantDevice::loop()

Loops the underlying Client to perform any required MQTT communication. Must
be called periodically, no less than once every few seconds.

```C
device.loop();
```

<br/>

*****

Copyright (c) 2021 Losant IoT, Inc

<https://www.losant.com>
