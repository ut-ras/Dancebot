/**
 * Author: Matthew Yu
 * Last Modified: 09/26/20
 * Project: Dancebot Swarm
 * File: WebController.cpp
 * Description: A generic web controller for controlling various Demobots Projects. 
 *      Robots should be able to: connect to a remote server and receive/send commands, 
 *      as well as spin up their own server and serve a webpage to the user to directly 
 *      interact with it.
 * Organization: UT IEEE RAS
 */
#include "WebController.h"

/* -----------------------------SETUP------------------------------- */

bool setupNetworking(DemobotNetwork &network) {
    /* Attempt to connect to network */
    if (!network.connectNetwork()) {
        Serial.println("[WARN] [setupNetworking] Unable to identify an available network.");
        if (!setupAPNetwork(network)) return false;
    }
    /* Attempt to ping server. */
    if (network.pingServer()) {
        /* And the remote server is up. */
        return true;
    } else {
        /* Remote server isn't up. Start our own? */
        if (!setupServer(network)) return false;
        /* Attempt to ping again */
        if (network.pingServer()) return true;
        else return false;
    }
}

bool setupAPNetwork(DemobotNetwork &network) {
    /* set up our own network. */
    char* ssid = network.getNetworkSSID();
    char* pass = network.getNetworkPassword();
    if ((ssid == nullptr) || (pass == nullptr)) {
        Serial.println("[ERROR] [setupAPNetwork] Network was improperly configured and could not setup a network on AP mode.");
        return false;
    }

    Serial.println("[setupAPNetwork] Setting up default network at: " + String(ssid));
    if (!WiFi.softAP(ssid, pass)) {
        Serial.println("[ERROR] [setupAPNetwork] Unable to setup access point.");
        return false;
    }

    /* Give the network some time to run up. */
    delay(500);
    return true;
}

bool setupServer(DemobotNetwork &network) {
    IPAddress ip = network.getIPAddress();
    if (!ip) {
        Serial.println("[ERROR] [setupServer] Server IP was improperly configured and could not setup a server on AP mode.");
        return false;
    }

    Serial.println("[setupServer] Setting up server at: " + network.IpAddress2String(network.getIPAddress()));
    if (!WiFi.softAPConfig(ip, gateway, subnet)) {
        Serial.println("[ERROR] [setupServer] Unable to setup server IP.");
        return false;
    }

    /* Give the server some time to run up. */
    startServer();
    delay(500);
    return true;
}

/* ------------------------CLIENT REQUESTS-------------------------- */

String joinServer(DemobotNetwork &network, const Dancebot &dancebot) {
    static const int numArgs = 5;
    String keys[numArgs] = {
        "ID",
        "CHARGE",
        "MOVE",
        "EC",
        "EE"
    };
    String vals[numArgs] = {
        String(dancebot.robotID),
        String(dancebot.soc),
        state2string(dancebot.robotState),
        expression2String(dancebot.robotExpression),
        eyeColor2String(dancebot.robotEyeColor)
    };

    String *response = nullptr;
    int responseCode = network.sendGETRequest("/robotJoin", keys, vals, numArgs, response);
    if (responseCode != OK || response == nullptr) return String("");
    else return String("[JOIN]");
}

String getState(DemobotNetwork &network, const Dancebot &dancebot) {
    static const int numArgs = 1;
    String keys[numArgs] = {
        "ID"
    };
    String vals[numArgs] = {
        String(dancebot.robotID)
    };

    String *response = nullptr;
    int responseCode = network.sendGETRequest("/robotUpdate", keys, vals, numArgs, response);
    if (responseCode != OK || response == nullptr) return String("");
    else return String("[UPDT] " + *response);
}

String leaveServer(DemobotNetwork &network, const Dancebot &dancebot) {
    static const int numArgs = 2;
    String keys[numArgs] = {
        "ID",
        "CHARGE"
    };
    String vals[numArgs] = {
        String(dancebot.robotID),
        String(dancebot.soc)
    };

    String *response = nullptr;
    int responseCode = network.sendGETRequest("/robotLeave", keys, vals, numArgs, response);
    if (responseCode != OK || response == nullptr) return String("");
    else return String("[LEFT]");
}

/* ---------------------SERVER HANDLER REQUESTS--------------------- */

/**
 * startServer sets up the URL hooks.
 */
void startServer() {
    // TODO: rewrite this to comply with new communication protocol.
}