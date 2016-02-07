#include "StructureDevice.h"

CommandCallback StructureDevice::commandCallback = NULL;

void commandReceived(char* topic, byte* payload, unsigned int length) {

  StructureCommand command;
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject((char*)payload);

  if(root.success()) {
    command.name = root["name"];
    command.time = root["$time"];
    command.payload = &(root["payload"].asObject());

    StructureDevice::commandCallback(&command);
  }
}

StructureDevice::StructureDevice(const char* id) {
  this->id = id;
  stateTopic = String(STRUCTURE_TOPIC_PREFIX);
  stateTopic.concat(id);
  stateTopic.concat(STRUCTURE_TOPIC_STATE);
  commandTopic = String(STRUCTURE_TOPIC_PREFIX);
  commandTopic.concat(id);
  commandTopic.concat(STRUCTURE_TOPIC_COMMAND);
}

const char* StructureDevice::getId() {
  return id;
}

void StructureDevice::onCommand(CommandCallback callback) {
  StructureDevice::commandCallback = callback;
}

void StructureDevice::connect(Client& client, const char* key, const char* secret) {
  this->connect(client, key, secret, STRUCTURE_BROKER, STRUCTURE_PORT);
}

void StructureDevice::connectSecure(Client& client, const char* key, const char* secret) {
  this->connect(client, key, secret, STRUCTURE_BROKER, STRUCTURE_PORT_SECURE);
}

void StructureDevice::connect(Client &client, const char* key, const char* secret, const char *brokerUrl, int brokerPort) {
    mqttClient.setClient(client);
    mqttClient.setServer(brokerUrl, brokerPort);
    mqttClient.connect(id, key, secret);
    mqttClient.setCallback(commandReceived);
    int topicLen = commandTopic.length() + 1;
    char topicBuf[topicLen];
    commandTopic.toCharArray(topicBuf, topicLen);
    mqttClient.subscribe(topicBuf);
}

void StructureDevice::disconnect() {
  mqttClient.disconnect();
}

boolean StructureDevice::connected() {
  return mqttClient.connected();
}

boolean StructureDevice::loop() {
  return mqttClient.loop();
}

void StructureDevice::sendState(JsonObject& state) {

  // Create a wrapper object and add provided state to "data" property.
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["data"] = state;

  // Convert to string to send over mqtt.
  String stateStr;
  root.printTo(stateStr);

  mqttClient.publish(this->stateTopic.c_str(), stateStr.c_str());
}
