#ifndef WEBCONTROLLER
#define WEBCONTROLLER

extern uint8_t broadcastAddress[];
//message struct that contains info that will be sent to clients
typedef struct struct_message {
  int integer;
  char character[32];
} struct_message;

void setupTransmission();
void setupWiFi(String mode, const char * _ssid, const char * _pass);
void setupWebServer(DancingServos* _bot);
void loopWebServer();

#endif
