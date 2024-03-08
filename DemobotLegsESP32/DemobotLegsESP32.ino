//ESP32 PICO Kit dev board
/* Current Pins
 * hip  L: 14
 * hip R: 13
 * ankle L: 12 
 * ankle R: 15
 */

/*
 * Hardware
 * 
 * Servos
 *    "Ankles": 2 HiTec HS-625MG "Super Torque" Servos https://www.horizonhobby.com/pdf/HRC32625S-Spec_Sheet.pdf
 *      Operating Voltage: 4.8-6.0 V
 *      Current Draw: 8.8 mA idle, 400 mA running
 *    "Hips": 2 HiTec HS-322HD "Deluxe" Servos https://www.robotshop.com/media/files/pdf/hitec-hs-322hd-servo-specsheet.pdf
 *      Operating Voltage: 4.8-6.0 V
 *      Current Draw: 7.4 mA idle, 160 mA running no load, 700 mA running stalled
 *    Total Current Draw 4 Servos: ~1600mA when running
 *    Both have 900-2100 us pulse width (see Oscillator.cpp attach function)
 * Microcontroller
 *    ESP32 Pico Kit https://www.espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_en.pdf
 *    Operating Voltage: There is a 5v pin
 *    Current Draw: 80mA average
 * Batteries
 *    Power Source: 6V 8000 mAH
 *    4 rechargeable AA (1.5V), 2000 mAH each
 *    8000 mAH battery using 1700mA lasts about 4.7 hours
 * Decoupling Capacitor
 *    100uF capacitor across the power source. I'm not sure if this is the best size, I haven't measured the noise yet.
 *    Decoupling capacitors help remove AC noise from a DC power source.
 *    The motors turning on/off and running can create noise in the power source; look at the differnce in current draw for idle vs running.
 *    The ESP32 will reset if the power supply is not stable. The Teensy didn't crash without the cap, but it doesn't hurt. 
 *    It would probably be better to use a 5v regulator.
 */

 
#include <Arduino.h>
#include "DancingServos.h"
#include "WebController.h"


//WiFi Settings
//STA = connect to a WiFi network with name ssid
//AP = create a WiFi access point with  name ssid
#define WIFI_MODE "STA"
//const char * ssid = "esp_hotspot";
//const char * pass = "esp";
const char * ssid = "Cole";
const char * pass = "cole1234";


DancingServos* bot;

long serverDelayEnd = 0;
long serverCheckInterval = 1000;


void setup() {
  Serial.begin(115200);
  delay(500);
  
  //[hipL, hipR, ankleL, ankleR]
  bot = new DancingServos(14, 13, 12, 15);
  calibrateTrims(bot);

  setupWiFi(WIFI_MODE, ssid, pass);       //Access Point or Station
  setupWebServer(bot);                    //Set up the Web Server

  delay(500);
  bot->position0();
}



void loop() {  
  //loop the motors and check for web server traffic
  bot->loopOscillation();

  //check if ready to start next move in dance
  bot->loopDanceRoutines();
  
  if (!bot->isOscillating() || millis() > serverDelayEnd) {
    serverDelayEnd = millis() + serverCheckInterval;
    loopWebServer();
  }
}



//manual calibration- based on how the servos are attatched to the 3d printed parts
void calibrateTrims(DancingServos* bot) {
  //[hipL, hipR, ankleL, ankleR]
  bot->setTrims(70, 150, 25, 18);
}

