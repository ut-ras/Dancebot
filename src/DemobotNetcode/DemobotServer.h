/**
 * File: DemobotServer.h
 * Author: Matthew Yu
 * Last Modified: 03/16/21
 * Project: Demobots General
 * Organization: UT IEEE RAS
 * Description: Implements definitions for the DemobotServer class, which
 * allows the ESP32 to set up and manage a web server.
 */
#pragma once

#include <ESPAsyncWebServer.h>


typedef void (*handlerFunctionPtr_t)(void);

class DemobotServer {
    /**
     * The DemobotServer class allows the Demobot to set up a web server and
     * manage HTTP requests by clients.
     */
    public:
        /** Creates a new DemobotServer at port 80. */
        DemobotServer();

        /**
         * Creates a new DemobotServer at the specified port.
         * @param port Port of new server.
         */
        DemobotServer(const unsigned int port);

        /**
         * Pings the configured ip address to see if we can get a response.
         * Useful for checking if we need to setup a server or not.
         *
         * @return Response code after sending a GET request to the server root
         *      endpoint ('/').
         */
        int pingServer() const;

        /**
         * Adds an endpoint, if it does not already exist, for responding to
         * client GET requests.
         *
         * @param[in] endpoint URL request endpoint.
         * @param[in] handler User defined function pointer that uses
         *                    AsyncWebServerRequest to handle the data.
         * @return True if endpoint was set up. False otherwise.
         */
        bool addGETEndpoint(const String endpoint, const handlerFunctionPtr_t handler);

        /**
         * Adds an endpoint, if it does not already exist, for responding to
         * client POST requests.
         *
         * @param[in] endpoint URL request endpoint.
         * @param[in] handler User defined function pointer that uses
         *                    AsyncWebServerRequest to handle the data.
         * @return True if endpoint was set up. False otherwise.
         */
        bool addPOSTEndpoint(const String endpoint, const handlerFunctionPtr_t handler);

        ~DemobotServer();
    private:
        /** Port for webserver to open on. */
        int port;

        /** Server object to manage. */
        AsyncWebServer *_server;
}