//Launch Pad
#include "Energia.h"
#include "DancingServos.h"
#include "Servo.h"

/* Current Pins
   hip R: C4
   hip  L: B3
   ankle R: C6
   ankle L: C7
*/
#define B0 3
#define B5 2
#define C5 36
#define C4 37
#define C6 35
#define C7 34
#define D1 24


void calibrateTrims(DancingServos* bot);

DancingServos* bot;
Oscillator osc;

Servo* s;

void setup() {
  //[hipL, hipR, ankleL, ankleR]
  bot = new DancingServos(C4, C5, C7, C6);
  calibrateTrims(bot);

  //s = new Servo();
  //s->attach(34);

  //delay(1000);
  //Serial.begin(9600);
  //delay(2000);
}

void loop() {
  
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

  /*
  s->write(70);
  delay(500);
  s->write(30);
  delay(500);
  */
}

//manual calibration- based on how the servos are attatched to the parts
void calibrateTrims(DancingServos* bot) {
  //[hipL, hipR, ankleL, ankleR]
  bot->setTrims(75, 130, 45, 45);
}
