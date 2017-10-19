#include <Arduino.h>
#include "DancingServos.h"

//TODO decide on some sort of control mechanism or auto running of the dance moves

/* Current Pins
 * hip R: 15
 * hip  L: 14
 * ankle R: 12
 * ankle L: 11
 */


void oscillatorTest();
void dancingServosTest();
void servoTest();
void calibrateTrims(DancingServos* bot);

DancingServos* bot;
Oscillator osc;

void setup() {
  bot = new DancingServos(14, 15, 11, 12);
  calibrateTrims(bot);

  //delay(1000);
  //Serial.begin(9600);
  //delay(2000);
}

void loop() {
  //oscillatorTest();
  //servoTest();
  //dancingServosTest();

  bot->position0();
  delay(1000);

  //dance moves here
  bot->themAnkles(1);
  bot->wiggle(30, 2);
  bot->hop(25, 1);
  bot->walk(4, 1500, false);
  bot->hop(18, 1);
  bot->walk(2, 1500, true);
  //add more moves

}

//manual calibration- based on how the servos are attatched to the parts
void calibrateTrims(DancingServos* bot) {
  //[hipL, hipR, ankleL, ankleR]
  bot->setTrims(75, 130, 45, 45);
}




//TEST FUNCTIONS
void servoTest(int angle) {
  osc.attach(12);
  osc.startO();
  osc.setAmp(40);
  osc.setOff(50);
  osc.setPer(2500);
  delay(2000);
  int x = osc.getPos();
  Serial.print(String(x));
  osc.setPos(angle);
  osc.stopO();
  while(true);
}

void oscillatorTest() {
  osc.attach(12);
  osc.startO();
  long period = 2500;
  long cycles = 5;
  long t0 = millis();
  for (long t = t0; t < (period * cycles + t0); t = millis()) {
    delay(30);
    osc.refreshPos();
  }
  osc.stopO();
  while(true);
}

void dancingServosTest() {
  bot->position0();
  delay(500);
  bot->themAnkles(5);
  delay(500);
  bot->position0();
  while(true);
}
