# How to Connect to the Web App

## Intializing Server
The code to initialize and manage the web server is in "WebController.cpp". The code to start the web server will be run during the "setup" function in "DemobotLegsESP32.ino." 

Specifically the following code will run:

```
Serial.println("Setting up WiFi...");
setupWiFi(WIFI_MODE, ssid, pass);       //Access Point or Station
setupWebServer(bot);                    //Set up the Web Server
Serial.println("Finished setting up WiFi!");
```


Once "Finished setting up WiFi!" is printed, you should be able to connect to the Wifi network and access the web app. 

## Connecting to Wifi Network

The ESP32 launches its own WiFi network you need to connect to access the web app. 

As of right now, the SSID and password the network are as follows:

SSID: Cole  
Password: cole1234

If this documentation becomes out of date, you can find the current SSID/password in "DemobotLegsESP32.ino" file

## Accessing the Web App

To access the web app, visit "http://192.168.4.1/" while on the ESP32's WiFi network.  
You should see something like this:

![Dancing Bot Webpage](../img/dancebot_webpage.png)


