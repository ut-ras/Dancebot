#ifndef WEBCONTROLLER
#define WEBCONTROLLER

#include "WebController.h"
#include "DemobotNetwork.h"
#include "RobotConfig.h"

void setupWiFi(String mode, const char * _ssid, const char * _pass);
void setupWebServer(DancingServos* _bot);
void loopWebServer();
String getState(DemobotNetwork &network, const Dancebot &dancebot);

#endif
