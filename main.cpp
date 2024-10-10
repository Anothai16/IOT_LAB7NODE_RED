#include <Arduino.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
 
const char *mqtt_broker = "broker.emqx.io";
const char *mqtt_topic_pub = "demo/signature";
const char *mqtt_topic_sub = "demo/signature/sub";
const int mqtt_port = 1883;
 
WiFiClient espClient;
PubSubClient mqtt_client(espClient);
String mqttMessage;
 
void setupWiFiManager();
void connectToMQTTBroker();
void mqttCallback(char *topic, byte *payload, unsigned int length);
 
JsonDocument doc_pub, doc_sub , address;
char jsonBuffer[512];
 
int MAIN = 0;
int state = 99999;
void setup() {
    Serial.begin(115200);
    setupWiFiManager();
 
    mqtt_client.setServer(mqtt_broker, mqtt_port);
    mqtt_client.setCallback(mqttCallback);
    connectToMQTTBroker();
    state = MAIN;
}
 
void loop() {
  if (state == MAIN){
    mqtt_client.loop();
 
    // สร้างข้อมูล JSON ของนักศึกษา
   doc_pub["firstName"] = "Anothai & BIGGO";
    doc_pub["lastName"] = "Paolo";
    doc_pub["address"]["streetAddress"] = " University Street";
    doc_pub["address"]["city"] = "Bangkok";
    doc_pub["address"]["state"] = "Thailand";
    doc_pub["address"]["postalCode"] = "10140";
 
    serializeJson(doc_pub, jsonBuffer);
    mqtt_client.publish(mqtt_topic_pub, jsonBuffer);
 
    delay(5000);
    state = MAIN;
  }
}
 
void setupWiFiManager() {
    WiFiManager wm;
    bool res = wm.autoConnect("Signature");
    if(!res) {
        Serial.println("Failed to connect");
    } else {
        Serial.println("Connected to WiFi");
    }
}
 
void connectToMQTTBroker() {
    while (!mqtt_client.connected()) {
        String client_id = "esp8266-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
        if (mqtt_client.connect(client_id.c_str())) {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic_sub);
        } else {
            Serial.print("Failed to connect to MQTT broker, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}
 
void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    mqttMessage = "";
    for (unsigned int i=0;i<length;i++) {
        mqttMessage += (char)payload[i];  // Convert *byte to string
    }
    deserializeJson(doc_sub, mqttMessage);
    const char* doc_sub_message = doc_sub["message"];
    Serial.println(doc_sub_message);
}
