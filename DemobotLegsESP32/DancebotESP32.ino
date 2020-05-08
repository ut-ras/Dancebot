/**
 * Author: Matthew Yu
 * Last Modified: 05/07/20
 * Project: Dancebot
 * File: DancebotESP32.ino
 * Description: Dance Dance Dance! https://youtu.be/ciEHtGpSviE
 * Organization: UT IEEE RAS
 */
#include <Arduino.h>
#include "DancingServos.h"
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
    // initializeRobot();
}

void loop() {
    while(1) {
        delay(1000);
    }
}