#ifndef WEBCONTROLLER
#define WEBCONTROLLER

#include "DancingServos.h"
#include "PowerController.h"
typedef struct struct_message {
  int id;
  int danceMove;
  bool batteryFlag;
  float batteryLevel;
  int status;
  char character[32]; 
} struct_message;

void printMACAddress();
int setupESPNOW(DancingServos* _dance_bot, PowerController* _power);
void setupWiFi(String mode, const char * _ssid, const char * _pass);
void setupWebServer(DancingServos* _bot);
void loopWebServer();
void handleRoot();
void handleReceivedDanceMove();
void handleDanceMove();

#endif
