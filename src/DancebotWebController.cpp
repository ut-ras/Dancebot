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
    int headers = request->headers();

    int i;
    for(i=0;i<headers;i++){
        AsyncWebHeader* h = request->getHeader(i);
        Serial.printf("HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    Serial.println("Robot config: ");
    if (request->hasParam("ID")) {
        Serial.print("\t-ID: ");
        Serial.println(request->getParam("ID")->value());
    }
    if (request->hasParam("CHARGE")) {
        Serial.print("\t-CHARGE: ");
        Serial.println(request->getParam("CHARGE")->value());
    }
    if (request->hasParam("MOVE")) {
        Serial.print("\t-MOVEMENT: ");
        Serial.println(request->getParam("MOVE")->value());
    }
    if (request->hasParam("EE")) {
        Serial.print("\t-EYE EXPRESSION: ");
        Serial.println(request->getParam("EE")->value());
    }
    if (request->hasParam("EC")) {
        Serial.print("\t-EYE COLOR: ");
        Serial.println(request->getParam("EC")->value());
    }

    request->send(200, "text/plain", "Success,");
}

void onRobotUpdate(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request for robot to get an update from the server.");
    request->send(200, "text/plain", "Hello, world"); /* TODO: return params here. */
}

void onRobotLeave(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request for robot to leave the server.");
    request->send(200, "text/plain", "Success.");
}

/** Add error responses if the input doesn't make sense or the robot id doesn't exist. */
void onUserMove(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request for user to update a robot movement on the server.");
    request->send(200, "text/plain", "Success.");
}

void onUserExpress(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request for user to update a robot expression on the server.");
    request->send(200, "text/plain", "Success.");
}

void onUserShutdown(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request for user to update a robot power state on the server.");
    request->send(200, "text/plain", "Success.");
}

/** Client handlers. */
void onRobotJoinResponse(void *optParm, asyncHTTPrequest *request, int readyState) {
    if (readyState == 4) {
        if (request->responseHTTPcode() == 200) {
            Serial.println("[CLIENT] Robot joined the server.");
        } else {
            Serial.println("[CLIENT] Received response ");
            Serial.print(request->responseHTTPcode());
            Serial.println(".");
        }
    }
}

void onRobotUpdateResponse(void *optParm, asyncHTTPrequest *request, int readyState) {
    if (readyState == 4) {
        if (request->responseHTTPcode() == 200) {
            Serial.println("[CLIENT] Robot updated parameters from the server.");
            /* TODO: call parse and update from global robot object? */
        } else {
            Serial.println("[CLIENT] Received response ");
            Serial.print(request->responseHTTPcode());
            Serial.println(".");
        }
    }
}

void onRobotLeaveResponse(void *optParm, asyncHTTPrequest *request, int readyState) {
    if (readyState == 4) {
        if (request->responseHTTPcode() == 200) {
            Serial.println("[CLIENT] Robot left the server.");
            /* TODO: call shutdown from global robot object? */
        } else {
            Serial.println("[CLIENT] Received response ");
            Serial.print(request->responseHTTPcode());
            Serial.println(".");
        }
    }
}
