/**
 * @file Dancebot.hpp
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Controls our Dancebot.
 * @version 0.1.0
 * @date 2023-04-20
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include "./netcode/src/DemobotConfig.hpp"
#include "./netcode/src/DemobotNetwork.hpp"
#include "./netcode/src/DemobotServer.hpp"
#include "./netcode/src/DemobotClient.hpp"

class Dancebot {
    public:
        enum DancebotMovement {
            RESET,
            WALK,
            HOP,
            WIGGLE,
            ANKLES,
            DEMO1,
            DEMO2,
            num_states,
        };

        enum DancebotExpression {
            NONE,
            SLEEPY,
            HAPPY,
            SAD,
            NEUTRAL,
            UT,
            num_expressions,
        };

        enum DancebotEyeColor {
            WHITE,
            RED,
            ORANGE,
            YELLOW,
            GREEN,
            BLUE,
            PURPLE,
            num_eye_colors
        };

        explicit Dancebot(const char *name);

        explicit Dancebot(
            const char *name,
            DancebotMovement move,
            DancebotExpression expression,
            DancebotEyeColor eye_color
        );

        ~Dancebot();

    private:
        void get_state();

        void send_state();

        void update_state();

        void move();

        void shutdown();

        bool setup_network(enum DemobotServer::StaticIPMode *mode);

        bool setup_client_server(enum DemobotServer::StaticIPMode mode);

        bool join_server(void);

        /**
         * Converts the DancebotMovement enum instance into a string value.
         *
         * @param[in] state DancebotMovement enum indicating input state.
         * @return a pointer to a string that indicates the state.
         */
        const char* move2string(DancebotMovement state);

        /**
        * Converts the DancebotExpression enum instance into a string value.
        *
        * @param[in] expression DancebotExpression enum indicating input state.
        * @return a pointer to a string that indicates the state.
        */
        const char* expression2string(DancebotExpression expression);

        /**
        * Converts the DancebotEyeColor enum instance into a string value.
        *
        * @param[in] eyeColor DancebotEyeColor enum indicating input state.
        * @return A pointer to a string that indicates the state.
        */
        const char* eyeColor2string(DancebotEyeColor eye_color);

    private:
        char *_name;

        enum DemobotID _id;

        enum DancebotMovement _movement;

        enum DancebotExpression _expression;

        enum DancebotEyeColor _eye_color;

        float _soc;

        DemobotNetwork *_network;

        DemobotServer *_server;

        DemobotClient *_client;
};
