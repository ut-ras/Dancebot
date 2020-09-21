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

enum RobotExpression {
    Off,
    Happy,
    Sad,
    Neutral,
};

enum RobotEyeColor {
    White,
    Red,
    Green,
    Blue,
};

typedef struct Dancebot {
    char* robotType;
    int robotID;
    double soc; // state of charge
    DancebotStates robotState;
    RobotExpression robotExpression;
    RobotEyeColor robotEyeColor;
} Dancebot;

// Individual Dancebot Definition
char name[9] = "Dancebot";
Dancebot dancebot1 {
    name,
    0,
    100.0,
    Reset,
    Off,
    White,
};


