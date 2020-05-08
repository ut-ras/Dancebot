/**
 * Author: Matthew Yu
 * Last Modified: 05/06/20
 * Project: Demobots General
 * File: RobotConfig.h
 * Description: A config file to be used by WebController.h and DemobotAPI.h.
 * Organization: UT IEEE RAS
 */
#ifndef WIFICONFIG
#define WIFICONFIG

#define MAX_ROBOTS 20       // I seriously doubt that we'll ever have more than 5 robots on a single webserver (i.e. Dancebots)
#define CRED_LOG_LEN 2
#define RETRY_WAIT 200      // 200 ms
#include <IPAddress.h>

// SET TO YOUR ROBOT NAME LISTED IN RobotIDs 
const int ROBOT_ID = Dancebot1;
// SET TO YOUR DEFAULT NETWORK ID INDEX LISTED IN CredentialsLog
const int DEFAULT_NETWORK_ID = 0;

// --------------------------- ROBOT INFO --------------------
enum RobotIDs {
    Dancebot1,
    Dancebot2,
    Dancebot3,
    Dancebot4,
    Dancebot5,
    Mothership,
    Polargraph,
    Marquee,
    TowerOfPower,
    NUM_IDS
};

// --------------------------- NETWORK INFO --------------------
IPAddress gateway(192,168,1,1);     // redirect any traffic with an unknown address to here
IPAddress subnet(255,255,255,251);  // 255-251-1 = 3 allowed IP addresses on the subnet
/**
 * Represents a single possible network that can be connected to or established.
 * Has a SSID and WPA2/PSK password associated with it.
 */
struct Credential {
    char *SSID;
    char *PASSWORD;
};

/**
 * CredentialsLog is a set of all credentials that the robot can check.
 */
// ADD ANY NETWORKS HERE, LIKE HOTSPOTS. DO NOT COMMIT TO GITHUB.
Credential CredentialsLog[] = {
    {
        "Demobot",
        "Demobots1234"
    },{
        "DemobotsNetwork",
        "Dem0b0tsRu1e!"
    }
};

// --------------------------- SERVER INFO --------------------
/**
 * Represents a single Demobot type and has an ID and IPAddress it belongs to.
 */
struct DemobotNetwork {
    int ID;
    IPAddress defaultIP;
};

/**
 * Robots is a set of all Demobots.
 */
DemobotNetwork Robots[] = {
    {
        Dancebot1,
        IPAddress(192,168,1,1)
    },{
        Dancebot2,
        IPAddress(192,168,1,1)
    },{
        Dancebot3,
        IPAddress(192,168,1,1)
    },{
        Dancebot4,
        IPAddress(192,168,1,1)
    },{
        Dancebot5,
        IPAddress(192,168,1,1)
    },{
        Mothership,
        IPAddress(192,168,1,1)
    },{
        Polargraph,
        IPAddress(192,168,1,2)
    },{
        Marquee,
        IPAddress(192,168,1,3)
    },{
        TowerOfPower,
        IPAddress(192,168,1,4)
    }
};

// -------------------- ROBOT SPECIFIC INFO --------------------

/* MODIFY BELOW WITH YOUR RELEVANT ROBOT */

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

// String fillers for sendHTML(). Position corresponds to DancebotStates enum.
char* dancebotStates[] {
    "Reset",
    "Walk",
    "Hop",
    "Wiggle",
    "Ankles",
    "Demo1",
    "Demo2"
};

struct Dancebot {
    int robotID;
    int robotState;
    int robotEyeColor;
    int robotExpression;
};

int numConnectedRobots = 0;
Dancebot connectedRobots[MAX_ROBOTS] = { {0, 0, 0, 0} };

#endif