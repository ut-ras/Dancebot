#include <PS4Controller.h> 
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <ESP32Servo.h> 
#include <Wire.h> //i2c library
#include <WiFi.h>

#define servoPin 12

int LeftX, LeftY, RightX, RightY, FRBL, FLBR, Mode;
float Lmagnitude, Rmagnitude, Langle, Rangle;
Servo myServo;

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

  myServo.attach(servoPin);

  // init all motors to drive forward
  BackRight->run(FORWARD);
  FrontRight->run(FORWARD);
  FrontLeft->run(FORWARD);
  BackLeft->run(FORWARD);
}

void loop() {
  if (PS4.isConnected()) {
    int FRBRFLBL;
    while(1) {
           
        Mode = 0;
        getData();

        printData();
        
        // calculate speed as function as magnitude of joystick
        FRBRFLBL = (Rmagnitude * 200);
        
        FrontRight->setSpeed(FRBRFLBL);
        BackRight->setSpeed(FRBRFLBL);
        FrontLeft->setSpeed(FRBRFLBL);
        BackLeft->setSpeed(FRBRFLBL);
        
        // spin belt
        if(PS4.Cross() > 0) {
          myServo.write(1750);
          Serial.println("BELT SPINNING");
        }
        else {
          myServo.write(1500);
          Serial.println("BELT STOPPED");
        }
        // rotate clockwise
        if((Rangle >= 5.497) && (Rangle < 6.28) || (Rangle <= 0.785) && (Rangle > 0)) {
          FrontLeft->run(FORWARD);
          BackLeft->run(FORWARD);
          FrontRight->run(BACKWARD);
          BackRight->run(BACKWARD);
          Serial.println("CLOCKWISE");
          continue;
        }
        // forward
        else if((0.785 < Rangle) && (Rangle < 2.356)) {
          FrontLeft->run(BACKWARD);
          BackLeft->run(BACKWARD);
          FrontRight->run(BACKWARD);
          BackRight->run(BACKWARD);
          Serial.println("FORWARD");
          continue;
        }

        // backward
        else if((3.926 < Rangle) && (Rangle < 5.497)) {
          FrontLeft->run(FORWARD);
          BackLeft->run(FORWARD);
          FrontRight->run(FORWARD);
          BackRight->run(FORWARD);
          Serial.println("BACKWARD");
          continue;
        }
        // rotate counterclockwise
        else if(((2.356 <= Rangle) && (Rangle <= 3.926))) { // 
          FrontLeft->run(BACKWARD);
          BackLeft->run(BACKWARD);
          FrontRight->run(FORWARD);
          BackRight->run(FORWARD);
          Serial.println("COUNTERCLOCKWISE");
          continue;
        }


        // latch to omni drive
        if(PS4.Square()) {
          while(1) {
                Mode = 1;
                getData();
                printData();

                //  FRBL: sin(angle−1/4π) * magnitude
                //  FLBR: sin(angle+1/4π) * magnitude 
              
                // positive FRBL or FLBR goes BACKWARD!!
                // FRBL = ((255*Rmagnitude) * sin(Rangle-(0.25*PI)));
                // FLBR = -((255*Rmagnitude) * sin(Rangle+(0.25*PI)));
                //^ prob wont need the negative sign on FLBR because orientation already flipped through hardware

                FRBL = (100*Rmagnitude) * (sin(Rangle-(0.25*3.14)));
                FLBR = (100*Rmagnitude) * (sin(Rangle+(0.25*3.14)));

                if (FRBL < 0){
                  FrontRight->run(FORWARD);
                  BackLeft->run(FORWARD);
                  FRBL = FRBL * (-1);
                }
                else{
                  FrontRight->run(BACKWARD);
                  BackLeft->run(BACKWARD);
                }

                if (FLBR < 0) {
                  FrontLeft->run(FORWARD);
                  BackRight->run(FORWARD);
                  FLBR = FLBR * (-1);
                }
                else{
                  FrontLeft->run(BACKWARD);
                  BackRight->run(BACKWARD);
                }
                
                FrontLeft->setSpeed(FLBR);
                BackRight->setSpeed(FLBR);

                FrontRight->setSpeed(FRBL);
                BackLeft->setSpeed(FRBL);

                
                if(PS4.Triangle()) {
                  break;
                }
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

void getData(void) {
    // aliases for ease of typing
    // LeftX = PS4.LStickX();
    // LeftY = PS4.LStickY();
    RightX = PS4.RStickX();
    RightY = PS4.RStickY();

    // deadzone box to mitigate drift effects
    // if(abs(LeftX) < 20 && abs(LeftY) < 20) {
    //   LeftX = 0;
    //   LeftY = 0;
    // }
    if(abs(RightX) < 20 && abs(RightY) < 20) {
      RightX = 0;
      RightY = 0;
    }
    
    // get joystick data
    // Lmagnitude = getMagnitude(LeftX, LeftY);
    Rmagnitude = getMagnitude(RightX, RightY);
    // Langle = getAngle(LeftX, RightY);
    Rangle = getAngle(RightX, RightY);
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
    Serial.print(" FRBL: ");
    Serial.print(FRBL);
    Serial.print(" FLBR: ");
    Serial.print(FLBR);
    Serial.println();
    return;
}

