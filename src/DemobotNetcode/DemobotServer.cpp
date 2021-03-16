/**
 * File: DemobotServer.cpp
 * Author: Matthew Yu
 * Last Modified: 03/16/21
 * Project: Demobots General
 * Organization: UT IEEE RAS
 * Description: Implements definitions for the DemobotServer class, which
 * allows the ESP32 to set up and manage a web server.
 */
#include "DemobotServer.h"


/** Public methods. */

DemobotServer::DemobotServer() {
    _server = new AsyncWebServer(80);
}

DemobotServer::DemobotServer(const unsigned int port) {
    _server = new AsyncWebServer(port);
}

int DemobotServer::pingServer() const {
    /* Send a root level GET request to see if the server exists. */


    /* Send a root level GET request to see if the page exists. */
    String queryPath = "http://" + IpAddress2String(_ipAddress) + "/";
    _http->begin(queryPath.c_str());
    return _http->GET();
}

DemobotServer::~DemobotServer() {
    disconnectServer();
    delete _server;
}