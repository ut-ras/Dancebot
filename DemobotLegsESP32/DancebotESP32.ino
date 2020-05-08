/**
 * Author: Matthew Yu
 * Last Modified: 05/07/20
 * Project: Dancebot
 * File: DancebotESP32.ino
 * Description: Dance Dance Dance! https://youtu.be/ciEHtGpSviE
 * Organization: UT IEEE RAS
 */
#include <Arduino.h>
#include "DemobotAPI.h"     // grab abstracted robot API - we fill in this for each robot
#include "WebController.h"  

/** 
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



void setup() {
    setupWifi();
    initializeRobot();
}

void loop() {
    move();
    // send GetState request to server and update the robot state
    // if hosting a server, manage the server requests
}