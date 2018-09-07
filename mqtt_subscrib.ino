/* MQTT Client: Subscrib
 * Francis David (www.fdavid.com.br) 
 * 
 * Build with IDE Arduino 1.8.4
 * 
 * REQUIREMENT
 *  Board ESP8266
 *  Library PubSubClient (Sketch >> Include Library >> Manage Libraies)
 *  Server MQTT
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
// Connect to the WiFi
const char* ssid = "EnterTheMatrix";
const char* password = "555escolhido";
const char* mqtt_server = "192.168.15.10";

//Services
WiFiClient espClient;
PubSubClient mqtt_client(espClient);
 
 
void callback(char* topic, byte* payload, unsigned int length) 
{  
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 
 for (int i=0;i<length;i++) {
    char receivedChar = (char)payload[i];
    Serial.print(receivedChar);    
  }
 
  Serial.println();
}
 
 
void reconnect() 
{
 // Loop until we're reconnected
 while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqtt_client.connect("ESP8266 Client")) {
      Serial.println("connected");
      
      // ... and subscribe to topic
      mqtt_client.subscribe("debug");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      
      // Wait 5 seconds before retrying
      delay(5000);
    }
 }
 
}
 
void setup() 
{
 Serial.begin(9600);
 
 mqtt_client.setServer(mqtt_server, 1883);
 mqtt_client.setCallback(callback);
}
 
void loop()
{
 if (!mqtt_client.connected()) {
  reconnect();
 }
 
 mqtt_client.loop();
}
