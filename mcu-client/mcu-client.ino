#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "secrets.h"

// Set secrets
const char* ssid = SSID;   
const char* password = PASS;
const char* mqttServer = MQTTSERVER;
const int mqttPort = MQTTPORT;               
const char* mqttUser = MQTTUSER; 
const char* mqttPassword = MQTTPASS; 

// Set wifi and mqtt clients
WiFiClientSecure espClient;
PubSubClient client(espClient);

// initialize nodemcu
void setup() {

  Serial.begin(115200);

  // wifi connect and get ip
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  Serial.println("mcu-wifi: ok");

  // skip certificate verification and set mqtt broker
  espClient.setInsecure(); 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  if (!client.connected()) {
      reconnect();
  }
  client.subscribe("tare");
  client.subscribe("timeout");
}

// main loop
void loop() {

  // broker connection
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // publish to broker when arduino sends data
  if (Serial.available()) {

    // received number
    int number = Serial.parseInt();
    String message = String(number);

    // Publish message to MQTT broker
    String topic = "caps";
    boolean retain = true;
    client.publish(topic.c_str(), message.c_str(), retain);
  }
}

// reconnect to mqtt broker
void reconnect() {
  while (!client.connected()) {

    if (client.connect("NodeMCU_Client", mqttUser, mqttPassword)) {
      Serial.println("mcu-con: ok ");
    } 

    else 
    {
      Serial.println("mcu-con: " + client.state());
      delay(5000);
    }

  }
}

// Callback function for handling incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {

  // turn topic and mess to string
  String topic_str = String(topic);

  String mess = "";
  for (int i=0; i<length; i++) {
    mess += char(payload[i]);
  }

  // check topic and act accordingly
  // tare -> true, to use tare() function on arduino
  if (topic_str == "tare") {

    if (mess == "true") {
      Serial.print("tare");
    }

  }
  // timeout -> int X, to set mqtt messages every X seconds
  else if (topic_str == "timeout") {

    if (mess.toInt() != 0) {
      Serial.print("timeout " + mess);
    }

  }
}