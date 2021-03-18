/**
 * Author: Matthew Yu
 * Last Modified: 03/18/21
 * Project: Dancebot
 * File: DancebotWebController.cpp
 * Description: Defines and implements web API for servers and clients operating
 * on the Dancebot.
 * Organization: UT IEEE RAS
 */
#include <DancebotWebController.h>


/** Server handlers. */
void onRoot(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request to serve the webpage.");
    request->send(200, "text/plain", "Hello, world");
}

void onRobotJoin(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request for robot to join the server.");
    request->send(200, "text/plain", "Hello, world");
}

void onRobotUpdate(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request for robot to get an update from the server.");
    request->send(200, "text/plain", "Hello, world");
}

void onRobotLeave(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request for robot to leave the server.");
    request->send(200, "text/plain", "Hello, world");
}

void onUserMove(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request for user to update a robot movement on the server.");
    request->send(200, "text/plain", "Hello, world");
}

void onUserExpress(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request for user to update a robot expression on the server.");
    request->send(200, "text/plain", "Hello, world");
}

void onUserShutdown(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request for user to update a robot power state on the server.");
    request->send(200, "text/plain", "Hello, world");
}

/** Client handlers. */
void onRobotJoinResponse(void *optParm, asyncHTTPrequest *request, int readyState) {
    if (readyState == 4) {
        if (request->responseHTTPcode() == 200) {
            Serial.println("[CLIENT] Robot joined the server.");
        } else {
            Serial.println("[CLIENT] Robot did not join the server.");
        }
    }
}
