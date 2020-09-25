/**
 * Author: Matthew Yu
 * Last Modified: 05/22/20
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
 * @param state: String
 *      encoded version of the server expectation of the Dancebot state.
 *      We expect the following types of messages:
 *          - "" (no or bad response)
 *          - "[JOIN]"  (robot has joined the network)
 *      - "[LEFT]"  (robot has left the network willingly and the server
 *      acquiesced. Shutdown the robot.)
 *      - "[UPDT] ROBOT_ID,MOVE,EYE_COLOR,EYE_EXPRESSION,SHUTDOWN"
 *          - a [UPDT], update identifier, and the following CSV with no spaces:
 *              - Robot ID: check to see if we got the right data for the robot
 *              - Robot Movement: next state to change to
 *              - Eye Color: robot eye color to set to
 *              - Eye Expression: robot eye expression to set to
 *              - Shutdown: supercedes Robot Movement. Calls the shutdown routine 
 *                  if applicable. 
 */
void updateState(String state) {
    // parse identifier

    if (state.equals("")) {
        /* We received a bad response. Do nothing this iteration. */
        return;
    } else if (state.substring(0, 6).equals("[JOIN]")) {
        /**
         * Robot received an affirmative for a /robotJoin request.
         * TODO: do we need to do anything here?
         */
        return;
    } else if (state.substring(0, 6).equals("[LEFT]")) {
        /**
         * Robot received an affirmative for a /robotLeave request.
         * Call the shut down routine.
         */
        shutdown();
    } else if (state.substring(0, 6).equals("[UPDT]")) {
        /**
         * Robot received an affirmative for a /robotUpdate request.
         * Parse further for more instructions.
         */
        // TODO: parse move from index 7 to delimiter ','
        int move = 0;
        switch(move) {
            case Reset:
                bot->position0();
                bot->enableDanceRoutine(false);
                break;
            case Walk:
                bot->walk(-1, 1500, false);
                break;
            case Hop:
                bot->hop(25, -1);
                break;
            case Wiggle:
                bot->wiggle(30, -1);
                break;
            case Ankles:
                bot->themAnkles(-1);
                break;
            case Demo1:
                bot->setDanceRoutine(0);
                bot->enableDanceRoutine(true);
                break;
            case Demo2:
                bot->setDanceRoutine(1);
                bot->enableDanceRoutine(true);
                break;
            default:
                Serial.println("Invalid state.");
            // TODO: parse eye color to the next delimeter ','
            // TODO: parse eye expression to the next delimeter ','
            // TODO: parse shutdown to the end of the line.
            // TODO: if shutdown, send a /robotLeave request to the server.
        }

    } else {
        /** Some invalid format. Do nothing this iteration. */
        return;
    }
}

/**
 * isOscillating checks whether the robot is currently moving.
 * @return true if it is oscillating.
 */
bool isOscillating() {
    return bot->isOscillating();
}

void shutdown() {
    /* TODO: shutdown routine here calling the auto off driver. */
}