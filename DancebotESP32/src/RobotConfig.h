/**
 * Author: Matthew Yu
 * Last Modified: 05/22/20
 * Project: Demobots General
 * File: RobotConfig.h
 * Description: A config file to be used by WebController.h and DemobotAPI.h.
 * Organization: UT IEEE RAS
 * NOTE: Modify the MODIFIABLE INFO section for your robot.
 */
#ifndef ROBOTCONFIG_H
#define ROBOTCONFIG_H

#define MAX_ROBOTS 20       // I seriously doubt that we'll ever have more than 5 robots on a single webserver (i.e. Dancebots)
#define CRED_LOG_LEN 5      // arbitrarily decided max credentials defined at any time
#define RETRY_WAIT 200      // 200 ms
#define UPDATE_WAIT 150     // 150 ms
#include <IPAddress.h>

extern const int ROBOT_ID;

extern IPAddress gateway;
extern IPAddress subnet;
/**
 * Represents a single possible network that can be connected to or established.
 * Has a SSID and WPA2/PSK password associated with it.
 */
typedef struct Credential {
    char* SSID;
    char* PASSWORD;
} Credential;
extern Credential CredentialsLog[];
extern const int DEFAULT_NETWORK_ID;
/**
 * Represents a single Demobot type and has an ID and IPAddress it belongs to.
 */
typedef struct DemobotNetwork {
    int ID;
    String name;
    IPAddress defaultIP;
} DemobotNetwork;
extern DemobotNetwork Robots[];

// --------------------------- MODIFIABLE INFO ---------------
// FOR DANCEBOT
enum DancebotStates {
    Reset,
    Walk,
    Hop,
    Wiggle,
    Ankles,
    Demo1,
    Demo2,
    NumStates
};

extern char* dancebotStates[];
extern int numConnectedRobots;

typedef struct Dancebot {
    int robotID;
    int robotState;
    int robotEyeColor;
    int robotExpression;
} Dancebot;
extern Dancebot connectedRobots[MAX_ROBOTS];

#endif
