/**
 * Author: Matthew Yu
 * Last Modified: 03/18/21
 * Project: Dancebot
 * File: DancebotWebController.h
 * Description: Defines and implements web API for servers and clients operating
 * on the Dancebot.
 * Organization: UT IEEE RAS
 */
#include <DemobotServer.h>
#include <DemobotClient.h>


/** Server handlers. */
void onRoot(AsyncWebServerRequest *request);

void onRobotJoin(AsyncWebServerRequest *request);

void onRobotUpdate(AsyncWebServerRequest *request);

void onRobotLeave(AsyncWebServerRequest *request);

void onUserMove(AsyncWebServerRequest *request);

void onUserExpress(AsyncWebServerRequest *request);

void onUserShutdown(AsyncWebServerRequest *request);

/** Client handlers. */
void onRobotJoinResponse(void *optParm, asyncHTTPrequest *request, int readyState);