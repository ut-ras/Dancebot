/**
 * Author: Matthew Yu
 * Last Modified: 03/18/21
 * Project: Dancebot
 * File: Dancebot.h
 * Description: Implements definitions for the Dancebot class, which builds an
 * internal Dancebot state that can move physical hardware.
 * Organization: UT IEEE RAS
 */
#pragma once

#include <Arduino.h>

class Dancebot {
    /**
     * The Dancebot class manages Dancebot firmware code. It holds an internal
     * state representation of the Dancebot, and can control features such as
     * the LED eyes, power controller, and servos.
     */
    public:
        /** List of all possible Dancebot names. */
        enum DancebotID {
            CARMEN,
            BUNNIE,
            DOTTY,
            MIRA,
            SNAKE
        };

        /** Enum describing possible Dancebot movement states. */
        enum DancebotMovements {
            Reset,
            Walk,
            Hop,
            Wiggle,
            Ankles,
            Demo1,
            Demo2,
            NUM_STATES
        };

        /** Enum describing possible Dancebot expressions. */
        enum DancebotExpressions {
            None,
            Happy,
            Sad,
            Neutral,
            NUM_EXPRESSIONS
        };

        /** Enum describing possible Dancebot eye colors. */
        enum DancebotEyeColors {
            Off,
            White,
            Red,
            Pink,
            Yellow,
            Green,
            Blue,
            NUM_COLORS
        };

        /**
         * Creates a new Dancebot object and fills in the relevant default
         * parameters, if any.
         *
         * @param[in] ID Enum referencing the specific Dancebot.
         */
        explicit Dancebot(const DancebotID ID);

        /** Getters. */
        String getID();
        String getCharge();
        int    getChargeInt();
        String getMovement();
        String getEyeExpression();
        String getEyeColor();

        /** Setters. Does not set upon invalid enum. */
        void setMovement(DancebotMovements movement);
        void setEyeExpression(DancebotExpressions eyeExpression);
        void setEyeColor(DancebotEyeColors eyeColor);

    private:
        /** Pin used for shutting down the robot. */
        int shutdownPin;

        /** Pin used for sampling state of charge. */
        int socPin;

        /**
         * Returns the state of charge sampled.
         *
         * @param[in] pin Pin to sample.
         * @return Value of pin, from 0-100 scaled.
         */
        int getSOC(int pin);

        /**
         * Shuts down the robot.
         *
         * @param[in] pin Pin to trigger shutdown with.
         */
        void shutdown(int pin);

        /** Internal configuration specifying the Dancebot state. */
        struct DancebotConfig {
            /** ID of the Dancebot. */
            DancebotID ID;

            /** State of charge of the Dancebot. */
            int soc;

            /** Current movement state. */
            DancebotMovements move;

            /** Current expression. */
            DancebotExpressions expression;

            /** Current eye color. */
            DancebotEyeColors eyeColor;
        };
        struct DancebotConfig dancebotConfig;
};
