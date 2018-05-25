
#include "Arduino.h"   // replace this by "WProgram.h" when your IDE is older then 1.0
#define DEBUG true // flag to turn on/off debugging

#define Serial if(DEBUG)Serial

#include <WiFiIOTCredentials.h>

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>

#define MQTT_SERVER             "miranda.iot.catslair"
#define USE_SERIAL
#define HA_MQTT_BASE            "display/aquarium"
#define ONE_WIRE_PIN            D3

#include <ArduinoOTA.h>

WiFiClient espClient;
PubSubClient client(espClient);

#include "sensory_bme.h"
#include "sensory_dallas.h"

Sensory_BME      SensorBME;
Sensory_Dallas   SensorDallas(ONE_WIRE_PIN);

String myHostname;

void sensorCallback(enum Sensory::sensorType sType, char * _sensor, char * _value) {
  char topic[100];
  char sensor[100];
  char value[100];
  Sensory::sensorType st;
  
  sprintf(sensor, "%s", _sensor);
  sprintf(value, "%s", _value);
  st = sType;
    
  if(st == Sensory::sensorType::STATE) {
    sprintf(topic, "%s/sensor/%s/state", HA_MQTT_BASE, sensor);
  }
  if(st == Sensory::sensorType::EVENT) {
    sprintf(topic, "%s/event", HA_MQTT_BASE);
  }
  Serial.printf("%s=%s\n", topic, value);
  client.publish(topic, value);                     
}


void setup() {
  Serial.begin(115200);  
  pin_init();

  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);

  Wire.begin();
  SensorBME.setup(sensorCallback).setInterval(10000);
  SensorDallas.setup(sensorCallback).setPollInterval(5).setPublishInterval(30); 

  ArduinoOTA.begin();
}


void loop() {

  if (!client.connected())reconnect() ;
  client.loop();

  ArduinoOTA.handle();
  SensorDallas.loop();
  SensorBME.loop();    
};



void setup_wifi() {
  delay(100);
  randomSeed(micros());

  Serial.printf("\nConnecting to %s\n", WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.softAPdisconnect(true);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);
  delay(100);
  
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }

    
  Serial.println("\nConnected!\n");
  Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
  Serial.printf("IP address : %s/%s\n", WiFi.localIP().toString().c_str(), WiFi.subnetMask().toString().c_str());
  Serial.printf("Hostname   : %s\n", WiFi.hostname().c_str());

  myHostname = WiFi.hostname();
  //WiFi.softAPdisconnect(true);
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "DevArd-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
};

void pin_init() {
    
  pinMode(0, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  //pinMode(2, OUTPUT);
  digitalWrite(0, HIGH); // Turn the LED off by making the voltage HIGH
  //digitalWrite(2, HIGH); // Turn the LED off by making the voltage HIGH
};



