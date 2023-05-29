#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "CYTA0A49";    
const char* password = "JAZTpDaSUENFREKQ";
const char* mqttServer = "a4a74c8f98954cf7a210421441e7a849.s1.eu.hivemq.cloud";
const int mqttPort = 8883;               
const char* mqttUser = "nodemcu"; 
const char* mqttPassword = "NODEmcu2023"; 

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {

  Serial.begin(115200);

  // wait 1 sec after powered by arduino
  delay(1000);

  // wifi connect and get ip
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("mcu-wifi: ok");

  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());

  // certificate and mqtt server
  espClient.setInsecure(); 
  client.setServer(mqttServer, mqttPort);
}

void loop() {

  // publish to broker when arduino sends data
  if (Serial.available()) {

    float number = Serial.parseFloat();

    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    // Float to string
    String message = String(number, 1);
    Serial.println(message);

    // Publish message to MQTT broker
    String topic = "caps";
    boolean retain = true;
    client.publish(topic.c_str(), message.c_str(), retain);

  }

  // dont read random possible data
  delay(1000);
}

void reconnect() {
  while (!client.connected()) {

    if (client.connect("NodeMCU_Client", mqttUser, mqttPassword)) {
      Serial.print("mcu-con: ok ");
    } 
    
    else 
    {
      Serial.print("mcu-con: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}
