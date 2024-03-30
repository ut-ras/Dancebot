#include <PS4Controller.h> 
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Wire.h> //i2c library

// TODO: FIX LEFT JOYSTICK INPUT ~340-45 degrees broken
// TODO: LATCHING MODES WITH BUTTON
// TODO: OMNIDIRECTIONAL DRIVE

/*
ESP32 -> Driver:
PLEASE NOTE THAT ESP32FLASH MEMORY MAY NEED TO BE WIPED IF BLUETOOTH DOES NOT CONNECT (see https://randomnerdtutorials.com/esp32-erase-flash-memory/)
- Original motor drivers broke (Region 5 Competition drivers), so we are using Adafruit Motor Shield V2.3 connected to ESP32 Wroom (Pico-D4 not compatible with PS4 library)
- Wire I2C ESP32 pins to respective SDA and SCL pins on driver, ground to ground, and 5v to 5v (4 connections)

ESP32   |   Driver
  SCL   |    SCL (GPIO22)
  SDA   |    SDA (GPIO21)
  5v    |    5v
  GND   |    GND

- PS4 remote MUST be configured to search for MAC address of ESP32
- Use "Sixaxis Pair Tool" to edit this MAC address the remote searches for
- Specific MAC address pairs that worked for me (notice how ESP32's last octet is 2 less than controller's):
    ESP32: c8:f0:9e:f1:da:28
    Remote: c8:f0:9e:f1:da:2a
- Look into WiFi libraries to print and edit ESP32 MAC address if necessary :D


Driver -> Motors:
- only motor wires connected are RED and BLACK
- when looking at motor driver with long set of header pins and protoboard holes at the top, 
  the RED wires are always on top of the BLACK wires

 !!!RED and BLACK wires for M2 and M4 are swapped!!!

        FRONT
  -------------------                
O |  M3         M2  |O
  |                 |
  |                 |
O |  M4         M1  |O
   -------------------   
        BACK

Motor pinout:
- we are only using RED and BLACK

white - ENC BOUT
yellow - ENC AOUT
blue - ENC VCC
green - ENC GND
black - PWR B
red - PWR A

*/

int LeftX, LeftY, RightX, RightY, FRBL, FLBR;
float Lmagnitude, Rmagnitude, Langle, Rangle;

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *BackRight = AFMS.getMotor(1);
Adafruit_DCMotor *FrontRight = AFMS.getMotor(2);
Adafruit_DCMotor *FrontLeft= AFMS.getMotor(3);
Adafruit_DCMotor *BackLeft = AFMS.getMotor(4);

void setup() {
  PS4.begin();
  AFMS.begin();
  Serial.begin(115200);
  Serial.println("Ready for controller connection!!"); 

  // init all motors to drive forward
  BackRight->run(FORWARD);
  FrontRight->run(FORWARD);
  FrontLeft->run(FORWARD);
  BackLeft->run(FORWARD);
}

void loop() {
  if (PS4.isConnected()) {

    // aliases for ease of typing
    LeftX = PS4.LStickX();
    LeftY = PS4.LStickY();
    RightX = PS4.RStickX();
    RightY = PS4.RStickY();

    // deadzone box to mitigate drift effects
    if(abs(LeftX) < 20 && abs(LeftY) < 20) {
      LeftX = 0;
      LeftY = 0;
    }
    if(abs(RightX) < 20 && abs(RightY) < 20) {
      RightX = 0;
      RightY = 0;
    }
    
    // get joystick data
    Lmagnitude = getMagnitude(LeftX, LeftY);
    Rmagnitude = getMagnitude(RightX, RightY);
    Langle = getAngle(LeftX, RightY);
    Rangle = getAngle(RightX, RightY);

    // print joystick data
    // Serial.print("LM: " );
    // Serial.print(Lmagnitude);
    // Serial.print("  Lθ: ");
    // Serial.print(Langle);
    Serial.print(" RM: ");
    Serial.print(Rmagnitude);
    Serial.print("  Rθ: ");
    Serial.print(Rangle);
    // Serial.print(" Left ");
    // Serial.print(LeftSpeed);
    // Serial.print(" speed ");
    // Serial.print(RightSpeed);
    Serial.print(" FRBL: ");
    Serial.print(FRBL);
    Serial.print(" FLBR: ");
    Serial.print(FLBR);
    Serial.println();


    //  FRBL: sin(angle−1/4π) * magnitude
    //  FLBR: sin(angle+1/4π) * magnitude 
  
    // positive FRBL or FLBR goes BACKWARD!!
    // FRBL = ((255*Rmagnitude) * sin(Rangle-(0.25*PI)));
    // FLBR = -((255*Rmagnitude) * sin(Rangle+(0.25*PI)));

    FRBL = (255*Rmagnitude) * (sin(Rangle-(0.25*PI)));
    FLBR = (255*Rmagnitude) * (sin(Rangle+(0.25*PI)));

    if (FRBL < 0){
      FrontRight->run(FORWARD);
      BackLeft->run(FORWARD);
    }
    else{
      FrontRight->run(BACKWARD);
      BackLeft->run(BACKWARD);
    }

    if (FLBR < 0) {
      FrontLeft->run(FORWARD);
      BackRight->run(FORWARD);
    }
    else{
      FrontLeft->run(BACKWARD);
      BackRight->run(BACKWARD);
    }
    
    FrontLeft->setSpeed(FLBR);
    BackRight->setSpeed(FLBR);

    FrontRight->setSpeed(FRBL);
    BackLeft->setSpeed(FRBL);

    // delay(200);
  }
}


// returns joystick angle in radians with +x axis as 0. increases counterclockwise
float getAngle(int X, int Y) {
  float res = (atan2(Y, X));
  if(res < 0) {
    res += 6.28; // account for switch to negative angles after pi radians
  }
  return res;
}


// returns magnitude of joystick position from 0 to 1
float getMagnitude(int X, int Y) {
  float result = sqrt(pow(X, 2) + pow(Y, 2));
  if(result > 125) { // cap at 125 to avoid mapping cartesian joystick coords to circle for simplicity
    result = 125;
  }
  return result/125;
}