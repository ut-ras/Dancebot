/**
 * File: DemobotNetwork.h
 * Author: Matthew Yu
 * Last Modified: 09/26/20
 * Project: Demobots General
 * Organization: UT IEEE RAS
 * Description: Contains definitions for the DemobotNetwork class, which allows
 * users to identify setup and connect a robot to the network given some credentials.
 */
#pragma once

#include <IPAddress.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define RETRY_WAIT 200      // 200 ms
#define RETRY_AMOUNT 10

extern IPAddress gateway;
extern IPAddress subnet;

/**
 * Definition of an implementation for the DemobotNetwork class.
 *
 * The DemobotNetwork class manages Demobot networking code. It allows robots to
 * find and connect to networks and servers given just a demobot name. It also
 * exposes a simple API to send and get requests to the network and server it is
 * connected to.
 */
class DemobotNetwork {
    public:
        /**
         * Creates a new DemobotNetwork object and fills in the relevant network
         * and IP address information, if any.
         *
         * @param[in] demobotName String containing the demobot name.
         */
        explicit DemobotNetwork(const String demobotName);

        /**
         * Attempts to reset the network configuration. Useful for if we know a
         * network has dropped and we want to join/start a new one.
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

        /**
         * Checks to see if we're currently connected to a network.
         *
         * @return True if we're currently connected to a network, false otherwise.
         */
        bool isNetworkConnected() const;

        /**
         * Pings the configured ip address to see if we can get a response.
         * Useful for checking if we need to setup a server or not.
         *
         * @return Response code after sending a GET request to the server root
         *      endpoint ('/').
         */
        int pingServer() const;

        /**
         * Submits a GET request and looks for a response. Synchronous.
         *
         * @param[in] endpoint URL endpoint to send request to.
         * @param[in] keys Pointer to an array of strings that each contain a key.
         * @param[in] vals Pointer to an array of strings that each contain a value.
         * @param[in] argSize Number of key-value entries to go through.
         * @param[out] response Pointer to a string to fill up with the HTTP response.
         * @return HTTP response code. <0 are errors.
         */
        int sendGETRequest(const String endpoint, const String keys[], const String vals[], const int argSize, String *response) const;

        /**
         * Submits a POST request and looks for a response. Synchronous.
         *
         * @param[in] endpoint URL endpoint to send request to.
         * @param[in] keys Pointer to an array of strings that each contain a key.
         * @param[in] vals Pointer to an array of strings that each contain a value.
         * @param[in] argSize Number of key-value entries to go through.
         * @param[out] response Pointer to a string to fill up with the HTTP response.
         * @return HTTP response code. <0 are errors.
         */
        int sendPOSTRequest(const String endpoint, const String keys[], const String vals[], int argSize, String *response) const;

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
         * Frees relevant objects that were put on the heap.
         */
        void shutdownNetwork();

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

    private:
        /**
         * Quick and dirty way to hash a string for a switch case.
         * Reasonable assumption that the output is different for most strings.
         * Implementation details are inside.
         *
         * @param[in] valptr Pointer to a string to hash.
         * @return Hash value.
         */
        int hash(char* valptr) const;

        /**
         * Goes through the available networks and attempts to find one that
         * matches the internal credentials log. If we do find one, set the ssid
         * and password and return true.
         *
         * @param[out] ssid Char array pointer to the SSID.
         * @param[out] password Char array pointer to the password.
         * @return True if we found a matching network and set SSID and password,
         *      false elsewise.
         */
        bool getNetwork(char ssid[], char password[]) const;

    private:
        /** Robot name. */
        String _demobotName;

        /** IP address of server to connect to or host */
        IPAddress _ipaddress;

        /** Network info */
        char* _SSID;
        char* _PASSWORD;
        bool _startup;

        /** Boolean for whether the network is connected to or not. */
        bool _connected;

        /** HTTP client for sending and receiving requests. */
        HTTPClient *http;

        /**
         * Represents a single possible network that can be connected to or established.
         * Has a SSID and WPA2/PSK password associated with it.
         */
        struct Credential {
            const char* SSID;
            const char* PASSWORD;
        };

        /** Credentials log. */
        const int numCredentials = 4;
        const Credential *credentialsLog = nullptr;
};
