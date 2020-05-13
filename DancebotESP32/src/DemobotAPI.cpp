/**
 * Author: Matthew Yu
 * Last Modified: 05/06/20
 * Project: Dancebot Swarm
 * File: DancebotAPI.cpp
 * Description: Demobot API to drive hardware
 * Organization: UT IEEE RAS
 * NOTE: This is a template file. This should be customized for each robot.
 */
#include "DemobotAPI.h"

DancingServos* bot;
/**
 * initializeRobot sets up a new DancingServos instance and calibrates the servos.
 */
void initializeRobot() {
    bot = new DancingServos(14, 13, 12, 15); 

    // manual calibration - based on how the servos are attached to the 3d printed parts
    // [hipL, hipR, ankleL, ankleR]
    bot->setTrims(70, 150, 25, 18);

    bot->position0();
}

/**
 * move performs one iteration of the robot movement.
 */
void move() {
    // loop the motors
    bot->loopOscillation();
    // check if ready to start the next move in the dance
    bot->loopDanceRoutines();
}

/**
 * updateState updates the internal function of the DancingServos* bot.
 * @param String state - encoded version of the server expectation of the Dancebot state.
 * TODO: need to test
 */
void updateState(String state) {
    Serial.println("Server received state: " + state);
    if(state.indexOf(String(Reset))) {
        bot->position0();
        bot->enableDanceRoutine(false);

    }else if(state.indexOf(String(Walk))) {
        bot->walk(-1, 1500, false);

    }else if(state.indexOf(String(Hop))) {
        bot->hop(25, -1);

    }else if(state.indexOf(String(Wiggle))) {
        bot->wiggle(30, -1);

    }else if(state.indexOf(String(Ankles))) {
        bot->themAnkles(-1);
        
    }else if(state.indexOf(String(Demo1))) {
        bot->setDanceRoutine(0);
        bot->enableDanceRoutine(true);

    }else if(state.indexOf(String(Demo2))) {
        bot->setDanceRoutine(1);
        bot->enableDanceRoutine(true);

    }else {
        Serial.println("Invalid state.");
    }
}

