
#ifndef LOSANT_DEVICE_H_
#define LOSANT_DEVICE_H_

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Client.h"
#include "Losant.h"
#include "LosantCommand.h"

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
		PubSubClient mqttClient;
	private:
		const char* id;
		void setTopics();
		void connect(Client &client, const char* key,
			const char* secret, const char *brokerUrl, int brokerPort);
};

#endif /* LOSANT_DEVICE_H_ */
