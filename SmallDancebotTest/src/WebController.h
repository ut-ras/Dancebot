#ifndef WEBCONTROLLER
#define WEBCONTROLLER

typedef struct struct_message {
  int id;
  int danceMove;
  float batteryLevel;
  char character[32];
} struct_message;

void printMACAddress();
void setupESPNOW(DancingServos* _bot);
void setupWiFi(String mode, const char * _ssid, const char * _pass);
void setupWebServer(DancingServos* _bot);
void loopWebServer();
void handleDanceMove();

#endif
