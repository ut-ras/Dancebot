/**
 * File: DemobotClient.h
 * Author: Matthew Yu
 * Last Modified: 03/16/21
 * Project: Demobots General
 * Organization: UT IEEE RAS
 * Description: Implements definitions for the DemobotClient class, which
 * allows the ESP32 to send requests to a DemobotServer.
 */
#pragma once

#include <HTTPClient.h>

class DemobotClient {
    /**
     * The DemobotClient class allows the Demobot to send HTTP requests to
     * DemobotServers.
     */
    public:
        /** Creates a new DemobotClient at port 80. */
        DemobotClient();

        /**
         * Creates a new DemobotClient at the specified port.
         * @param port Port of new client.
         */
        DemobotClient(const uint8_t port);

        /**
         *
         */
        bool connectServer() const;

        /**
         *
         */
        bool disconnectServer() const;

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
    private:
        WebServer *_server;

        /** HTTP client for sending and receiving requests. */
        HTTPClient *_http;
}