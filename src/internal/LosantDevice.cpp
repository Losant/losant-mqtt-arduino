#include "LosantDevice.h"

CommandCallback LosantDevice::commandCallback = NULL;

void commandReceived(char* topic, byte* payload, unsigned int length) {

  LosantCommand command;
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject((char*)payload);

  if(root.success()) {
    command.name = root["name"];
    command.time = root["$time"];
    command.payload = &(root["payload"].asObject());

    LosantDevice::commandCallback(&command);
  }
}

LosantDevice::LosantDevice() {
  this->id = NULL;
}

LosantDevice::LosantDevice(const char* id) {
  this->id = id;
  this->setTopics();
}

void LosantDevice::setId(const char* id) {
  this->id = id;
  this->setTopics();
}

void LosantDevice::setTopics() {
  this->stateTopic = String(LOSANT_TOPIC_PREFIX);
  this->stateTopic.concat(this->id);
  this->stateTopic.concat(LOSANT_TOPIC_STATE);
  this->commandTopic = String(LOSANT_TOPIC_PREFIX);
  this->commandTopic.concat(this->id);
  this->commandTopic.concat(LOSANT_TOPIC_COMMAND);
}

const char* LosantDevice::getId() {
  return id;
}

void LosantDevice::onCommand(CommandCallback callback) {
  LosantDevice::commandCallback = callback;
}

void LosantDevice::connect(Client& client, const char* key, const char* secret) {
  this->connect(client, key, secret, LOSANT_BROKER, LOSANT_PORT);
}

void LosantDevice::connectSecure(Client& client, const char* key, const char* secret) {
  this->connect(client, key, secret, LOSANT_BROKER, LOSANT_PORT_SECURE);
}

void LosantDevice::connect(Client &client, const char* key, const char* secret, const char *brokerUrl, int brokerPort) {

  if(!this->id) {
    return;
  }

  mqttClient.setClient(client);
  mqttClient.setServer(brokerUrl, brokerPort);
  mqttClient.connect(this->id, key, secret);
  mqttClient.setCallback(commandReceived);
  int topicLen = commandTopic.length() + 1;
  char topicBuf[topicLen];
  commandTopic.toCharArray(topicBuf, topicLen);
  mqttClient.subscribe(topicBuf);
}

void LosantDevice::disconnect() {
  mqttClient.disconnect();
}

boolean LosantDevice::connected() {
  return mqttClient.connected();
}

boolean LosantDevice::loop() {
  return mqttClient.loop();
}

void LosantDevice::sendState(JsonObject& state) {

  // Create a wrapper object and add provided state to "data" property.
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["data"] = state;

  // Convert to string to send over mqtt.
  String stateStr;
  root.printTo(stateStr);

  mqttClient.publish(this->stateTopic.c_str(), stateStr.c_str());
}
