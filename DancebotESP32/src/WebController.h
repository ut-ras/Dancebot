/**
 * Author: Matthew Yu
 * Last Modified: 05/22/20
 * Project: Dancebot Swarm
 * File: WebController.h
 * Description: A generic secure web controller for controlling various Demobots Projects. Robots should be able to: connect to a remote server and receive/send commands, as well as spin up their own server and serve a webpage to the user to directly interact with it.
 * Organization: UT IEEE RAS
 */
#pragma once

// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
#include "RobotConfig.h"
#include "DemobotNetwork.h"

#define NO_SERVER 404
#define NO_RESPONSE -1
#define OK 200

/* ---------------------SETUP--------------------------------------- */

/**
 * sets up a network and a server (if either are necessary) on the robot.
 * The end result should be either a connected robot or a failure in which the
 * robot can try again at another point.
 */
bool setupNetworking();
/**
 * sets up a network for the robot and other robots to connect to.
 * Fails in the event of an error.
 * 
 * @return bool
 *      true on success, false on some failure.
 */
bool setupAPNetwork();
/**
 * sets up a server for the robot and other robots to connect to.
 * Fails in the event of an error.
 * 
 * @return bool
 *      true on success, false on some failure.
 */
bool setupServer();
/* ---------------------CLIENT REQUESTS----------------------------- */
/**
 * joinServer sends a GET request with the robot id to IP/robotJoin to add
 * itself to the server. Looks for a 200 HTTP code response to signal acceptance.
 * 
 * @return string
 *      server response.
 */
String joinServer();
/**
 * getState sends a GET request with the robot id to IP/getState to retrieve the
 * new operating state. Looks for a 200 HTTP code response to signal success.
 */
String getState();
/* ---------------------SERVER HANDLER REQUESTS--------------------- */
void startServer();
// void handle_getState(AsyncWebServerRequest *request);
// void handle_joinServer(AsyncWebServerRequest *request);
// void handle_state(AsyncWebServerRequest *request, int state);
// void handle_notFound(AsyncWebServerRequest *request);
/* ---------------------WEB PAGE------------------------------------ */
// String sendHTML();
// String sendJavascript();

/* ---------------------HELPER FUNCTIONS---------------------------- */

/**
 * General WIFI access
 * https://www.arduino.cc/en/Tutorial/ConnectWithWPA
 * https://www.megunolink.com/articles/wireless/how-do-i-connect-to-a-wireless-network-with-the-esp32/
 * Secure WIFI over SSL with ESP32
 * https://www.youtube.com/watch?v=Wm1xKj4bKsY
 * https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientSecure/WiFiClientSecure.ino
 * Get SSID of current network
 * https://www.arduino.cc/en/Reference/WiFiSSID
 * Scanning networks
 * https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiScan/WiFiScan.ino
 * Static/Fixed IP
 * https://randomnerdtutorials.com/esp32-static-fixed-ip-address-arduino-ide/
 * HTTP GET/POST with esp32
 * https://randomnerdtutorials.com/esp32-http-get-post-arduino/#http-get-1
 * ESP32 GET requests
 * https://techtutorialsx.com/2017/05/19/esp32-http-get-requests/
 * Simple ESP32 web server
 * https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/
 * https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
 * fetch promises
 * https://javascript.info/fetch
 */