#ifndef WEBCONTROLLER
#define WEBCONTROLLER

void setupWiFi(String mode, const char * _ssid, const char * _pass);
void setupWebServer(DancingServos* _bot);
void loopWebServer();

#endif
