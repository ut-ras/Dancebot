/**
 * Author: Matthew Yu
 * Last Modified: 05/06/20
 * Project: Dancebot Swarm
 * File: WebController.h
 * Description: A generic secure web controller for controlling various Demobots Projects. Robots should be able to: connect to a remote server and receive/send commands, as well as spin up their own server and serve a webpage to the user to directly interact with it.
 * Organization: UT IEEE RAS
 */

#ifndef WEBCONTROLLER
#define WEBCONTROLLER

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WebServer.h>

// #include "DemobotAPI.h"
#include "WiFiConfig.h"

#define NO_SERVER 501
#define OK 200
#define MAX_ROBOTS 20 // I seriously doubt that we'll ever have more than 5 robots on a single webserver (i.e. Dancebots)
#define MAX_ROBOT_ID_LENGTH 20
WebServer server(80);

/**
 * Robot modes:
 * STA  - host service on a found network
 * AP   - host service on my own network
 * CON  - connect to existing service on a found network 
 */
enum mode {STA, AP, CON};

void setupWifi();
void startServer();

/* MODIFY BELOW WITH YOUR RELEVANT ROBOT */
enum DancebotStates {
    Reset,
    Walk,
    Hop,
    Wiggle,
    Ankles,
    Demo1,
    Demo2
};
struct Robot {
    // TODO: consider hashing to an integer and checking that
    char robotID[MAX_ROBOT_ID_LENGTH];
    int robotState;
    int robotEyeColor;
    int robotExpression;
};

int numConnectedRobots = 0;
Robot connectedRobots[MAX_ROBOTS] = { {NULL, 0, 0, 0} };

void handle_OnConnect();
void handle_Reset();
void handle_Walk();
void handle_Hop();
void handle_Wiggle();
void handle_Ankles();
void handle_Demo1();
void handle_Demo2();
void handle_GetState();
void handle_RobotJoin();
void handle_NotFound();
// helper function
void handle_State(DancebotStates state);

/* MODIFY BELOW WITH YOUR RELEVANT ROBOT */
String sendHTML();
String sentJavascript();

#endif

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
 * 
 */


/** 
 * TODO: move to arduino sketch
 * Example flow for a Demobots sketch with an ESP32:
 * setup() {
 *   initializeRobot()
 *   setupWifi()
 * }
 * 
 * loop() {
 *   // update robot state
 *   // look for update from server
 * }
 */