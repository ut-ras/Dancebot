/**
 * File: RobotConfig.h
 * Author: Matthew Yu
 * Last Modified: 09/21/20
 * Project: Demobots General
 * Organization: UT IEEE RAS
 * Description: A configuration file to be used by WebController.h and
 * DemobotAPI.h to define the Demobot internal representation.
 * NOTE: Modify the MODIFIABLE INFO section for your robot.
 */
#pragma once
// --------------------------- MODIFIABLE INFO ---------------

// Dancebot Definition
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

// Individual Dancebot Definition
char name[9] = "Dancebot";
Dancebot dancebot1 {
    name,
    0,
    100.0,
    Reset,
    None,
    Off,
};


const char* state2string(DancebotStates state) {
    switch(state) {
        case Walk:
            return "Walk";
        case Hop:
            return "Hop";
        case Wiggle:
            return "Wiggle";
        case Ankles:
            return "Ankles";
        case Demo1:
            return "Demo1";
        case Demo2:
            return "Demo2";
        case Reset:
        default:
            return "Reset";
    }
}

const char* expression2String(DancebotExpression expression) {
    switch(expression) {
        case Happy:
            return "Happy";
        case Sad:
            return "Sad";
        case Neutral:
            return "Neutral";
        case None:
        default:
            return "None";
    }
}

const char* eyeColor2String(DancebotEyeColor eyeColor) {
    switch(eyeColor) {
        case Red:
            return "Red";
        case Green:
            return "Green";
        case Blue:
            return "Blue";
        case Off:
        default:
            return "Off";
    }
}
