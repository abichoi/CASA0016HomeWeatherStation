#include <ArduinoMqttClient.h>
#include <SPI.h>
#include <WiFiNINA.h>

#include "DHT.h"
#include <Wire.h>
#include "Adafruit_SGP30.h"
#include <Adafruit_NeoPixel.h>

#define DIN 10
#define NUMPIXELS 8
Adafruit_NeoPixel pixels(NUMPIXELS,DIN);
#include "LEDpallet.h"

Adafruit_SGP30 sgp;

#define DHTTYPE DHT22 
#define DHTindoor 6
DHT dhtin(DHTindoor, DHTTYPE);

#include "arduino_secrets.h"
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = SECRET_MQTTURL;
int        port     = 1884;
const char topic1[]  = SECRET_MQTTTOPIC1;
const char topic2[]  = SECRET_MQTTTOPIC2;
const char topic3[]  = SECRET_MQTTTOPIC3;
const char topic4[]  = SECRET_MQTTTOPIC4;

float distance;
float WeatherValue;
float tout;
float hout;
char* value;


int AQI = 0;
int counter = 0;

float hin;
float tin;

#include <LiquidCrystal.h>
#include "LCD_custom_character.h"
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define buttonin_LCD A2
#define buttonAQI_LCD A1


#define buzzer 13

//get the absolute humidity to increase the accuracy of SGP30
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}


void setup() {
  Serial.begin(9600);

  //if sgp can't be initialised, the system won't start
    if (! sgp.begin()){
    Serial.println("Sensor not found :(");
    while (1);
  }

  dhtin.begin();
  pixels.begin();  

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  //create custom character
  lcd.createChar(0, water);
  lcd.createChar(1, temp);
  lcd.createChar(2, sun1);
  lcd.createChar(3, sun2);
  lcd.createChar(4, cloud1);
  lcd.createChar(5, cloud2);
  lcd.createChar(6, tick);
  lcd.createChar(7, cross);
  //connect to wifi
  connectWiFi();
  //connect to MQTT
  connectMQTT();
}

void loop() {
  // MQTT keep alives which avoids being disconnected by the broker
  mqttClient.poll();
  getDHT();
  getSGP();
  bool buttoninPressed = digitalRead(buttonin_LCD);
  bool buttonAQIPressed = digitalRead(buttonAQI_LCD);

  pixels.clear();
  for (int i=0; i<AQI; i++){
    pixels.setPixelColor(i,LEDpallet[i]);
  }
  
  // show if it is cold or warm, the threshold is set to 27C for demonstration
  if (tout<27){
    pixels.setPixelColor(6,tempblue);
  } else{
    pixels.setPixelColor(6,tempyellow);
  }

  //if distance between the object and the rangefinder is less than 50
  //the last LED of  the LED stick flashes in Red
  if(distance<50){
    pixels.setPixelColor(7,LEDpallet[4]);
  }
  pixels.show();

   lcd.clear();

    Serial.print("distance:");
    Serial.println(distance); 
    Serial.print("Weatherout:");
    Serial.println(WeatherValue);
    Serial.print("Tempout:");
    Serial.println(tout);
    Serial.print("Humidityout:");
    Serial.println(hout);

  //if the right button is pressed, show TVOC and eCO2 values
  if(buttonAQIPressed){
    lcd.setCursor(0,0);
    lcd.print("TVOC:");
    lcd.print(sgp.TVOC);
    lcd.setCursor(11,0);
    lcd.print("ppb");
    lcd.setCursor(0,1);
    lcd.print("eCO2:");
    lcd.print(sgp.eCO2);
    lcd.setCursor(11,1);
    lcd.print("ppm");
   }
   //if the left button is pressed, show indoor parameters
   //humidity, temperature and indoor air quality index (AQI)
   else if (buttoninPressed){
  lcd.setCursor(0,0);
  lcd.write((byte) 0x00);
  lcd.print(hin,1);
  lcd.print("%");
  lcd.setCursor(10,0);
  lcd.write(1);
  lcd.print(tin,1);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("AQI:");
  lcd.print(AQI);
  lcd.setCursor(10,1);
  lcd.print("Indoor");
   }
  //if no button is pressed, show outdoor parameters
 //humidity, temperature, weather (sunny or cloudy), outdoor AQI and the state of the phototransistor
   else{
  lcd.setCursor(0,0);
  lcd.write((byte) 0x00);
  lcd.print(hout);
  lcd.setCursor(5,0);
  lcd.print("%");
  lcd.setCursor(7,0);
  lcd.write(1);
  lcd.print(tout);
  lcd.setCursor(12,0);
  lcd.print("C");
  lcd.setCursor(14,0);
    if (WeatherValue<500){
    lcd.write(4);
    lcd.write(5);
  }else{
    lcd.write(2);
    lcd.write(3);
  }
  lcd.setCursor(0,1);
  lcd.print("Sensor");
  //if there is something covering the phototransistor
  //lcd will show "X"
  //otherwise a tick
    if(distance<50){
      lcd.write(7);
    }else{
      lcd.write(6);
    }
  lcd.setCursor(9,1);
  lcd.print("Outdoor");
  }

    //if there is something covering the phototransistor
    //buzzer will beep
    if(distance<50){
      tone(buzzer, 1000);
    }else{
      noTone(buzzer);
    }

  //Flash the alert LED and make the buzzer stop beeping for 0.15 sec
  delay(150);
  pixels.setPixelColor(7,0,0,0);
  noTone(buzzer);
  pixels.show();
  delay(150);
}

