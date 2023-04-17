/**
 * Author: Matthew Yu
 * Last Modified: 09/26/20
 * Project: Dancebot Swarm
 * File: DancebotAPI.h
 * Description: Dancebot API to drive hardware with using a state structure
 * Organization: UT IEEE RAS
 */
#ifndef DEMOBOTAPI_H
#define DEMOBOTAPI_H

#include "DancingServos.h"
#include "RobotConfig.hpp"

/**
 * Sets up a new DancingServos instance and calibrates the servos.
 */
void initializeRobot();

/**
 * Performs one iteration of the robot movement.
 */
void move();

/**
 * Updates the internal function of the DancingServos* bot.
 * We expect the following types of messages:
 *      - "" (no or bad response)
 *      - "[JOIN]"  (robot has joined the network)
 *      - "[LEFT]"  (robot has left the network willingly and the server
 *          acquiesced. Shutdown the robot.)
 *      - "[UPDT] ROBOT_ID,MOVE,EYE_COLOR,EYE_EXPRESSION,SHUTDOWN"
 *          - Robot ID: check to see if we got the right data for the robot
 *          - Robot Movement: next state to change to
 *          - Eye Color: robot eye color to set to
 *          - Eye Expression: robot eye expression to set to
 *          - Shutdown: supercedes Robot Movement. Calls the shutdown routine 
 *              if applicable. 
 *
 * @param[in] state Encoded version of the server expectation of the Dancebot state.
 */
void updateState(const String state);

/**
 * checks whether the robot is currently moving.
 * @return True if it is oscillating, false otherwise.
 */
bool isOscillating();

/**
 * Performs all spindown routines that may or may not end in the robot cutting
 * power to itself.
 */
void shutdown();

#endif