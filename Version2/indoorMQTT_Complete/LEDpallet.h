#include <Adafruit_NeoPixel.h>
float LEDpallet[] = {
pixels.Color(0, 64, 8), //AQI 1
pixels.Color(20, 54, 0), //AQI 2
pixels.Color(63, 42, 0), //AQI 3
pixels.Color(61, 28, 0), //AQI 4
pixels.Color(64, 0, 0)}; //AQI and Alert 
int tempblue = pixels.Color(0, 63, 8); //Temperature -- cold
int tempyellow = pixels.Color(255, 204, 0); //Temperature -- warm
