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
        Serial.println("[DEBUG] [setupNetworking] Unable to identify an available network.");
        if (!setupAPNetwork(network)) return false;
    }
    /* Attempt to ping server. */
    Serial.println("[DEBUG] [setupNetworking] Pinging server.");
    if (network.pingServer()) {
        /* And the remote server is up. */
        Serial.println("[DEBUG] [setupNetworking] Server responded back.");
        return true;
    } else {
        Serial.println("[DEBUG] [setupNetworking] No server response. Setting up our own.");
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
        Serial.println("[DEBUG] [setupAPNetwork] Network was improperly configured and could not setup a network on AP mode.");
        return false;
    }

    Serial.println("[DEBUG] [setupAPNetwork] Setting up default network at: " + String(ssid));
    if (!WiFi.softAP(ssid, pass)) {
        Serial.println("[DEBUG] [setupAPNetwork] Unable to setup access point.");
        return false;
    }

    /* Give the network some time to run up. */
    delay(1500);
    return true;
}

bool setupServer(DemobotNetwork &network) {
    IPAddress ip = network.getIPAddress();
    if (!ip) {
        Serial.println("[DEBUG] [setupServer] Server IP was improperly configured and could not setup a server on AP mode.");
        return false;
    }

    Serial.println("[DEBUG] [setupServer] Setting up server at: " + network.IpAddress2String(network.getIPAddress()));
    if (!WiFi.config(ip, gateway, subnet)) {
        Serial.println("[DEBUG] [setupServer] Unable to setup server IP.");
        return false;
    }

    char buffer[100] = {'\0'};
    snprintf(buffer, 100, "[DEBUG] [setupServer] IP ADDRESS: %s.", WiFi.localIP().toString().c_str());
    Serial.println(buffer);

    /* Give the server some time to run up. */
    startServer();
    delay(1500);
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
WiFiServer server(80);
void startServer() {
    // TODO: rewrite this to comply with new communication protocol.
    server.begin();
    while (true) {
        Serial.println("Looking for clients.");
        WiFiClient client = server.available();
        if (client) {
            Serial.println("Client available.");
            while (client.connected()) {
                if (client.available()) {
                    String header;
                    char c = '\0';
                    do {
                        c = client.read();
                        Serial.write(c);
                        header += c;
                    } while (c != '\n');
                    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                    // and a content-type so the client knows what's coming, then a blank line:
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type:text/html");
                    client.println("Connection: close");
                    client.println();
                }
            }
            client.stop();
            Serial.println("Client disconnected.");
        }
    }


}