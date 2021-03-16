/**
 * File: DemobotClient.h
 * Author: Matthew Yu
 * Last Modified: 03/16/21
 * Project: Demobots General
 * Organization: UT IEEE RAS
 * Description: Implements definitions for the DemobotClient class, which
 * allows the ESP32 to send requests to a DemobotServer.
 */
#include "DemobotClient.h"


bool DemobotServer::connectServer() {
    int serverStatus = pingServer();
    switch (serverStatus) {
        case -2: /* Not connected to network. */
            return false;
        case -1: /* Server is not set up. */
            _http = new HTTPClient();
            _http.begin()
            break;
        default:
            Serial.println("Server status: ");
            Serial.println(serverStatus);
    }
}

bool DemobotServer::disconnectServer() {
    return false;
}

int DemobotNetwork::sendGETRequest(
    const String endpoint,
    const String keys[],
    const String vals[],
    const int argSize,
    String *response
) const {
    if (!_connected) return -1;

    String queryPath = "http://" + IpAddress2String(_ipAddress) + endpoint;
    queryPath += '?';

    /* Append keys and values to the path. */
    for (int i = 0; i < argSize; i++) {
        queryPath += keys[i];
        queryPath += '=';
        queryPath += vals[i];
        if (i < (argSize - 1)) {
            queryPath += '&';
        }
    }

    /* Send the request. */
    _http->begin(queryPath.c_str());
    int responseCode = _http->GET();
    if (responseCode > 0) {
        String str = String(_http->getString());
        response = &(str);
    }
    return responseCode;
}

int DemobotNetwork::sendPOSTRequest(
    const String endpoint,
    const String keys[],
    const String vals[],
    int argSize,
    String *response
) const {
    if (!_connected) return -1;

    String queryPath = "http://" + IpAddress2String(_ipAddress) + endpoint;
    _http->begin(queryPath);
    _http->addHeader("Content-Type", "application/x-www-form-urlencoded");
    String data = "";

    /* Append keys and values to the path. */
    for (int i = 0; i < argSize; i++) {
        data += keys[i];
        data += '=';
        data += vals[i];
        if (i < (argSize - 1)) {
            data += '&';
        }
    }

    /* Send the request. */
    int responseCode = _http->POST(data);
    if (responseCode > 0) {
        String str = String(_http->getString());
        response = &(str);
    }
    return responseCode;
}