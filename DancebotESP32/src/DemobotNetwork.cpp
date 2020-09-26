/**
 * File: DemobotNetwork.cpp
 * Author: Matthew Yu
 * Last Modified: 09/26/20
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

DemobotNetwork::DemobotNetwork(const String demobotName) {
    credentialsLog = new Credential[numCredentials] { 
        Credential{"Demobot", "Demobots1234"},
        Credential{"DemobotsNetwork", "Dem0b0tsRu1e!"},
        Credential{"", ""},
        Credential{"", ""} 
    };

    http = new HTTPClient();
    _demobotName = demobotName;
    /* Set server ip based on robot type. */
    char* charArray = new char[_demobotName.length() + 1];
    _demobotName.toCharArray(charArray, _demobotName.length());
    switch(hash(charArray)) {
        case 40742: // Dancebot1
        case 40998: // Dancebot2
        case 41254: // Dancebot3
        case 41510: // Dancebot4
        case 41766: // Dancebot5
        case 111755: // Mothership
            _ipaddress = IPAddress(192,168,1,1);
            break;
        case 108262: // Polargraph
            _ipaddress = IPAddress(192,168,1,2);
            break;
        case 13199: // Marquee
            _ipaddress = IPAddress(192,168,1,3);
            break;
        case 448950: // TowerOfPower
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

int DemobotNetwork::sendGETRequest(const String endpoint, const String keys[], const String vals[], const int argSize, String *response) const {
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

int DemobotNetwork::sendPOSTRequest(const String endpoint, const String keys[], const String vals[], int argSize, String *response) const {
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

int DemobotNetwork::hash(char* valptr) const {
    /**
     * This is dumb hash and really shouldn't be used for anything but small
     * subsets of problems where we're pretty confident that collisions are small.
     * 
     * We're just taking the char and multiplying it by 2 by its place:
     * (i.e. hello -> h*2^0 + e*2^1 + l*2^2 + l*2^3 + o*2^4).
     * Preferably, for no collisions, you'd want to have everything be a power
     * of 62 (52 characters - lower and upper - and 10 digits), i.e. 36^place. But this is
     * prohibitively expensive. Even 10^place is expensive for words with 9+
     * characters like Dancebot1.
     * 
     * By expensive, I'm referring to the integer output which for this
     * application corresponds to the switch case. Which means we might overflow
     * our int type if the string input is too long.
     */
    int tot = 0;
    int count = 0;
    while(*valptr != '\0') {
        /* Calculate the multiplying power for the digit. */
        int power = 1; // 2^0
        for (int i = 0; i < count; i++) {
            power *= 2; // 2^count
        }
        tot += *valptr * power;
        count++;
        valptr++;
    }
    return tot;
}

bool DemobotNetwork::getNetwork(char ssid[], char password[]) const {
    /* 1. scan networks. */
    int networks = WiFi.scanNetworks();
    if (networks == 0) { return false; }

    /* 2. assign the relevant network to the robot. */
    for (int i = 0; (i < numCredentials); i++) {
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