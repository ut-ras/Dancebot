/**
 * Author: Matthew Yu
 * Last Modified: 05/06/20
 * Project: Demobots General
 * File: WiFiConfig.h
 * Description: A config file to be used by WebController.h.
 * Organization: UT IEEE RAS
 */
#ifndef WIFICONFIG
#define WIFICONFIG

// update this locally. Never push or update the repository with information containing your SSID and password.
const int DEFAULT_NETWORK_ID = 0;   // default network will be Demobot
const int ROBOT_IDX = 0;            // UPDATE THIS BASED ON YOUR NETWORK
const int CredentialsLogLength = 2;
const int RETRY_WAIT = 200;         // 200ms

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
Credential CredentialsLog[2] = {
    {
        "Demobot",
        "Demobots1234"
    },{
        "DemobotsNetwork",
        "Dem0b0tsRu1e!"
    }
    /* ADD ANY NETWORKS HERE, LIKE HOTSPOTS. DO NOT COMMIT TO GITHUB. */
};

/**
 * Represents a single Demobot type and has an ID and IPAddress it belongs to.
 */
struct DemobotNetwork {
    char *ID;
    IPAddress defaultIP;
};

/**
 * Robots is a set of all Demobots.
 */
DemobotNetwork Robots[] = {
    {
        "Dancebot1",
        IPAddress(192,168,1,1)
    },{
        "Dancebot2",
        IPAddress(192,168,1,1)
    },{
        "Dancebot3",
        IPAddress(192,168,1,1)
    },{
        "Dancebot4",
        IPAddress(192,168,1,1)
    },{
        "Dancebot5",
        IPAddress(192,168,1,1)
    },{
        "Mothership",
        IPAddress(192,168,1,1)
    },{
        "Polargraph",
        IPAddress(192,168,1,2)
    },{
        "Marquee",
        IPAddress(192,168,1,3)
    }
};

#endif