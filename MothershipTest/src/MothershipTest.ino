#include <PS4Controller.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Wire.h>


// #include <Wifi.h> // for checking mac address stored on esp // PLEASE NOTE THAT YOU MAY HAVE TO WIPE ESP32 FLASH MEMORY IF BT DOESNT WORK

// #include <esp_wifi.h>


int LeftX, LeftY, RightX, RightY, AngleL;
double magnitude, CircleX, CircleY;

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);

void setup() {
  Serial.begin(115200);

// change MAC address of ESP32 to match that of the remote
  // uint8_t newMACAddress[] = {0x30, 0x83, 0x98, 0xD7, 0x33, 0xE0};
  // WiFi.mode(WIFI_STA);
  // esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  // Serial.print("[NEW] ESP32 Board MAC Address:  ");
  // Serial.println(WiFi.macAddress());

  PS4.begin();
  AFMS.begin();


  Serial.println("Ready!!!"); 
  // Serial.println(WiFi.macAddress());

    myMotor->setSpeed(100);
    myMotor->run(FORWARD);

}

void loop() {


  if (PS4.isConnected()) {

    // early debugging code:
    // Serial.println("connected!");
    // if (PS4.Square()) {
    //   digitalWrite(LED, HIGH);
    //   Serial.println("ON");
    // }
    // if (PS4.Square() == 0) {
    //   digitalWrite(LED, LOW);
    //   Serial.println("OFF!!!!!!");
    // }

    // for convenience of typing in following functions, might delete later?
    LeftX = PS4.LStickX();
    LeftY = PS4.LStickY();
    RightX = PS4.RStickX();
    RightY = PS4.RStickY();



    // deadzone box to mitigate drift
    if(abs(LeftX) < 10 && abs(LeftY) < 10) {
      LeftX = 0;
      LeftY = 0;
    }

    // get magnitude
    magnitude = sqrt(pow(LeftX, 2) + pow(LeftY, 2));
    if(magnitude > 125) { // cap at 125 to avoid mapping cartesian joystick coordinates to a circle (lol!)
      magnitude = 125;
    }
    magnitude = magnitude/125;
    Serial.print("  M: ");
    Serial.print(magnitude);



    // get angle from left stick
    Serial.print("  θ: ");
    AngleL = ((int)(atan2(LeftY, LeftX) * 180/PI) + 360) % 360; 
    Serial.print(AngleL);
    Serial.println("  ");

    // print X and Y of left stick
    // Serial.print("L: ");
    // Serial.print(LeftX);
    // Serial.print(",");    
    // Serial.println(LeftY);

    // print X and Y of right stick
    // Serial.print("  R: ");
    // Serial.print(RightX);
    // Serial.print(",");
    // Serial.print(RightY);
    // Serial.print("  ");




    // myMotor->setSpeed(magnitude*255);
    




    delay(200);
  }
}