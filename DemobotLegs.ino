#include <Arduino.h>
#include "DancingServos.h"

void oscillatorTest();
void dancingServosTest();
void servoTest();

Oscillator osc;

void setup() {
    osc.attach(12);
    osc.setAmp(40);
    osc.setOff(50);
    osc.setPer(2500);
    delay(1000);
    Serial.begin(9600);
    delay(1000);
}

void loop() {
  //TODO decide on some sort of control mechanism or auto running of the dance moves

  osc.startO();
  oscillatorTest();
  //servoTest();
}

/* TODO create a function that takes in the oscillator parameters for 4 motors
 * then sets all the oscillator parameters
 */

/* TODO create functions for dance moves
 * decide on the parameters for the 4 oscillators for the particular dance move
 * then use the function described above
 */



//TEST FUNCTIONS
void servoTest() {
  for(int i = 0; i < 20000; i++) {
    osc.setPos(20);
    delay(2000);
    osc.setPos(50);
    delay(2000);
  }
}
void oscillatorTest() {
  long period = 2500;
  long cycles = 5;
  long t0 = millis();
  for (long t = t0; t < (period * cycles + t0); t = millis()) {
    delay(30);
    osc.refreshPos();
  }
  
}
void dancingServosTest() {
  //TODO implement test function
}
