#ifndef WEBCONTROLLER
#define WEBCONTROLLER

void setupESPNOW(DancingServos* _bot);
void setupWiFi(String mode, const char * _ssid, const char * _pass);
void setupWebServer(DancingServos* _bot);
void loopWebServer();
void handleDanceMove();
void connect_to_server(void);
void requestMainDancebot(void);

#endif
