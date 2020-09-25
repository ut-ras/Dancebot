/**
 * Author: Matthew Yu
 * Last Modified: 05/22/20
 * Project: Dancebot
 * File: DancebotESP32.ino
 * Description: Dance Dance Dance! https://youtu.be/ciEHtGpSviE
 * Organization: UT IEEE RAS
 */
#include <Arduino.h>
#include "DemobotAPI.h"     // grab abstracted robot API - we fill in this for each robot
#include "WebController.h"  
#include "DemobotNetwork.h"

/** 
 * Example flow for a Demobots sketch with an ESP32:
 * setup() {
 *   initializeRobot()
 *   setupWifi()
 * }
 * 
 * loop() {
 *   update robot state
 *   look for update from server
 * }
 */

void setup() {
    delay(5000); //give time for me to open up serial monitor
    setupNetworking();
    initializeRobot();
    while(!joinServer().equals("[JOIN]"));
}

void loop() {
    static int lastMsgTime = 12000; // start 12 seconds into bootup

    move();

    // send GetState request to server and update the robot state
    int time = millis();
    if(!isOscillating() || ((time - lastMsgTime) > 250)) { // 250 is UPDATE_WAIT
        // in updateState we'll update state and send a leaveServer() request
        // and shut down if necessary.
        updateState(getState());
        lastMsgTime = time;
    }
}
