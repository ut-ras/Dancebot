/**
 * Author: Matthew Yu
 * Last Modified: 03/18/21
 * Project: Dancebot
 * File: DancebotESP32.ino
 * Description: Dance Dance Dance! https://youtu.be/ciEHtGpSviE
 * Organization: UT IEEE RAS
 */
#include <Arduino.h>
#include <DemobotNetwork.h>
#include <DancebotWebController.h>
#include <Dancebot.h>


/** Network Declaration. */
DemobotNetwork *network;
DemobotServer *server;
DemobotClient *client;

/** Robot Declaration. */
Dancebot *dancebot;

int response;
String url;

void setup() {
    Serial.begin(115200);
    Serial.println("\nDancebotESP32.ino.");
    Serial.println("UT IEEE RAS (2021).");
    /* Give some time to open up the serial monitor. */
    delay(500);

    /* Set up robot. */
    Serial.println("\nRobot startup.");
    dancebot = new Dancebot(Dancebot::CARMEN);

    /* Start up the network. */
    Serial.println("\nStarting network configuration.");
    network = new DemobotNetwork(DemobotNetwork::DANCEBOT_1);
    network->connectNetwork();
    delay(100);

    /* Check that the robot is connected to the network. */
    Serial.print("Is network connected [T=1|F=0]: ");
    Serial.println(network->isNetworkConnected());

    /* Set up the server. */
    Serial.println("\nStarting server setup.");
    server = new DemobotServer();

    /* Ping to see if another server is up. */
    IPAddress ip = network->getIPAddress();
    response = server->pingServer(network->IpAddress2String(ip));
    if (response == 200) {
        /* Server is already set up. */
        Serial.println("Server is already set up on another bot.");
        server->~DemobotServer();
    } else {
        /* Set up our own server. */
        Serial.println("Server is not yet up; hosting the server...");

        /* Set up server endpoints. */
        server->addGETEndpoint(String("/"), onRoot);
        server->addGETEndpoint(String("/robotJoin"),    onRobotJoin);
        server->addGETEndpoint(String("/robotLeave"),   onRobotUpdate);
        server->addGETEndpoint(String("/robotUpdate"),  onRobotLeave);
        server->addGETEndpoint(String("/userMove"),     onUserMove);
        server->addGETEndpoint(String("/userExpress"),  onUserExpress);
        server->addGETEndpoint(String("/userShutdown"), onUserShutdown);
        server->startServer();
    }

    /* Set up the robot client. */
    Serial.println("\nStarting client setup.");
    url = String("http://" + network->IpAddress2String(ip) + ":80/");
    client = new DemobotClient(); /* Default port 80. */

    response = client->pingServer(url);
    if (response == 200) {
        Serial.println("Server is responsive. Robot is ready to start up!");
    } else {
        Serial.println("Server is not responsive. Shutting down.");
    }

    Serial.println("\nRobot is attempting to join the server.");
    int numArgs = 5;
    String keys[numArgs] = {
        String("ID"),
        String("CHARGE"),
        String("MOVEMENT"),
        String("EYE EXPRESSION"),
        String("EYE COLOR")
    };
    String vals[numArgs] = {
        dancebot->getID(),
        dancebot->getCharge(),
        dancebot->getMovement(),
        dancebot->getEyeExpression(),
        dancebot->getEyeColor()
    };
    client->sendGETRequest(url + "robotJoin", keys, vals, numArgs, onRobotJoinResponse);
}

void loop() {
    delay(100);
    /* TODO: Loop robot state, loop robot movement. */

    /* TODO: If 200 ms has passed, send /robotUpdate request to the server. */

}
