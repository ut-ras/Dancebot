/**
 * File: DemobotNetwork.cpp
 * Author: Matthew Yu
 * Last Modified: 09/20/20
 * Project: Demobots General
 * Organization: UT IEEE RAS
 * Description: Implements definitions for the DemobotNetwork class, which allows
 * users to identify setup and connect a robot to the network given some credentials.
 */
#include "DemobotNetwork.h"

DemobotNetwork::DemobotNetwork(char* demobotName) {
    _demobotName = demobotName;
    // set server ip based on robot type
    switch(hash(_demobotName)) {
        case hash("Dancebot1"):
        case hash("Dancebot2"):
        case hash("Dancebot3"):
        case hash("Dancebot4"):
        case hash("Dancebot5"):
        case hash("Mothership"):
            _ipaddress = IPAddress(192,168,1,1);
            break;
        case hash("Polargraph"):
            _ipaddress = IPAddress(192,168,1,2);
            break;
        case hash("Marquee"):
            _ipaddress = IPAddress(192,168,1,3);
            break;
        case hash("TowerOfPower"):
            _ipaddress = IPAddress(192,168,1,4);
            break;
        default:
            break;
    }
    // set credentials
    reconfigureNetworks();
}

void DemobotNetwork::reconfigureNetworks() {
    _startup = getNetwork(_SSID, _PASSWORD);
    if (!_startup) {
        // take the first entry (assuming that it's filled) of the
        // credentials log
        _SSID = credentialsLog[0].SSID;
        _PASSWORD = credentialsLog[0].PASSWORD;
    }

    _connected = false;
}

bool DemobotNetwork::connectNetwork() {
    // if we never found a network with credentials, don't attempt to connect
    if (_SSID == nullptr || _PASSWORD == nullptr) { return false; }
    
    int retry = 0;
    loop {
        WiFi.begin(_SSID, _PASSWORD);
        // poll until we get connected or get a connection failure
        while (WiFi.status() != WL_CONNECTED) {
            // in WL_IDLE_STATUS of WL_CONNECT_FAILED
            if (WiFi.status() == WL_CONNECT_FAILED) {
                retry++;
                if (retry < RETRY_AMOUNT) break;
                else return false;
            }
            delay(RETRY_WAIT);
        }
        // if we're connected jump out
        if (WiFi.status() == WL_CONNECTED) {
            _connected = true;
            return true;
        }
    }
}

bool DemobotNetwork::isNetworkConnected() {
    return _connected;
}

bool DemobotNetwork::pingServer() {
    // if not connected to a network or doesn't have an ip address TODO: don't
    // know if !_ipaddress will work
    if (!_connected || !_ipaddress) return false;

    // send a root level GET request to see if the page exists
    String queryPath = "http://" + IpAddress2String(_ipaddress) + "/";
    http.begin(queryPath.c_str())
    return = http.GET();
}

int DemobotNetwork::sendGETRequest(String endpoint, String[] *keys, String[] *vals, int argSize, char* response) {
    // if not connected to a network or doesn't have an ip address TODO: don't
    // know if !_ipaddress will work
    if (!_connected || !_ipaddress) return -1;

    String queryPath = "http://" + IpAddress2String(_ipaddress) + endpoint;
    queryPath += '?';
    // append keys and values to the path
    for (int i = 0; i < argSize; i++) {
        queryPath += keys[i];
        queryPath += '=';
        queryPath += vals[i];
        if (i < (argSize - 1)) {
            queryPath += '&';
        }
    }

    http.begin(queryPath.c_str())
    int responseCode = http.GET();
    if (responseCode > 0) {
        response = http.getString();
    }
    return responseCode;
}

int DemobotNetwork::sendPOSTRequest(String endpoint, String[] *keys, String[] *vals, int argSize) {
    // if not connected to a network or doesn't have an ip address TODO: don't
    // know if !_ipaddress will work
    if (!_connected || !_ipaddress) return -1;

    String queryPath = "http://" + IpAddress2String(_ipaddress) + endpoint;
    http.begin(queryPath);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String data = "";
    // append keys and values to the path
    for (int i = 0; i < argSize; i++) {
        data += keys[i];
        data += '=';
        data += vals[i];
        if (i < (argSize - 1)) {
            data += '&';
        }
    }

    return http.POST(data);
}

char* DemobotNetwork::getNetworkSSID() {
    return _SSID;
}

char* DemobotNetwork::getNetworkPassword() {
    return _PASSWORD;
}

IPAddress DemobotNetwork::getIPAddress() {
    return _ipaddress;
}


// Private methods

int DemobotNetwork::hash(char* val) {
    int tot = 0;
    int count = 0;
    while(*valPtr != '\0') {
        // calculate the multiplying power for the digit
        int power = 1; // 10E0
        for (int i = 0; i < count; i++) {
            power *= 10; // 10Ecount
        }
        tot += *valPtr * power);
        count++;
        valPtr++;
    }
    return tot;
}

bool DemobotNetwork::getNetwork(char* ssid, char* password) {
    // 1. scan networks
    int networks = WiFi.scanNetworks();
    if (networks == 0) { return false; }
    // 2. assign the relevant network to the robot
    for (int i = 0; (i < numCredentials); i++) {
        for (int j = 0; j < networks; j++) {
            if(WiFi.SSID(j).equals(String(credentialsLog[i].SSID))) {
                ssid = credentialsLog[i].SSID;
                password = credentialsLog[i].PASSWORD;
                return true;
            }
        }
    }
    // 2b. if we didn't find a network, set it to default
    return false;
}