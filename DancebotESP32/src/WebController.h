/**
 * Author: Matthew Yu
 * Last Modified: 09/26/20
 * Project: Dancebot Swarm
 * File: WebController.h
 * Description: A generic secure web controller for controlling various Demobots Projects. 
 *      Robots should be able to: connect to a remote server and receive/send commands, as 
 *      well as spin up their own server and serve a webpage to the user to directly 
 *      interact with it.
 * Organization: UT IEEE RAS
 */
#pragma once

// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
#include "RobotConfig.h"
#include "DemobotNetwork.h"

#define NO_SERVER 404
#define NO_RESPONSE -1
#define OK 200

/* -----------------------------SETUP------------------------------- */

/**
 * Sets up a network and a server (if either are necessary) on the robot.
 * The end result should be either a connected robot or a failure in which the
 * program can use to try again at another point.
 *
 * @param[in] network Reference to the DancebotNetwork.
 * @return bool
 *      true on success, false on some failure.
 */
bool setupNetworking(DemobotNetwork &network);

/**
 * Sets up an access point network for the robot and other robots to connect to.
 * Fails in the event of an error.
 * 
 * @param[in] network Reference to the DancebotNetwork.
 * @return True on success, false on some failure.
 */
bool setupAPNetwork(DemobotNetwork &network);

/**
 * Sets up a server for the robot and other robots to connect to. Fails in the 
 * event of an error.
 * 
 * @param[in] network Reference to the DancebotNetwork.
 * @return True on success, false on some failure.
 */
bool setupServer(DemobotNetwork &network);

/* ------------------------CLIENT REQUESTS-------------------------- */

/**
 * Sends a GET request with the robot id to IP/robotJoin to add itself to the server. 
 * Looks for a 200 HTTP code response to signal acceptance.
 * 
 * @param[in] network Reference to the DancebotNetwork.
 * @param[in] dancebot Reference to the Dancebot object.
 * @return Response header from robotJoin request. Empty string upon failure.
 */
String joinServer(DemobotNetwork &network, const Dancebot &dancebot);

/**
 * Sends a GET request with the robot id to IP/robotUpdate to retrieve the
 * new operating state. Looks for a 200 HTTP code response to signal success.
 * 
 * @param[in] network Reference to the DancebotNetwork.
 * @param[in] dancebot Reference to the Dancebot object.
 * @return Response header from robotUpdate request. Empty string upon failure.
 */
String getState(DemobotNetwork &network, const Dancebot &dancebot);

/**
 * Sends a GET request with the robot id to IP/robotLeave to disconnect from 
 * the server. Looks for a 200 HTTP code response to signal success.
 * 
 * @param[in] network Reference to the DancebotNetwork.
 * @param[in] dancebot Reference to the Dancebot object.
 * @return Response header from robotLeave request. Empty string upon failure.
 */
String leaveServer(DemobotNetwork &network, const Dancebot &dancebot);

/* ---------------------SERVER HANDLER REQUESTS--------------------- */

/**
 * Starts a server hosted by the Dancebot. Experimental.
 */
void startServer();