//ESP32 devkitC

 
#include <Arduino.h>
#include "DancingServos.h"
// #include "WebController.h"
// #include "PowerController.h"
// #include "WiFi.h"
// #include "esp_now.h"


//WiFi Settings
//STA = connect to a WiFi network with name ssid
//AP = create a WiFi access point with  name ssid
// #define WIFI_MODE "AP"
// //const char * ssid = "esp_hotspot";
// //const char * pass = "esp";
// const char * ssid = "Cole";
// const char * pass = "cole1234";


DancingServos* bot;
// PowerController* powerControl;

// long serverDelayEnd = 0;
// long serverCheckInterval = 1000;

//hat servo (only for main dancebot rn)
Servo hat;
int pos = 0;

void setup() {
  Serial.begin(115200);
  delay(500);

  // printMACAddress();
  // Serial.println("I started setting up!");

  //[hipL, hipR, ankleL, ankleR]
  bot = new DancingServos(21, 23, 19, 22);
  calibrateTrims(bot);
  bot->position0();

  delay(1000);
  bot->hop(25, -1);

  // Serial.println("Starting Power");
  // powerControl = new PowerController();
  // powerControl->batteryADCInit();

  // Serial.println("Starting ESPNOW");
  // if(!setupESPNOW(bot, powerControl)){
  //   Serial.println("Failed ESPNOW init...");
  // }

  //only hat code
  // hat.attach(26);
  // hat.write(0);
}



void loop() {  
  //loop the motors and check for web server traffic
  bot->loopOscillation();

  //check if ready to start next move in dance
  bot->loopDanceRoutines();
}



//manual calibration- based on how the servos are attatched to the 3d printed parts
void calibrateTrims(DancingServos* bot) {
  //CW - decrease value, CCW - increase value
  bot->setTrims(95, 90, 180, 60);
  //  bot->setTrims(95, 90, 140, 130); //big dancebot

  // dev notes: new trims because the screw is stripped

  // dev notes: front-end trim adjustment and save to flash memory

  // dev notes: bot 1
  // bot->setTrims(165, 100, 160, 20);

  // dev notes: bot 2
  //bot->setTrims(107, 75, 135, 132); 

  // dev notes: bot 3
  // bot->setTrims(165, 100, 25, 40);

  //bot->setTrims(95, 90, 130, 120); //big dancebot
  //bot->setTrims(170, 60, 25, 18); //small dancebot
}


