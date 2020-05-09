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

