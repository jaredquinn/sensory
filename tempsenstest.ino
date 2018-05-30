
#include "Arduino.h"   // replace this by "WProgram.h" when your IDE is older then 1.0
#define DEBUG true // flag to turn on/off debugging

#define Serial if(DEBUG)Serial

#include <WiFiIOTCredentials.h>

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>

#define MQTT_SERVER             "miranda.iot.catslair"
#define USE_SERIAL
#define HA_MQTT_BASE            "display/test"
#define ONE_WIRE_PIN            D3

#include <ArduinoOTA.h>

WiFiClient espClient;
PubSubClient client(espClient);

#include "sensory_dallas.h"
#include "sensory_bme.h"
#include "sensory_sht.h"

Sensory_SHT      SensorSHT;
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
  delay(500);
  scan();
  
  SensorSHT.setup(sensorCallback).setPollInterval(5);
  SensorBME.setup(sensorCallback).setPollInterval(5);
  SensorDallas.setup(sensorCallback).setPollInterval(5); 

  ArduinoOTA.begin();
}


void loop() {

  if (!client.connected())reconnect() ;
  client.loop();

  ArduinoOTA.handle();
  SensorDallas.loop();
  SensorBME.loop();    
  SensorSHT.loop();
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

  Serial.printf("SDA is %d and SCL is %d\n", SDA, SCL);
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


void scan() {
  
{
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

}

