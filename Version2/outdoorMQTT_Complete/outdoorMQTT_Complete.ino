#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include "secret.h"

#include "DHT.h"
#include <Wire.h>
#define DHToutdoor 7 
#define DHTTYPE DHT22 
DHT dhtout(DHToutdoor, DHTTYPE);

#define trigPin 8
#define echoPin 9

#define weatherPin A3

int port = 1884;
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqtt_server = SECRET_MQTTURL;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;

const char topic1[]  = SECRET_MQTTTOPIC1;
const char topic2[]  = SECRET_MQTTTOPIC2;
const char topic3[]  = SECRET_MQTTTOPIC3;
const char topic4[]  = SECRET_MQTTTOPIC4;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

//set interval for sending messages (milliseconds)
const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  //connect to wifi
  connectWiFi();

  //connect to MQTT
  connectMQTT();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  dhtout.begin();
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  unsigned long currentMillis = millis();

  //The message will only be published every 1 sec for demonstration
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    double hout = round((dhtout.readHumidity())*10)/10;
    // Read temperature as Celsius
    double tout = round((dhtout.readTemperature())*10)/10;
    
    Serial.print(F("Outdoor Humidity: "));
    Serial.print(hout);
    Serial.print(F("%  Outdoor Temperature: "));
    Serial.print(tout);
    Serial.println(F("°C "));

    
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
  
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    //Get the distance between the rangefinder and the object on top of it
    long duration = pulseIn(echoPin, HIGH);
    float distance = 34400 *duration/2000000;
    Serial.print("Distance in cm: ");
    Serial.println(distance);

    //Get the brightness of the surrounding from the phtotransistor
    int WeatherValue = analogRead(weatherPin);
    Serial.print("Weather: ");
    Serial.println(WeatherValue);    
    
    //pubish sensor data to MQTT
    mqttClient.beginMessage(topic1);
    Serial.print("Publish distance: ");
    Serial.println(distance);   
    mqttClient.print(distance);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic2);
    Serial.print("Publish weather: ");
    Serial.println(WeatherValue);   
    mqttClient.print(WeatherValue);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic3);
    Serial.print("Publish outdoor temperature: ");
    Serial.println(tout);  
    mqttClient.print(tout);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic4);
    Serial.print("Publish outdoor humidity: ");
    Serial.println(hout);
    mqttClient.print(hout);
    mqttClient.endMessage();
  }
}

//Connect to WiFi
void connectWiFi(){
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);

  //if it is not connected try again
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(100);
  }

  Serial.println("You're connected to the network");
  Serial.println();
}

//Connect to MQTT
void connectMQTT(){
    Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(mqtt_server);
    // Create a random client ID
  String clientId = "UnoWiFIiClient-";
  clientId += String(random(0xffff), HEX);
  //Set the client ID, MQTT username and password
  mqttClient.setId(clientId);
  mqttClient.setUsernamePassword(mqttuser, mqttpass);

  //if it is not connected, try again
  if (!mqttClient.connect(mqtt_server, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}
