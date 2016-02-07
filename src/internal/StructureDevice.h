
#ifndef STRUCTURE_DEVICE_H_
#define STRUCTURE_DEVICE_H_

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Client.h"
#include "Structure.h"
#include "StructureCommand.h"

typedef void (*CommandCallback)(StructureCommand*);

class StructureDevice {
	public:
		StructureDevice(const char* id);
		void connect(Client& client, const char* key, const char* secret);
		void connectSecure(Client &client, const char* key, const char* secret);
		void disconnect();
		boolean loop();
		void sendState(JsonObject& state);
		void onCommand(CommandCallback);
		const char* getId();
		boolean connected();
		String stateTopic;
		String commandTopic;
		static CommandCallback commandCallback;
		PubSubClient mqttClient;
	private:
		const char* id;
		void connect(Client &client, const char* key,
			const char* secret, const char *brokerUrl, int brokerPort);
};

#endif /* STRUCTURE_DEVICE_H_ */
