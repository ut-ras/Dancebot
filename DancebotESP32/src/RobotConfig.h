/**
 * File: RobotConfig.h
 * Author: Matthew Yu
 * Last Modified: 09/26/20
 * Project: Demobots General
 * Organization: UT IEEE RAS
 * Description: A configuration file to be used by WebController.h and
 * DemobotAPI.h to define the Demobot internal representation.
 */
#pragma once

/** Dancebot Definitions. */
enum DancebotStates {
    Reset,
    Walk,
    Hop,
    Wiggle,
    Ankles,
    Demo1,
    Demo2,
    NumStates,
};

enum DancebotExpression {
    None,
    Happy,
    Sad,
    Neutral,
};

enum DancebotEyeColor {
    Off,
    Red,
    Green,
    Blue,
};

typedef struct Dancebot {
    char* robotType;
    int robotID;
    double soc; // state of charge
    DancebotStates robotState;
    DancebotExpression robotExpression;
    DancebotEyeColor robotEyeColor;
} Dancebot;

/**
 * Converts the DancebotStates enum instance into a string value.
 * 
 * @param[in] state DancebotStates enum indicating input state.
 * @return a pointer to a string that indicates the state.
 */
const char* state2string(DancebotStates state);

/**
 * Converts the DancebotExpression enum instance into a string value.
 * 
 * @param[in] expression DancebotExpression enum indicating input state.
 * @return a pointer to a string that indicates the state.
 */
const char* expression2String(DancebotExpression expression);

/**
 * Converts the DancebotEyeColor enum instance into a string value.
 * 
 * @param[in] eyeColor DancebotEyeColor enum indicating input state.
 * @return A pointer to a string that indicates the state.
 */
const char* eyeColor2String(DancebotEyeColor eyeColor);