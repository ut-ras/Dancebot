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
#define NO_SERVER 404
#define NO_RESPONSE -1
#define OK 200
#define RETRY_AMOUNT 3


// redirect any traffic with an unknown address to here
extern IPAddress gateway(192,168,1,1);
// 255-245-1 = 9 allowed IP addresses on the subnet
extern IPAddress subnet(255,255,0,0);

extern enum RequestType {
    GET,
    POST
};

/**
 * Definition of an implementation for the DemobotNetwork class.
 * 
 * The DemobotNetwork class manages Demobot networking code. It allows robots to
 * find and connect to networks and servers given just a demobot name. It also
 * exposes a simple API to send and get requests to the network and server it is
 * connected to.
 */
class DemobotNetwork {
    protected:
        String _demobotName;

        // IP address of server to connect to or host
        IPAdress _ipaddress;

        // Network info
        char* _SSID;
        char* _PASSWORD;
        bool _startup;

        // boolean for whether the network is connected to or not.
        bool _connected;

        /**
         * Represents a single possible network that can be connected to or established.
         * Has a SSID and WPA2/PSK password associated with it.
         */
        struct Credential {
            char* SSID;
            char* PASSWORD;
        };

        // static credentials log
        static const numCredentials = 4;
        static const Credential credentialsLog[numCredentials] = {
            {
                "Demobot",
                "Demobots1234"
            },{
                "DemobotsNetwork",
                "Dem0b0tsRu1e!"
            },{
                "",
                ""
            },{
                "",
                ""
            }
        };

        /**
         * quick and dirty way to hash a string for a switch case.
         * Reasonable assumption that the output is different for most strings.
         * 
         * @param demobotName (char*)
         *      pointer to a string to hash.
         * @return int
         *      hash value.
         */
        int hash(char* val);

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
        bool getNetwork(char* ssid, char* password);

    public:
        // default constructor. Should set up a network with no ip address and a
        // network config from the top of the credentials log.
        DemobotNetwork() { 
            DemobotNetwork(
                "Default"
            )
        }

        /**
         * creates a new DemobotNetwork object and fills in the relevant network
         * and IP address information, if any.
         * 
         * @param demobotName (char*)
         *      pointer to a char array containing the demobot name.
         */
        DemobotNetwork(char* demobotName);

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
         * @param response (char*)
         *      address of a char* to fill up with the HTTP response.
         * @return int
         *      http return code. <0 are errors.
         */
        int sendGETRequest(String endpoint, String[] *keys, String[] *vals, int argSize, char* response);

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
        int sendPOSTRequest(String endpoint, String[] *keys, String[] *vals, int argSize, char* response);
};

#endif
