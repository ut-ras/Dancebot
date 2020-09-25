/**
 * File: DemobotNetwork.h
 * Author: Matthew Yu
 * Last Modified: 09/20/20
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
#define RETRY_AMOUNT 3

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
         * creates a new DemobotNetwork object and fills in the relevant network
         * and IP address information, if any.
         * 
         * @param demobotName (string)
         *      pointer to a string containing the demobot name.
         */
        explicit DemobotNetwork(String demobotName);

        /**
         * attempts to reset the network configuration. Useful for if we know a
         * network has dropped and we want to join/start a new one.
         */
        void reconfigureNetworks();

        /**
         * attempts to connect to the network selected during object
         * instantiation. Fails if we could not find a network during object
         * instantiation or we can't connect to the network.
         * 
         * @return bool
         *      Whether or not we have connected to a network or not.
         */
        bool connectNetwork();

        /**
         * checks to see if we're currently connected to a network.
         * 
         * @return bool
         *      whether or not we have connected to a network or not.
         */
        bool isNetworkConnected();

        /**
         * pings the configured ip address to see if we can get a response.
         * Useful for checking if we need to setup a server or not.
         * 
         * @return bool
         *      whether or not a server at the given IP exists.
         */
        bool pingServer();

        /**
         * submits a GET request and looks for a response. Synchronous.
         * 
         * @param endpoint   (string)
         *      url endpoint to send request to.
         * @param keys  (string[] *)
         *      array of strings that each contain a key.
         * @param vals  (string[] *)
         *      array of strings that each contain a value.
         * @param argSize (int)
         *      number of key-value entries to go through.
         * @param response (string*)
         *      address of a char* to fill up with the HTTP response.
         * @return int
         *      http return code. <0 are errors.
         */
        int sendGETRequest(String endpoint, String keys[], String vals[], int argSize, String *response);

        /**
         * submits a POST request and looks for a response. Synchronous.
         * 
         * @param endpoint   (string)
         *      url endpoint to send request to.
         * @param keys  (string[] *)
         *      array of strings that each contain a key.
         * @param vals  (string[] *)
         *      array of strings that each contain a value.
         * @param argSize (int)
         *      number of key-value entries to go through.
         * @return int
         *      http return code. <0 are errors.
         */
        int sendPOSTRequest(String endpoint, String keys[], String vals[], int argSize);

        /**
         * returns the network SSID.
         * 
         * @return char*
         *      network SSID. May be nullptr if configured incorrectly.
         */
        char* getNetworkSSID();

        /**
         * returns the network password.
         * 
         * @return char*
         *      network password. May be nullptr if configured incorrectly.
         */
        char* getNetworkPassword();

        void shutdownNetwork();

        /**
         * returns the relevant server IP address.
         * 
         * @return IPAddress
         *      server IPAddress that corresponds to Demobot name.
         */
        IPAddress getIPAddress();

        /**
         * helper function to convert an IPAddress to a string.
         * @author: apicquot from https://forum.arduino.cc/index.php?topic=228884.0
         * 
         * @param const IPAddress&
         *      reference to an IPAddress object.
         * @return String
         *      string output.
         */
        String IpAddress2String(const IPAddress ipAddress);

    private:
        /**
         * quick and dirty way to hash a string for a switch case.
         * Reasonable assumption that the output is different for most strings.
         * 
         * @param demobotName (char*)
         *      pointer to a string to hash.
         * @return int
         *      hash value.
         */
        int hash(char* valptr);

        /**
         * goes through the available networks and attempts to find one that
         * matches the internal credentials log. If we do find one, set the ssid
         * and password and return true.
         * 
         * @param ssid  (char*)
         *      char array pointer to the SSID.
         * @param password (char*)
         *      char array pointer to the password.
         * @return bool
         *      true if we found a matching network and set SSID and password,
         *      false elsewise.
         */
        bool getNetwork(char ssid[], char password[]);

    private:
        String _demobotName;

        // IP address of server to connect to or host
        IPAddress _ipaddress;

        // Network info
        char* _SSID;
        char* _PASSWORD;
        bool _startup;

        // boolean for whether the network is connected to or not.
        bool _connected;

        // http client for sending and receiving requests.
        HTTPClient *http;
        /**
         * Represents a single possible network that can be connected to or established.
         * Has a SSID and WPA2/PSK password associated with it.
         */
        struct Credential {
            const char* SSID;
            const char* PASSWORD;
        };

        // credentials log
        const int numCredentials = 4;
        const Credential *credentialsLog = nullptr;
};