/**
 * File: DemobotNetwork.cpp
 * Author: Matthew Yu
 * Last Modified: 03/16/21
 * Project: Demobots General
 * Organization: UT IEEE RAS
 * Description: Implements definitions for the DemobotNetwork class, which
 * allows users to identify, setup, and connect a robot to the network given
 * some credentials.
 */
#include "DemobotNetwork.h"


/** Redirect any traffic with an unknown address to here. */
IPAddress gateway(192,168,1,1);

/** 255-245-1 = 9 allowed IP addresses on the subnet. */
IPAddress subnet(255,255,0,0);

/** Optional Domain Name Systems. */
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

/** Public methods. */

DemobotNetwork::DemobotNetwork(const DemobotID ID) {
    /* Generate possible network credentials. */
    credentialsLog = new Credential[numCredentials] {
        Credential{"Demobot", "Demobots1234"},
        Credential{"DemobotsNetwork", "Dem0b0tsRu1e!"},
        Credential{"***REMOVED***", "***REMOVED***"},
        Credential{"", ""}
    };

    /* Set server IP based on robot ID. */
    switch(ID) {
        case DANCEBOT_1:
        case DANCEBOT_2:
        case DANCEBOT_3:
        case DANCEBOT_4:
        case DANCEBOT_5:
        case MOTHERSHIP:
            _ipAddress = IPAddress(192,168,1,1);
            break;
        case POLARGRAPH:
            _ipAddress = IPAddress(192,168,1,2);
            break;
        case MARQUEE:
            _ipAddress = IPAddress(192,168,1,3);
            break;
        case TOWER_OF_POWER:
            _ipAddress = IPAddress(192,168,1,4);
            break;
        default:
            _ipAddress = IPAddress(192,168,1,0);
            break;
    }

    /* Set credentials. */
    reconfigureNetworks();
}

void DemobotNetwork::reconfigureNetworks() {
    if (!getNetwork()) {
        /* Upon failure to find a relevant network, take the first entry of the
         * credentials log and use it to start its own. */
        _SSID = const_cast<char*>(credentialsLog[0].SSID);
        _PASSWORD = const_cast<char*>(credentialsLog[0].PASSWORD);
        _mode = AP;
        Serial.println("Network configured for AP mode.");
    } else {
        _mode = STA;
        Serial.println("Network configured for STA mode.");
    }
}

bool DemobotNetwork::connectNetwork() {
    /* Don't attempt to connect without valid credentials. */
    if (_SSID == nullptr || _PASSWORD == nullptr) return false;

    if (_mode == AP) { /* Access Point mode. */
        Serial.println("Connecting via AP mode.");

        /* Configure IP. */
        if (!WiFi.softAPConfig(_ipAddress, gateway, subnet)) {
            Serial.println("AP failed to configure.");
        }

        /* Set up our own access point. */
        WiFi.mode(WIFI_AP);
        if (!WiFi.softAP(_SSID, _PASSWORD)) {
            Serial.println("AP failed to start.");
        }

        Serial.print("Connected to network ");
        Serial.println(_SSID);
        Serial.print("With IP address ");
        Serial.println(WiFi.softAPIP());
    } else { /* Station mode. */
        Serial.println("Connecting via STA mode.");

        /* Configure IP. */
        if (!WiFi.config(_ipAddress, gateway, subnet, primaryDNS, secondaryDNS)) {
            Serial.println("STA failed to configure.");
        }

        /* Connect to another network in station mode. */
        WiFi.mode(WIFI_STA);
        WiFi.begin(_SSID, _PASSWORD);

        /* Poll until we get connected or get a connection failure. */
        int retry = 0;
        while (WiFi.status() != WL_CONNECTED) {
            /* In WL_IDLE_STATUS of WL_CONNECT_FAILED. */
            if (WiFi.status() == WL_CONNECT_FAILED) {
                retry++;
                if (retry < RETRY_AMOUNT) break;
                else {
                    Serial.println("STA failed to start 3 times.");
                    return false;
                }
            }
            delay(RETRY_WAIT);
        }

        /* If we're connected, jump out. */
        if (WiFi.status() == WL_CONNECTED) {
            Serial.print("Connected to network ");
            Serial.println(_SSID);
            Serial.print("With IP address ");
            Serial.println(WiFi.localIP());
        }
    }

    return true;
}

void DemobotNetwork::disconnectNetwork() {
    WiFi.disconnect();
}

char* DemobotNetwork::getNetworkSSID() const {
    return _SSID;
}

char* DemobotNetwork::getNetworkPassword() const {
    return _PASSWORD;
}

bool DemobotNetwork::isNetworkConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

IPAddress DemobotNetwork::getIPAddress() const {
    return _ipAddress;
}

String DemobotNetwork::IpAddress2String(const IPAddress ipAddress) const {
    return String(ipAddress[0]) + String(".") +\
        String(ipAddress[1]) + String(".") +\
        String(ipAddress[2]) + String(".") +\
        String(ipAddress[3]) ;
}

DemobotNetwork::~DemobotNetwork() {
    disconnectNetwork();
    delete[] credentialsLog;
}

/** Private methods. */

bool DemobotNetwork::getNetwork() {
    /* 1. scan networks. */
    int networks = WiFi.scanNetworks();
    if (networks == 0) { return false; }

    /* 2. assign the relevant network to the robot. */
    for (int i = 0; i < numCredentials; i++) {
        for (int j = 0; j < networks; j++) {
            if(WiFi.SSID(j).equals(String(credentialsLog[i].SSID))) {
                _SSID = const_cast<char*>(credentialsLog[i].SSID);
                _PASSWORD = const_cast<char*>(credentialsLog[i].PASSWORD);
                return true;
            }
        }
    }

    /* 2b. if we didn't find a network, set it to default. */
    return false;
}