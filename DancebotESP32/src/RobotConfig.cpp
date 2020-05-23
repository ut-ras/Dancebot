/**
 * Author: Matthew Yu
 * Last Modified: 05/22/20
 * Project: Demobots General
 * File: RobotConfig.cpp
 * Description: A config file to be used by WebController.h and DemobotAPI.h.
 * Organization: UT IEEE RAS
 * NOTE: Modify the MODIFIABLE INFO section for your robot.
 */
#include "RobotConfig.h"

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
// --------------------------- SERVER INFO --------------------
IPAddress gateway(192,168,1,1);     // redirect any traffic with an unknown address to here
IPAddress subnet(255,255,0,0);  // 255-245-1 = 9 allowed IP addresses on the subnet
/**
 * Robots is a set of all Demobots.
 */
DemobotNetwork Robots[] = {
    {
        Dancebot1,
        "Dancebot1",
        IPAddress(192,168,1,1)
    },{
        Dancebot2,
        "Dancebot2",
        IPAddress(192,168,1,1)
    },{
        Dancebot3,
        "Dancebot3",
        IPAddress(192,168,1,1)
    },{
        Dancebot4,
        "Dancebot4",
        IPAddress(192,168,1,1)
    },{
        Dancebot5,
        "Dancebot5",
        IPAddress(192,168,1,1)
    },{
        Mothership,
        "Mothership",
        IPAddress(192,168,1,1)
    },{
        Polargraph,
        "Polargraph",
        IPAddress(192,168,1,2)
    },{
        Marquee,
        "Marquee",
        IPAddress(192,168,1,3)
    },{
        TowerOfPower,
        "TowerOfPower",
        IPAddress(192,168,1,4)
    }
};


// --------------------------- MODIFIABLE INFO ---------------
// SET TO YOUR ROBOT NAME LISTED IN RobotIDs 
const int ROBOT_ID = Dancebot1;
/**
 * CredentialsLog is a set of all credentials that the robot can check.
 */
// ADD ANY NETWORKS HERE, LIKE HOTSPOTS. DO NOT COMMIT TO GITHUB.
// IF YOU DO, USE BFG to sanitize sensitive info. https://help.github.com/en/github/authenticating-to-github/removing-sensitive-data-from-a-repository
Credential CredentialsLog[5] = {
    {
        "Demobot",
        "Demobots1234"
    },{
        "DemobotsNetwork",
        "Dem0b0tsRu1e!"
    },{
        "",
        ""
    },{
        "",
        ""
    },{
        "",
        ""
    }
};
// SET TO YOUR DEFAULT NETWORK ID INDEX LISTED IN CredentialsLog
const int DEFAULT_NETWORK_ID = 0;
/* MODIFY BELOW WITH YOUR RELEVANT ROBOT */
// String fillers for sendHTML(). Position corresponds to DancebotStates enum.
char* dancebotStates[] {
    "reset",
    "walk",
    "hop",
    "wiggle",
    "ankles",
    "demo1",
    "demo2"
};
int numConnectedRobots = 0;
Dancebot connectedRobots[MAX_ROBOTS] = { {0, 0, 0, 0} };

