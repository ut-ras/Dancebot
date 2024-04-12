#include <PS4Controller.h> 
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Wire.h> //i2c library

#include <WiFi.h>


/*

This documentation covers how to connect the PS4 controller to the ESP32, the ESP32 to the motor driver, and the driver to the moters.
// PLEASE NOTE THAT HARDWARE ON LEFT JOYSTICK FOR REMOTE WAS BROKEN

ESP32 -> Driver:
- Original motor drivers broke (Region 5 Competition drivers), so we are using Adafruit Motor Shield V2.3 connected to ESP32 Wroom (Pico-D4 not compatible with PS4 library)
- Wire I2C ESP32 pins to respective SDA and SCL pins on driver, ground to ground, and 5v to 5v (4 connections)

ESP32   |   Driver
  SCL   |    SCL (GPIO22)
  SDA   |    SDA (GPIO21)
  5v    |    5v
  GND   |    GND

Controller Bluetooth (BT) -> ESP32:
PLEASE NOTE THAT ESP32FLASH MEMORY MAY NEED TO BE WIPED IF BLUETOOTH DOES NOT CONNECT (see https://randomnerdtutorials.com/esp32-erase-flash-memory/)
- It seems you need to wipe the ESP32 flash if you connect the remote to another device (i.e. laptop) because something something BT client keys gone (it just works)
- PS4 remote MUST be configured to search for MAC address of ESP32 through external application "Sixaxis Pair Tool"
- Put ESP32 address in PS4.begin() 
  - i.e. 
- Specific MAC address pairs that worked for me (notice how ESP32's last octet is 2 less than the remote's):
  - ESP32 address the one printed through WiFi library
  - Remote address is one in SixAxis Pair Tool
    |  ESP32:  c8:f0:9e:f1:da:28
    |  Remote: c8:f0:9e:f1:da:2a
    |
    |  ESP32:  c8:f0:9e:f1:da:24
    |  Remote: c8:f0:9e:f1:da:26
- Look into WiFi libraries to print and edit (?) ESP32 MAC address if necessary :D


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
   ------------------
        BACK

Motor pinout:
- we are only using RED and BLACK because we don't need precise motion

white - ENC BOUT
yellow - ENC AOUT
blue - ENC VCC
green - ENC GND
BLACK - PWR B
RED - PWR A

*/

int LeftX, LeftY, RightX, RightY, FRBL, FLBR, Mode;
float Lmagnitude, Rmagnitude, Langle, Rangle;

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *BackRight = AFMS.getMotor(1);
Adafruit_DCMotor *FrontRight = AFMS.getMotor(2);
Adafruit_DCMotor *FrontLeft= AFMS.getMotor(3);
Adafruit_DCMotor *BackLeft = AFMS.getMotor(4);

void setup() {
  PS4.begin(); // leave blank
  delay(100);
  AFMS.begin();
  delay(100);
  Serial.begin(115200);
  delay(100);
  Serial.println(WiFi.macAddress()); // print "MAC address" of ESP32

  Serial.println("Ready for controller connection!!"); 

  // init all motors to drive forward
  BackRight->run(FORWARD);
  FrontRight->run(FORWARD);
  FrontLeft->run(FORWARD);
  BackLeft->run(FORWARD);
}

void loop() {
  if (PS4.isConnected()) {
    Mode = 0;
    getData();
    printData();

    //  FRBL: sin(angle−1/4π) * magnitude
    //  FLBR: sin(angle+1/4π) * magnitude 
  
    // positive FRBL or FLBR goes BACKWARD!!
    // FRBL = ((255*Rmagnitude) * sin(Rangle-(0.25*PI)));
    // FLBR = -((255*Rmagnitude) * sin(Rangle+(0.25*PI)));
    //^ prob wont need the negative sign on FLBR because orientation already flipped through hardware

    FRBL = (100*Rmagnitude) * (sin(Rangle-(0.25*PI)));
    FLBR = (100*Rmagnitude) * (sin(Rangle+(0.25*PI)));

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

  // latch to normal drive on right joystick (partition joystick into 4 regions: rotate clockwise, forward, rotate counterclockwose, backward)
    if(PS4.Square()) {
      int FRBRFLBL;
      while(1) { 
        Mode = 1;
        getData();
        // Serial.print(Mode);

        printData();
        
        // calculate speed as function as magnitude of joystick
        FRBRFLBL = (Rmagnitude * 100);
        
        FrontRight->setSpeed(FRBRFLBL);
        BackRight->setSpeed(FRBRFLBL);
        FrontLeft->setSpeed(FRBRFLBL);
        BackLeft->setSpeed(FRBRFLBL);
        
        if(PS4.Triangle()) {
          break;
        }
        // rotate clockwise
        if((5.497 <= Rangle) && (Rangle > 0) || (Rangle <= 0.785)) {
          FrontLeft->run(BACKWARD);
          BackLeft->run(BACKWARD);
          FrontRight->run(FORWARD);
          BackRight->run(FORWARD);
          Serial.println("COUNTERCLOCKWISE");
          continue;
        }
        // forward
        else if((0.785 < Rangle) && (Rangle < 2.356)) {
          FrontLeft->run(FORWARD);
          BackLeft->run(FORWARD);
          FrontRight->run(FORWARD);
          BackRight->run(FORWARD);
          Serial.println("FORWARD");
          continue;
        }

        // backward
        else if((3.926 < Rangle) && (Rangle < 5.497)) {
          FrontLeft->run(BACKWARD);
          BackLeft->run(BACKWARD);
          FrontRight->run(BACKWARD);
          BackRight->run(BACKWARD);
          Serial.println("BACKWARD");
          continue;
        }
                // rotate counterclockwise
        else if(((2.356 <= Rangle) && (Rangle <= 3.926))) { // 
          FrontLeft->run(FORWARD);
          BackLeft->run(FORWARD);
          FrontRight->run(BACKWARD);
          BackRight->run(BACKWARD);
          Serial.println("CLOCKWISE");
          continue;
        }



      }
    }
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

void printData(void) {
    // print controller data
    Serial.print("Mode: ");
    Serial.print(Mode);
    Serial.print(" Button: ");
    Serial.print(PS4.Square());
    // Serial.print(" LM: " );
    // Serial.print(Lmagnitude);
    // Serial.print(" Lθ: ");
    // Serial.print(Langle);
    Serial.print(" RM: ");
    Serial.print(Rmagnitude);
    Serial.print("  Rθ: ");
    Serial.print(Rangle);
    // Serial.print(" FRBL: ");
    // Serial.print(FRBL);
    // Serial.print(" FLBR: ");
    // Serial.print(FLBR);
    Serial.println();
    return;
}

void getData(void) {
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
}