//when there is message received
void onMqttMessage(int messageSize) {
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
  //create a char to store the message
  char combine[messageSize+1];
  //store the topic
  String topic = mqttClient.messageTopic();
  //read the message character by character and combine them 
    for (int i=0; i < messageSize; i++){
      combine[i]=(char)mqttClient.read();
    }
    //terminate the message variable
    combine[messageSize] = NULL;
   Serial.println(combine);
   Serial.println(topic);

  //store the message to its corresponding topic
  if (topic1 == topic){
    distance = atof(combine);
    Serial.print("distance:");
    Serial.println(distance); 
  } else if (topic2 == topic){
    WeatherValue = atof(combine);
    Serial.print("Weatherout:");
    Serial.println(WeatherValue);
  } else if (topic3 == topic){
    tout = atof(combine);
    Serial.print("Tempout:");
    Serial.println(tout);
  } else if (topic4 == topic){
    hout = atof(combine);
    Serial.print("Humidityout:");
    Serial.println(hout);
  }

  
}

//get the indoor humidity and temperature from DHT22
void getDHT(){
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    hin = dhtin.readHumidity();
    // Read temperature as Celsius (the default)
    tin = dhtin.readTemperature();
    Serial.print(F("Indoor Humidity: "));
    Serial.print(hin);
    Serial.print(F("%  Indoor Temperature: "));
    Serial.print(tin);
    Serial.println(F("°C "));
}

//get the indoor TVOC and eCO2 values and define the indoor Air Quality Index (AQI)
void getSGP(){
  sgp.setHumidity(getAbsoluteHumidity(tin, hin));
  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
  Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");
  if (sgp.TVOC <65){
    //Level 1 = Clean Hygienic Air
    AQI = 1;
  } else if (sgp.TVOC <220){
    //Level 2 = Good Air Quality
    AQI = 2;
  } else if (sgp.TVOC <660){
    //Level 3 = Noticeable Comfort Concerns
    AQI = 3;
  } else if (sgp.TVOC <2200){
    //Level 4 = Significant Comfort Issues
    AQI = 4;
  } else{
    //Level 5 - Unacceptable conditions
    AQI = 5;
  }
}

//connect to WiFi
void connectWiFi(){
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  //Try until it is connected
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(100);
  }

  Serial.println("You're connected to the network");
  Serial.println();
}
//connect to MQTT broker
void connectMQTT(){
  // Create a random client ID
  char* clientId = "UnoWiFIiClient-";
  clientId += char(random(0xffff), HEX);
  //Set the client ID, MQTT username and password
  mqttClient.setId(clientId);
  mqttClient.setUsernamePassword(mqttuser, mqttpass);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);
  //Try until it is connected
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);
 
  Serial.print("Subscribing to topic: ");
  Serial.println(topic1);
  Serial.print("Subscribing to topic: ");
  Serial.println(topic2);
  Serial.print("Subscribing to topic: ");
  Serial.println(topic3);
    Serial.print("Subscribing to topic: ");
  Serial.println(topic4);

  // subscribe to the topics
  mqttClient.subscribe(topic1);
  mqttClient.subscribe(topic2);
  mqttClient.subscribe(topic3);
  mqttClient.subscribe(topic4);

  Serial.print("Waiting for messages on topic: ");
  Serial.println(topic1);
    Serial.print("Waiting for messages on topic: ");
  Serial.println(topic2);
    Serial.print("Waiting for messages on topic: ");
  Serial.println(topic3);
    Serial.print("Waiting for messages on topic: ");
  Serial.println(topic4);
}
