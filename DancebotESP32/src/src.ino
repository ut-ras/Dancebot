/**
 * @file src.ino
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Entry point and main for dancebot swarm. Dance Dance Dance! https://youtu.be/ciEHtGpSviE
 * @version 0.2.0
 * @date 2023-04-16
 * @copyright Copyright (c) 2023
 *
 */
#include <Arduino.h>
#include <ArduinoLog.h>
#include "DemobotAPI.hpp"
#include "./netcode/src/DemobotNetwork.hpp"
#include "./netcode/src/DemobotServer.hpp"
#include "./netcode/src/DemobotClient.hpp"
#include "utils.hpp"
#include "SPIFFS.h"

#define SETUP_TIMEOUT 60000
#define UPDATE_TIMEOUT 250
/**
 * Example flow for a Demobots sketch with an ESP32:
 * setup() {
 *   setupNetworking()
 *   initializeRobot()
 * }
 *
 * loop() {
 *   do some movement
 *   look for update from server and update state
 * }
 */

/** Dancebot instantiation */
const char *dancebot_name = "Dancebot_0";
Dancebot dancebot {
    const_cast<char*>(dancebot_name),
    0,
    100.0,
    Reset,
    None,
    Off
};

/** Network and WebController instantiation. */
DemobotNetwork *network_conf;
DemobotServer *server;
DemobotClient *client;
// WebController *controller;

void onRoot(AsyncWebServerRequest *request) {
    Log.trace("[SERVER] The server got a client trying to access root!");
    request->send(200, "text/plain", "Hello, world");
}

#define NUM_ROUTES 1
const struct DemobotServer::Route routes[NUM_ROUTES] = {
    {"/", WebRequestMethod::HTTP_GET, onRoot},
};

void setup_logging(void) {
    Serial.println("Hello world! Initializing the logger...");
    Log.begin(LOG_LEVEL_NOTICE, &Serial);
    Log.setPrefix(print_timestamp);
    Log.setSuffix(print_newline);
}

bool setup_SPIFFS(void) {
    Log.notice("Setting up SPIFFS and checking for necessary files.");
    if (!SPIFFS.begin()) {
        Log.notice("An error has occurred while mounting SPIFFS.");
        return false;
    }

    /* Check for html and js file that is to be served by the server if set up. */
    const char *html_path = "/test.html";
    File html = SPIFFS.open(html_path);
    if (!html) {
        Log.notice("%s does not exist.", html_path);
        return false;
    }

    const char *js_path = "/test.js";
    File js = SPIFFS.open(js_path);
    if (!js) {
        Log.notice("%s does not exist.", js_path);
        return false;
    }

    Log.notice("All relevant files found.");
    return true;
}

bool setup_network(enum DemobotServer::StaticIPMode *mode) {
    Log.notice("Connecting to a network.");
    network_conf = new DemobotNetwork(dancebot.name, DemobotNetwork::DANCEBOT);
    bool success = network_conf->connect_network();
    if (!success) {
        Log.notice("No matching networks was found. Setting up our own network.");
        success = network_conf->setup_network();
        if (!success) {
            Log.notice("No networks were established. Defaulting to autonomous mode.");
            return false;
        } else {
            *mode = DemobotServer::MODE_AP;
        }
    } else {
        *mode = DemobotServer::MODE_STA;
    }
    Log.notice("A network connection has been established!");
    return true;
}

bool setup_client_server(enum DemobotServer::StaticIPMode mode) {
    /** Setup client. */
    Log.notice("Setting up our client.");
    client = new DemobotClient(DemobotClient::DANCEBOT);

    /** Connect to server. */
    Log.notice("Connecting to a server.");
    server = new DemobotServer(DemobotServer::DANCEBOT);
    if (!client->ping_server()) {
        Log.notice("We did not find a server online. Setting up our own server.");
        if (!server->setup_server(routes, NUM_ROUTES, mode)) {
            Log.notice("Unable to setup our own server. Defaulting to autonomous mode.");
            return false;
        } else {
            Log.notice("Starting server.");
            server->start_server();
        }
    }
    if (!client->ping_server()) {
        Log.notice("Unable to communicate to our own server. Defaulting to autonomous mode.");
        return false;
    }

    Log.notice("A server connection is available!");
    return true;
}

void setup() {
    Serial.begin(9600);
    while (!Serial);

    /** Give the user some time to open up the serial monitor. */
    delay(2500); // 2.5 seconds

    setup_logging();
    setup_SPIFFS();

    enum DemobotServer::StaticIPMode mode;
    if (!setup_network(&mode)) {
        // TODO: Setup autonomous mode.
        return;
    }

    if (!setup_client_server(mode)) {
        // TODO: Setup autonomous mode.
        return;
    }

    /** TODO: Initialize robot state and register current state with server. */
}

void loop() {
}
