/**
 * Author: Matthew Yu
 * Last Modified: 03/17/21
 * Project: Dancebot
 * File: DancebotESP32.ino
 * Description: Dance Dance Dance! https://youtu.be/ciEHtGpSviE
 * Organization: UT IEEE RAS
 */
#include <Arduino.h>
#include <DemobotNetwork.h>
#include <DemobotServer.h>
#include <DemobotClient.h>


/** Network instantiation. */
DemobotNetwork *network;
DemobotServer *server;
DemobotClient *client;

void onRoot(AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello, world");
}

void setup() {
    Serial.begin(115200);
    Serial.println("DancebotESP32.ino.");
    /* Give some time to open up the serial monitor. */
    delay(3000);

    /* Start up the network. */
    Serial.println("\nStarting network configuration.");
    network = new DemobotNetwork(DemobotNetwork::DANCEBOT_1);
    network->connectNetwork();
    delay(100);

    /* Check that the robot is connected to the network. */
    Serial.print("Is network connected [1=T|0=F]: ");
    Serial.println(network->isNetworkConnected());

    /* Set up the server. */
    Serial.println("\nStarting server setup.");
    server = new DemobotServer();

    /* Ping to see if another server is up. */
    IPAddress ip = network->getIPAddress();
    int response = server->pingServer(network->IpAddress2String(ip));
    if (response == 200) {
        /* Server is already set up. */
        Serial.println("Server is already set up on another bot.");
        server->~DemobotServer();
    } else {
        /* Set up our own server. */
        Serial.println("Server is not yet up; hosting the server...");

        /* TODO: Set up the following endpoints:
         * /
         * /robotJoin
         * /robotLeave
         * /robotUpdate
         *
         * /userMove
         * /userExpress
         * /userShutdown */
        server->addGETEndpoint(String("/"), onRoot);
        server->startServer();
    }

    /* Set up the robot client. */
    Serial.println("\nStarting client setup.");
    String url = String("http://" + network->IpAddress2String(ip) + ":80/");
    client = new DemobotClient(); /* Default port 80. */

    response = client->pingServer(url);
    if (response == 200) {
        Serial.println("Server is responsive. Robot is ready to start up!");
    } else {
        Serial.println("Server is not responsive....");
    }
}



void loop() {
    /* TODO: Loop robot state, loop robot movement. */

    /* TODO: If 200 ms has passed, send /robotUpdate request to the server. */

}
