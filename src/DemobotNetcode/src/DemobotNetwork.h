/**
 * File: DemobotNetwork.h
 * Author: Matthew Yu
 * Last Modified: 03/16/21
 * Project: Demobots General
 * Organization: UT IEEE RAS
 * Description: Implements definitions for the DemobotNetwork class, which
 * allows users to identify, setup, and connect a robot to the network given
 * some credentials.
 */
#pragma once

#include <IPAddress.h>
#include <WiFi.h>


#define RETRY_WAIT 200      /** 200 ms. */
#define RETRY_AMOUNT 3
#define MAX_STRING_SIZE 20

extern IPAddress gateway;
extern IPAddress subnet;
extern IPAddress primaryDNS;
extern IPAddress secondaryDNS;

class DemobotNetwork {
    /**
     * The DemobotNetwork class manages Demobot network code. It allows
     * robots to find and connect to a network and IP address given just a
     * Demobot enum.
     */
    public:
        /** List of all possible Demobots the DemobotNetwork supports. */
        enum DemobotID {
            DANCEBOT_1,
            DANCEBOT_2,
            DANCEBOT_3,
            DANCEBOT_4,
            DANCEBOT_5,
            MOTHERSHIP,
            POLARGRAPH,
            MARQUEE,
            TOWER_OF_POWER
        };

        /**
         * Creates a new DemobotNetwork object and fills in the relevant network
         * and IP address information, if any.
         * 
         * @param[in] ID Enum referencing the specific Demobot.
         */
        explicit DemobotNetwork(const DemobotID ID);

        /**
         * Attempts to reset the network configuration. Useful for when we know
         * a network has dropped and we want to join/start a new one.
         */
        void reconfigureNetworks();

        /**
         * Attempts to connect to the network selected during object
         * instantiation. Fails if we could not find a network during object
         * instantiation or we can't connect to the network.
         * 
         * @return True if can connect to the network, false otherwise.
         */
        bool connectNetwork();

        /** Disconnects from the current network, if any. */
        void disconnectNetwork();

        /**
         * Returns the network SSID.
         *
         * @return Network SSID. May be nullptr if configured incorrectly.
         */
        char* getNetworkSSID() const;

        /**
         * Returns the network password.
         *
         * @return Network password. May be nullptr if configured incorrectly.
         */
        char* getNetworkPassword() const;

        /**
         * Checks to see if we're currently connected to a network.
         * 
         * @return True if we're currently connected to a network, false otherwise.
         */
        bool isNetworkConnected() const;

        /**
         * Returns the relevant server IP address.
         * 
         * @return Server IPAddress that corresponds to Demobot name.
         */
        IPAddress getIPAddress() const;

        /**
         * Converts an IPAddress to a string.
         * @author: apicquot from https://forum.arduino.cc/index.php?topic=228884.0
         * 
         * @param[in] ipAddress IPAddress object.
         * @return String concatenation of the IP address.
         */
        String IpAddress2String(const IPAddress ipAddress) const;

        ~DemobotNetwork();

    private:
        /**
         * Goes through the available networks and attempts to find one that
         * matches the internal credentials log. If we do find one, set the ssid
         * and password and return true.
         * 
         * @return True if we found a matching network and set SSID and password,
         *      false elsewise.
         */
        bool getNetwork();

    private:
        /** IP address of server to connect to or host. */
        IPAddress _ipAddress;

        /** Network info */
        char* _SSID;
        char* _PASSWORD;
        enum WiFi_Mode { AP, STA };
        enum WiFi_Mode _mode;

        /**
         * Represents a single possible network that can be connected to or established.
         * Has a SSID and WPA2/PSK password associated with it.
         */
        struct Credential {
            const char SSID[MAX_STRING_SIZE];
            const char PASSWORD[MAX_STRING_SIZE];
        };

        /** Credentials log. */
        const int numCredentials = 4;
        const Credential *credentialsLog = nullptr;
};
