#include <Adafruit_NeoPixel.h>
float LEDpallet[] = {
//brightest
//pixels.Color(0, 255, 31),
//pixels.Color(159, 214, 0),
//pixels.Color(213, 167, 0),
//pixels.Color(245, 110, 0),
//pixels.Color(255, 0, 0)};

//half brightness
pixels.Color(0, 127, 15), //AQI
pixels.Color(79, 107, 0), //AQI
pixels.Color(106, 83, 0), //AQI
pixels.Color(122, 55, 0), //AQI
pixels.Color(127, 0, 0)}; //AQI and Alert 

int tempblue = pixels.Color(0, 126, 127); //Temperature -- cold
int tempyellow = pixels.Color(255, 204, 0); //Temperature -- warm
    
