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
#include "WiFi.h"
#include <esp_now.h>

//WiFi Settings
//STA = connect to a WiFi network with name ssid
//AP = create a WiFi access point with  name ssid
#define WIFI_MODE "AP"
//const char * ssid = "esp_hotspot";
//const char * pass = "esp";
const char * ssid = "Cole";
const char * pass = "cole1234";

DancingServos* bot;

long serverDelayEnd = 0;
long serverCheckInterval = 1000;

WiFiServer wifiServer(80);

/* Data Transmission */
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  delay(500);

  /* Data Transmission Setup*/
  //Set device as a Wi-Fi Station AND Wi-Fi Access Point
  WiFi.mode(WIFI_AP_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


  //[hipL, hipR, ankleL, ankleR]
  bot = new DancingServos(14, 13, 12, 15);
  calibrateTrims(bot);
  bot->position0();

  Serial.println("Setting up WiFi...");
  setupWiFi(WIFI_MODE, ssid, pass);       //Access Point or Station
  setupWebServer(bot);                    //Set up the Web Server
  Serial.println("Finished setting up WiFi!");

  // pinMode(LED, OUTPUT);
  // digitalWrite(LED, LOW);

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
  //CW - decrease value, CCW - increase value
  //bot->setTrims(95, 90, 180, 60);
  //bot->setTrims(95, 90, 140, 130); //big dancebot

  bot->setTrims(170, 60, 25, 18); //small dancebot
}


