
#ifndef LOSANT_DEVICE_H_
#define LOSANT_DEVICE_H_

#include <MQTT.h>
#include <ArduinoJson.h>
#include "Client.h"
#include "Losant.h"
#include "LosantCommand.h"

// MQTT_MAX_PACKET_SIZE : Maximum packet size
#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 256
#endif

typedef void (*CommandCallback)(LosantCommand*);

class LosantDevice {
	public:
		LosantDevice(const char* id);
		LosantDevice();
		void connect(Client& client, const char* key, const char* secret);
		void connectSecure(Client &client, const char* key, const char* secret);
		void disconnect();
		boolean loop();
		void sendState(JsonObject& state);
		void onCommand(CommandCallback);
		void setId(const char* id);
		const char* getId();
		boolean connected();
		String stateTopic;
		String commandTopic;
		static CommandCallback commandCallback;
		MQTTClient mqttClient = MQTTClient(MQTT_MAX_PACKET_SIZE);
	private:
		const char* id;
		void setTopics();
		void connect(Client &client, const char* key,
			const char* secret, const char *brokerUrl, int brokerPort);
};

#endif /* LOSANT_DEVICE_H_ */
