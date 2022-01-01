# CASA0016HomeWeatherStation
## Introduction
This project aims to create a prototype of a home weather station that is able to show:
Indoor: 1. Humidity
        2. Temperature
        3. Air quality parameters
Outdoor: 1. Humidity
         2. Temperature

## How it works
This system can be divided into 2 subsystem: indoor and outdoor.
### LCD
There are 3 different screens that the LCD can show. THe screen can be changed to another one by pressing the buttons of the indoor subsystem.
#### Screen 1 -- Outdoor parameters:
![image](https://user-images.githubusercontent.com/91946874/147841701-d007bf71-d235-4636-a6c6-f08fbe96bd9b.png)
- This shows when no button is pressed
- It shows: - Outdoor temperature and humidity
            - Outdoor weather – Sunny/Cloudy
            - Phototransistor status
            - “Outdoor”
#### Screen 2 -- Indoor parameters:
![image](https://user-images.githubusercontent.com/91946874/147841704-d3594ae3-0f17-4a36-96d5-281388c14edd.png)
- This shows when left button is pressed
- It shows: - Indoor temperature and humidity
            - Indoor Air Quality Index
            - “Indoor”

#### Screen 3 -- Indoor Air Quality parameters:
![image](https://user-images.githubusercontent.com/91946874/147841708-b9dc4b89-1876-48aa-854c-6d9c4482c578.png)
- This shows when right button is pressed
- It shows: - Total Volatile Organic Compound (TVOC)
            - Equivalent Carbon Dioxide (eCO2)

#### Icon
By using icons, the screen is able to display more. A comparison can be seen here:
![image](https://user-images.githubusercontent.com/91946874/147841768-8949034f-a5f9-4e1e-9775-f8d2aa552509.png)
This simulated LCD screen is generated using a Arduino LCD simulator => https://wokwi.com/arduino/projects/307244658548802112  

![image](https://user-images.githubusercontent.com/91946874/147841717-3d65af6b-04a8-4cfd-a76d-9abab3a9440a.png)  
These are the icons that are used to display information on the LCD.
The icon meanings are:  
Water Drop: Humidity  
Thermometer: Temperature  
Sun: Sunny  
Cloud: Cloudy  
Tick: Phototransistor is not covered by anything  
Cross: Phototransistor is covered by something  

##### How to create the icons  
Each character in the 16 x 2 LCD screen provided in the Arduino Starter Kit has a size of 5 x 8.
![image](https://user-images.githubusercontent.com/91946874/147841827-4a357e8f-c0f6-4607-a416-c75e59818aca.png)
1. Create a 5 x 8 table (Or 10 x 8 if the icon is long, like the sun and cloud icon)
2. Adjust the height and width of the table so that the spaces looks like squares
3. Fill the spaces with black background to create the icon
4. Go to https://maxpromer.github.io/LCD-Character-Creator/
5. Click on the squares on the left to make the icon
6. The code for the icon is automatically generated on the right

### LED stick
![image](https://user-images.githubusercontent.com/91946874/147841921-3c7cd92c-c3ae-49a3-ac93-f6ade0275190.png)  
The first 5 leds in the LED stick are used to show the indoor air quality index. the IAQ rating by [Renesa](https://www.renesas.com/us/en/application/home-building/indoor-air-quality-sensor-iaq), is used to decide which leds should light up. The 7th led is used to represent the outdoor temperature. When temperature is over 5 Celsius it will light up as a yellow led otherwise, it will be a blue led. But for demonstration sake I changed the threshold into 27 Celsius. The 8th led will flash in red when something is covering the phototransistor. Although the LED strip looks bright, the RGB values used for it actually looks like this. Full brightness of the LED is too bright.

## Version 1
### Schematic 
![image](https://user-images.githubusercontent.com/91946874/147842020-212501d1-9583-48f8-bdbf-66e7f2259e3b.png)  
### Breadboard view
![image](https://user-images.githubusercontent.com/91946874/147842025-cd132e1a-e09b-4966-8818-9d16ad94f376.png)
### Physical device
![image](https://user-images.githubusercontent.com/91946874/147842030-3baea129-19ad-4457-8032-db23e98474d7.png)
#### Indoor box
![image](https://user-images.githubusercontent.com/91946874/147842035-fa1bad95-a809-4af5-a51d-b800ef213282.png)
#### Outdoor box
![image](https://user-images.githubusercontent.com/91946874/147842039-b19ece48-5b15-42a8-aa7c-d0b238640ab0.png)

## Version 2
### Schematic 
#### Indoor box
![image](https://user-images.githubusercontent.com/91946874/147842055-e75e3477-21b5-47bc-b7da-1035eb86deca.png) 
#### Outdoor box  
![image](https://user-images.githubusercontent.com/91946874/147842060-167620d5-c1e9-4a42-906b-2944320ad41c.png)
### Breadboard view
![image](https://user-images.githubusercontent.com/91946874/147842069-c097e696-3429-41ef-8849-05a30617dd11.png)
### Physical device
![image](https://user-images.githubusercontent.com/91946874/147842072-d014e667-5827-4057-ac41-06910839d768.png)
#### Indoor box
![20211213_170306](https://user-images.githubusercontent.com/91946874/147842080-5ca5029f-4705-40b1-8590-9f2be7462cb2.jpg)
#### Outdoor box
![20211213_170316](https://user-images.githubusercontent.com/91946874/147842081-a389e38d-7f22-42eb-9424-ed565518d51d.jpg)  

## V1 vs V2  
### V1:  
- Send sensor data from the outdoor box through long wires to the indoor box  
- Uses 1 Arduino UNO  
- Only one code file is needed  
- -Simpler   
### V2:  
- Send sensor data from the outdoor box to the MQTT broker. The indoor box receive messages from the broker  
- Uses 2 Arduino UNO Wifi Rev 3  
- Needs multiple code files  
- More complex  
