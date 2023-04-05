/**
 * Author: Matthew Yu
 * Last Modified: 09/26/20
 * Project: Dancebot
 * File: DancebotESP32.ino
 * Description: Dance Dance Dance! https://youtu.be/ciEHtGpSviE
 * Organization: UT IEEE RAS
 */
#include <Arduino.h>
#include "DemobotAPI.h"
#include "WebController.h"  
#include "DemobotNetwork.h"

#define SETUP_TIMEOUT 60000
#define UPDATE_TIMEOUT 250
/** 
 * Example flow for a Demobots sketch with an ESP32:
 * setup() {
 *   setupNetworking()
 *   initializeRobot()
 * }
 * 
 * loop() {
 *   do some movement
 *   look for update from server and update state
 * }
 */

/** Dancebot instantiation */
char dancebotName[9] = "Dancebot";
Dancebot dancebot {
    dancebotName,
    0,
    100.0,
    Reset,
    None,
    Off
};

/** Network instantiation */
DemobotNetwork network(String(dancebot.robotType) + String(dancebot.robotID));


void setup() {
    /* Give some time to open up the serial monitor. */
    delay(5000); // 5 seconds
    setupNetworking(network);
    initializeRobot();

    /* Attempt to connect for at least half a minute. */
    bool connected = false;
    do {
        if (joinServer(network, dancebot).substring(0, 6).equals("[JOIN")) { 
            connected = true; 
        }
    } while (millis() < SETUP_TIMEOUT && !connected);

    /* If not connected, initiate shutdown routine. */
    if (connected) { shutdown(); }
}

void loop() {
    /* Internal counter for determining when to send a new message. */
    static int lastMsgTime = 0;

    /* Move the robot. */
    move();

    /* Send GetState request to server and update the robot state. */
    int time = millis();
    /**
     * Note that we don't care about time overflowing here since the arduino
     * docs for millis() says that it'll overflow 50 days, which at that point
     * the robot, if never turned off, will definitely have died by then.
     */
    if(!isOscillating() || ((time - lastMsgTime) > UPDATE_TIMEOUT)) {
        /**
         * In updateState we'll update state and send a leaveServer() request
         * and shut down if necessary.
         */
        updateState(getState(network, dancebot));
        lastMsgTime = time;
    }
}