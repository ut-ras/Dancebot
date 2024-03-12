#include <PS4Controller.h> // PLEASE NOTE THAT YOU MAY HAVE TO WIPE ESP32 FLASH MEMORY IF BT DOESNT CONNECT
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Wire.h>

int LeftX, LeftY, RightX, RightY, Langle, Rangle;
double Lmagnitude, Rmagnitude;

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor2 = AFMS.getMotor(2);
Adafruit_DCMotor *motor3 = AFMS.getMotor(3);
Adafruit_DCMotor *motor4 = AFMS.getMotor(4);

void setup() {
  PS4.begin();
  AFMS.begin();
  Serial.begin(115200);
  Serial.println("Ready for controller connection!!"); 

  motor1->run(FORWARD);
  motor2->run(FORWARD);
  motor3->run(FORWARD);
  motor4->run(FORWARD);
}

void loop() {
  if (PS4.isConnected()) {

    // aliases for ease of typing
    LeftX = PS4.LStickX();
    LeftY = PS4.LStickY();
    RightX = PS4.RStickX();
    RightY = PS4.RStickY();

    // deadzone box to mitigate drift
    if(abs(LeftX) < 20 && abs(LeftY) < 20) {
      LeftX = 0;
      LeftY = 0;
    }
    if(abs(RightX) < 20 && abs(RightY) < 20) {
      RightX = 0;
      RightY = 0;
    }

    // get magnitude
    Lmagnitude = sqrt(pow(LeftX, 2) + pow(LeftY, 2));
    if(Lmagnitude > 125) { // cap at 125 to avoid mapping cartesian joystick coordinates to a circle (lol!)
      Lmagnitude = 125;
    }
    Lmagnitude = Lmagnitude/125;
    Serial.print("  M: ");
    Serial.print(Lmagnitude);

    // get angle from left stick
    Serial.print("  θ: ");
    Langle = ((int)(atan2(LeftY, LeftX) * 180/PI) + 360) % 360; 
    Serial.print(Langle);
    Serial.print("  ");

    Rmagnitude = sqrt(pow(RightX, 2) + pow(RightY, 2));
    if(Rmagnitude > 125) { // cap at 125 to avoid mapping cartesian joystick coordinates to a circle (lol!)
      Rmagnitude = 125;
    }
    Rmagnitude = Rmagnitude/125;
    Serial.print("  M: ");
    Serial.print(Rmagnitude);

    // get angle from right stick
    Serial.print("  θ: ");
    Rangle = ((int)(atan2(RightY, RightX) * 180/PI) + 360) % 360; 
    Serial.print(Rangle);
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
   
    motor1->setSpeed((int) Lmagnitude*255);
    motor2->setSpeed((int) Lmagnitude*255);


    motor3->setSpeed((int) Rmagnitude*255);
    motor4->setSpeed((int) Rmagnitude*255);


    delay(200);
  }
}


