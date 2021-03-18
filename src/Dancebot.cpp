/**
 * Author: Matthew Yu
 * Last Modified: 03/18/21
 * Project: Dancebot
 * File: Dancebot.cpp
 * Description: Implements definitions for the Dancebot class, which builds an
 * internal Dancebot state that can move physical hardware.
 * Organization: UT IEEE RAS
 */
#include <Dancebot.h>


Dancebot::Dancebot(const DancebotID ID) {
    /* This pin is connected to a N-type MOSFET in the PCB.
     * Startup sets shutdown pin to high, allowing the MOSFET to close.
     * It shorts the circuit GND to the voltage regulator GND, allowing
     * the overall circuit to close and operate. */
    digitalWrite(shutdownPin, HIGH);

    socPin = 0; /* IO0. */
    shutdownPin = 1; /* IO1. */

    dancebotConfig.ID = ID;
    dancebotConfig.soc = getSOC(socPin);
    dancebotConfig.move = DancebotMovements::Reset;

    switch (ID) {
        case CARMEN:
            dancebotConfig.expression   = DancebotExpressions::Happy;
            dancebotConfig.eyeColor     = DancebotEyeColors::Blue;
            break;
        case BUNNIE:
            dancebotConfig.expression   = DancebotExpressions::Happy;
            dancebotConfig.eyeColor     = DancebotEyeColors::Yellow;
            break;
        case DOTTY:
            dancebotConfig.expression   = DancebotExpressions::Sad;
            dancebotConfig.eyeColor     = DancebotEyeColors::White;
            break;
        case MIRA:
            dancebotConfig.expression   = DancebotExpressions::Happy;
            dancebotConfig.eyeColor     = DancebotEyeColors::Red;
            break;
        case SNAKE:
            dancebotConfig.expression   = DancebotExpressions::Neutral;
            dancebotConfig.eyeColor     = DancebotEyeColors::Pink;
            break;
    }

    /* TODO: Turn on LED Eyes. */
}

String Dancebot::getID() {
    switch (dancebotConfig.ID) {
        case BUNNIE:    return String("Bunnie");
        case DOTTY:     return String("Dotty");
        case MIRA:      return String("Mira");
        case SNAKE:     return String("Snake");
        case CARMEN:
        default:        return String("Carmen");
    }
}

String Dancebot::getCharge() {
    return String(dancebotConfig.soc);
}

int Dancebot::getChargeInt() {
    return dancebotConfig.soc;
}

String Dancebot::getMovement() {
    switch (dancebotConfig.move) {
        case Walk:      return String("Walk");
        case Hop:       return String("Hop");
        case Wiggle:    return String("Wiggle");
        case Ankles:    return String("Ankles");
        case Demo1:     return String("Demo1");
        case Demo2:     return String("Demo2");
        case Reset:
        default:        return String("Reset");
    }
}

String Dancebot::getEyeExpression() {
    switch (dancebotConfig.expression) {
        case Happy:     return String("Happy");
        case Sad:       return String("Sad");
        case Neutral:   return String("Neutral");
        case None:
        default:        return String("None");
    }
}

String Dancebot::getEyeColor() {
    switch (dancebotConfig.eyeColor) {
        case White:     return String("White");
        case Red:       return String("Red");
        case Pink:      return String("Pink");
        case Yellow:    return String("Yellow");
        case Green:     return String("Green");
        case Blue:      return String("Blue");
        case Off:
        default:        return String("Off");
    }
}

void Dancebot::setMovement(DancebotMovements movement) {
    dancebotConfig.move = movement;
}

void Dancebot::setEyeExpression(DancebotExpressions eyeExpression) {
    dancebotConfig.expression = eyeExpression;
}

void Dancebot::setEyeColor(DancebotEyeColors eyeColor) {
    dancebotConfig.eyeColor = eyeColor;
}

/** Private methods. */
int Dancebot::getSOC(int pin) {
    int val = analogRead(pin);
    /* TODO: do some preprocessing to convert 0-1024 to 0-100 and to make sure
     * value reflects true SoC. */
    return val;
}

void Dancebot::shutdown(int pin) {
    /* By setting the pin to low, we open the MOSFET and thus the circuit,
     * halting power flow to the rest of the robot and turning it off until
     * startup (using the switch). */
    digitalWrite(pin, LOW);
}