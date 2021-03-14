/**
 * File: DemobotNetwork.cpp
 * Author: Matthew Yu
 * Last Modified: 03/08/21
 * Project: Demobots General
 * Organization: UT IEEE RAS
 * Description: Implements definitions for the DemobotNetwork class, which allows
 * users to identify setup and connect a robot to the network given some credentials.
 */
#include "DemobotNetwork.h"

/** Redirect any traffic with an unknown address to here. */
IPAddress gateway(192,168,1,1);

/** 255-245-1 = 9 allowed IP addresses on the subnet. */
IPAddress subnet(255,255,0,0);

/** Public methods. */

DemobotNetwork::DemobotNetwork(const DemobotID ID) {
    /* Generate possible network credentials. */
    credentialsLog = new Credential[numCredentials] { 
        Credential{"Demobot", "Demobots1234"},
        Credential{"DemobotsNetwork", "Dem0b0tsRu1e!"},
        Credential{"", ""},
        Credential{"", ""} 
    };

    http = new HTTPClient();
    _demobot = ID;

    /* Set server ip based on robot type. */
    switch(ID) {
        case DANCEBOT_1:
        case DANCEBOT_2:
        case DANCEBOT_3:
        case DANCEBOT_4:
        case DANCEBOT_5:
        case MOTHERSHIP:
            _ipaddress = IPAddress(192,168,1,1);
            break;
        case POLARGRAPH:
            _ipaddress = IPAddress(192,168,1,2);
            break;
        case MARQUEE:
            _ipaddress = IPAddress(192,168,1,3);
            break;
        case TOWER_OF_POWER:
            _ipaddress = IPAddress(192,168,1,4);
            break;
        default:
            _ipaddress = IPAddress(192,168,1,0);
            break;
    }

    /* Set credentials. */
    reconfigureNetworks();
}

void DemobotNetwork::reconfigureNetworks() {
    _startup = getNetwork(_SSID, _PASSWORD);
    if (!_startup) {
        /* Take the first entry (assuming that it's filled) of the credentials log. */
        _SSID = const_cast<char*>(credentialsLog[0].SSID);
        _PASSWORD = const_cast<char*>(credentialsLog[0].PASSWORD);
    }

    _connected = false;
}

bool DemobotNetwork::connectNetwork() {
    /* If we never found a network with credentials, don't attempt to connect. */
    if (_SSID == nullptr || _PASSWORD == nullptr) { return false; }
    
    int retry = 0;
    while (true) {
        WiFi.begin(_SSID, _PASSWORD);
        /* Poll until we get connected or get a connection failure. */
        while (WiFi.status() != WL_CONNECTED) {
            /* In WL_IDLE_STATUS of WL_CONNECT_FAILED. */
            if (WiFi.status() == WL_CONNECT_FAILED) {
                retry++;
                if (retry < RETRY_AMOUNT) break;
                else return false;
            }
            delay(RETRY_WAIT);
        }
        /* If we're connected, jump out. */
        if (WiFi.status() == WL_CONNECTED) {
            _connected = true;
            return true;
        }
    }
}

bool DemobotNetwork::isNetworkConnected() const {
    return _connected;
}

int DemobotNetwork::pingServer() const {
    /* If not connected to a network or doesn't have an ip address. */
    // TODO: don't know if !_ipaddress will work
    if (!_connected || !_ipaddress) return false;

    /* Send a root level GET request to see if the page exists. */
    String queryPath = "http://" + IpAddress2String(_ipaddress) + "/";
    http->begin(queryPath.c_str());
    return http->GET();
}

int DemobotNetwork::sendGETRequest(
    const String endpoint,
    const String keys[],
    const String vals[],
    const int argSize,
    String *response
) const {
    /* If not connected to a network or doesn't have an ip address. */
    // TODO: don't know if !_ipaddress will work
    if (!_connected || !_ipaddress) return -1;

    String queryPath = "http://" + IpAddress2String(_ipaddress) + endpoint;
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
    http->begin(queryPath.c_str());
    int responseCode = http->GET();
    if (responseCode > 0) {
        String str = String(http->getString());
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
    /* If not connected to a network or doesn't have an ip address. */
    // TODO: don't know if !_ipaddress will work
    if (!_connected || !_ipaddress) return -1;

    String queryPath = "http://" + IpAddress2String(_ipaddress) + endpoint;
    http->begin(queryPath);
    http->addHeader("Content-Type", "application/x-www-form-urlencoded");
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
    int responseCode = http->POST(data);
    if (responseCode > 0) {
        String str = String(http->getString());
        response = &(str);
    }
    return responseCode;
}

char* DemobotNetwork::getNetworkSSID() const {
    return _SSID;
}

char* DemobotNetwork::getNetworkPassword() const {
    return _PASSWORD;
}

void DemobotNetwork::shutdownNetwork() {
    delete[] credentialsLog;
    delete http;
}

IPAddress DemobotNetwork::getIPAddress() const {
    return _ipaddress;
}

String DemobotNetwork::IpAddress2String(const IPAddress ipAddress) const {
    return String(ipAddress[0]) + String(".") +\
        String(ipAddress[1]) + String(".") +\
        String(ipAddress[2]) + String(".") +\
        String(ipAddress[3]) ;
}

/** Private methods. */

bool DemobotNetwork::getNetwork(char ssid[], char password[]) const {
    /* 1. scan networks. */
    int networks = WiFi.scanNetworks();
    if (networks == 0) { return false; }

    /* 2. assign the relevant network to the robot. */
    for (int i = 0; i < numCredentials; i++) {
        for (int j = 0; j < networks; j++) {
            if(WiFi.SSID(j).equals(String(credentialsLog[i].SSID))) {
                ssid = const_cast<char*>(credentialsLog[i].SSID);
                password = const_cast<char*>(credentialsLog[i].PASSWORD);
                return true;
            }
        }
    }

    /* 2b. if we didn't find a network, set it to default. */
    return false;
}