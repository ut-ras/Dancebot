#include "RobotConfig.h"

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

char myDancebot[9] = "Dancebot";
Dancebot dancebot1 {
    myDancebot,
    0,
    100.0,
    Reset,
    None,
    Off,
};