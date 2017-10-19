#include "DancingServos.h"

/* Current Pins
 * hip R: B6
 * hip  L: B7
 * ankle R: C6
 * ankle L: C7
 */



void calibrateTrims(DancingServos* bot);

DancingServos* bot;
Oscillator osc;

void setup() {
  bot = new DancingServos(B6, B7, C6, C7);
  calibrateTrims(bot);

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

}

//manual calibration- based on how the servos are attatched to the parts
void calibrateTrims(DancingServos* bot) {
  //[hipL, hipR, ankleL, ankleR]
  bot->setTrims(75, 130, 45, 45);
}
