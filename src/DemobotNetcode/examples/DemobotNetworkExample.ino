/**
 * Author: Matthew Yu
 * Last Modified: 03/16/21
 * Project: Dancebot
 * File: DemobotNetworkExample.ino
 * Description: Test .ino for Demobot components.
 * Organization: UT IEEE RAS
 */
#include <Arduino.h>
#include <DemobotNetwork.h>


/** Network instantiation */
DemobotNetwork *network;


void setup() {
    Serial.begin(115200);
    Serial.println("DemobotNetworkExample.ino.");
    /* Give some time to open up the serial monitor. */
    delay(5000); // 5 seconds
    Serial.println("Starting network configuration.");
    network = new DemobotNetwork(DemobotNetwork::DANCEBOT_1);
    network->connectNetwork();

    delay(3000);
    for (uint8_t i = 0; i < 5; i++) {
        Serial.println(network->isNetworkConnected());
        delay(1000);
    }
    Serial.println(network->getNetworkSSID());
    Serial.println(network->getNetworkPassword());
    Serial.println(network->IpAddress2String(network->getIPAddress()));

    network->disconnectNetwork();
    delay(1000);
    Serial.println(network->isNetworkConnected());
    network->~DemobotNetwork();
    Serial.println(network->isNetworkConnected());
}

void loop() {}
