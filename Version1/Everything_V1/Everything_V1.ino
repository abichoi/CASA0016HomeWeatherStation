#include "DHT.h"
#include <Wire.h>
#include "Adafruit_SGP30.h"
#include <Adafruit_NeoPixel.h>

Adafruit_SGP30 sgp;

#define DHTPIN 7 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

#include <LiquidCrystal.h>
#include "LCD_custom_character.h"
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define button_LCD 6

#define DIN 10
#define NUMPIXELS 8
Adafruit_NeoPixel pixels(NUMPIXELS,DIN);
#include "LEDpallet.h"

#define trigPin 8
#define echoPin 9

#define weatherPin A3

#define buzzer 13


uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}



void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // Wait for serial console to open!
  
  Serial.println("SGP30 and DHT22 test");
  dht.begin();
  if (! sgp.begin()){
    Serial.println("Sensor not found :(");
    while (1);
  }

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
//  lcd.print("hello, world!");
  lcd.createChar(0, water);
  lcd.createChar(1, temp);
//  lcd.createChar(2, celsius);
  lcd.createChar(2, sun1);
  lcd.createChar(3, sun2);
  lcd.createChar(4, cloud1);
  lcd.createChar(5, cloud2);
  lcd.createChar(6, tick);
  lcd.createChar(7, cross);
  
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);
  
  pixels.begin(); 

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(buzzer, OUTPUT);
}

int AQI = 0;
int counter = 0;

void loop() {
  bool buttonPressed = digitalRead(button_LCD);
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));

  // If you have a temperature / humidity sensor, you can set the absolute humidity to enable the humditiy compensation for the air quality signals
  sgp.setHumidity(getAbsoluteHumidity(t, h));

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  
  Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
  Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");

//  find out the indoor air quality index
  if (sgp.TVOC <65){
//    Level 1 = Clean Hygienic Air
    AQI = 1;
  } else if (sgp.TVOC <220){
    AQI = 2;
  } else if (sgp.TVOC <660){
    AQI = 3;
  } else if (sgp.TVOC <2200){
    AQI = 4;
  } else{
    AQI = 5;
  }
  pixels.clear();
    for (int i=0; i<AQI; i++){
    pixels.setPixelColor(i,LEDpallet[i]);
  }

// show if it is cold or warm
  if (t<5){
    pixels.setPixelColor(6,tempblue);
  } else{
    pixels.setPixelColor(6,tempyellow);
  }

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = 34400 *duration/2000000;
  Serial.print("Distance in cm: ");
  Serial.println(distance);

  if(distance<50){
    pixels.setPixelColor(7,LEDpallet[4]);
  }
  pixels.show();

  int WeatherValue = analogRead(weatherPin);
  Serial.print("Weather: ");
  Serial.println(WeatherValue);

  lcd.clear();
  if(buttonPressed){
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
   else{
  lcd.setCursor(0,0);
  lcd.write((byte) 0x00);
  lcd.print(h,1);
  lcd.print("%");
  lcd.setCursor(9,0);
  lcd.write(1);
  lcd.print(t,1);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("AQI:");
  lcd.print(AQI);
  lcd.setCursor(6,1);
    if (WeatherValue<500){
      lcd.write(4);
      lcd.write(5);
    }else{
      lcd.write(2);
      lcd.write(3);
    }
  lcd.setCursor(9,1);
  lcd.print("Sensor");
    if(distance<50){
      lcd.write(7);
      tone(buzzer, 1000);
    }else{
      lcd.write(6);
      noTone(buzzer);
    }
  }
  //Flash the alert LED and make the buzzer stop beeping for 0.25 sec
  delay(250);
  pixels.setPixelColor(7,0,0,0);
  noTone(buzzer);
  pixels.show();
  delay(250);
}
