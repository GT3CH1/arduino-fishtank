#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include "secret.h"
int LIGHT_PIN = 2;
int PUMP_PIN = 3;
const int REPORT_INTERVAL = 60;
WiFiClient net;
MqttClient  client(net);
byte mac[6];

unsigned long lastMillis = 0;
int status = WL_IDLE_STATUS;  

void publishLightConfig()
{

  DynamicJsonDocument jsonBuffer(1024);
  jsonBuffer["name"] = "fishtank199959";
  jsonBuffer["stat_t"] = LIGHT_REPORT_TOPIC;
  jsonBuffer["cmd_t"] = light_topic;
  jsonBuffer["pl_avail"] = "online";
  jsonBuffer["pl_not_avail"] = "offline";
  jsonBuffer["uniq_id"] = "fishtank199959";
  String payload;
  serializeJson(jsonBuffer, payload);
  Serial.println(payload);
  bool retained = false;
  int qos = 1;
  bool dup = false;

  client.beginMessage("homeassistant/light/fishtank199959/config", payload.length(), retained, qos, dup);
  client.print(payload);
  client.endMessage();
  
}

void publishPumpConfig()
{

  DynamicJsonDocument jsonBuffer(1024);
  jsonBuffer["name"] = "fishtank299959";
  jsonBuffer["stat_t"] = PUMP_REPORT_TOPIC;
  jsonBuffer["cmd_t"] = pump_topic;
  jsonBuffer["pl_avail"] = "online";
  jsonBuffer["pl_not_avail"] = "offline";
  jsonBuffer["uniq_id"] = "fishtank299959";
  String payload;
  serializeJson(jsonBuffer, payload);
  Serial.println(payload);
  bool retained = false;
  int qos = 1;
  bool dup = false;
  client.beginMessage("homeassistant/switch/fishtank299959/config", payload.length(), retained, qos, dup);
  client.print(payload);
  client.endMessage();
}

void connection_setup()
{
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to wifi network: " + String(ssid));
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(10000);
  }
  Serial.println();
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);
  client.setUsernamePassword(mqtt_user,mqtt_pass);
  while (!client.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(client.connectError());
  }
  Serial.println("Connected!");
  Serial.println("Sending discovery message");
  publishLightConfig();
  publishPumpConfig();
  client.onMessage(onMessage);
  int subscribeQos = 1;
  client.subscribe(light_topic, subscribeQos);
  client.subscribe(pump_topic, subscribeQos);
}

void setup() {
  Serial.begin(115200);
  connection_setup();
  String payload = "OFF";
  bool retained = false;
  int qos = 1;
  bool dup = false;
  client.beginMessage(LIGHT_REPORT_TOPIC, payload.length(), retained, qos, dup);
  client.print(payload);
  client.endMessage();
  payload = "ON";
  client.beginMessage(PUMP_REPORT_TOPIC, payload.length(), retained, qos, dup);
  client.print(payload);
  client.endMessage();
  pinMode(LIGHT_PIN,OUTPUT);
  digitalWrite(LIGHT_PIN,HIGH);
  pinMode(PUMP_PIN,OUTPUT);
  digitalWrite(PUMP_PIN,HIGH);

}

void onMessage(int messageSize)
{
  String recvTopic = client.messageTopic();
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(recvTopic);
  Serial.print("', duplicate = ");
  Serial.print(client.messageDup() ? "true" : "false");
  Serial.print(", QoS = ");
  Serial.print(client.messageQoS());
  Serial.print(", retained = ");
  Serial.print(client.messageRetain() ? "true" : "false");
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
  String payload;
  while (client.available()) {
    payload += String((char)client.read());
  }
  bool retained = false;
  int qos = 1;
  bool dup = false;
  Serial.println(payload);
  String light_topic_check = String(light_topic);
  recvTopic.trim();
  light_topic_check.trim();
  Serial.println("|"+recvTopic + ":"+ light_topic_check+"|");
  if(recvTopic == light_topic){
    digitalWrite(LIGHT_PIN, payload == "ON" ? LOW : HIGH);
    client.beginMessage(LIGHT_REPORT_TOPIC, payload.length(), retained, qos, dup);
    client.print(payload);
    client.endMessage();
    Serial.println("Setting tank light " + payload);
  }
  if (recvTopic == pump_topic){
    digitalWrite(PUMP_PIN, payload != "ON" ? LOW : HIGH);
    client.beginMessage(PUMP_REPORT_TOPIC, payload.length(), retained, qos, dup);
    client.print(payload);
    client.endMessage();
    Serial.println("Setting tank pump " + payload);   
  }
  Serial.println();

  Serial.println();  
}

void loop() {
    client.poll();
  // publish a message roughly every second.
  if (millis() - lastMillis > 1*1000) {
    lastMillis = millis();
    connection_setup();
  }
}
