/* DancingServos.cpp
 * UT Austin RAS Demobots
 * TO PROGRAM NEW DANCE MOVE FUNCTION
 *    pick the parameters for the oscillating movement
 *    call startOscillation()
 *    (look at walk() for an example)
 */

#include <Arduino.h>
#include "DancingServos.h"

//SETUP FUNCTIONS
DancingServos::DancingServos(int hL, int hR, int aL, int aR) {
  isOsc = false;
  endMoveTime = -1;
  pins[0] = hL;
  pins[1] = hR;
  pins[2] = aL;
  pins[3] = aR;
  for (int i = 0; i < 4; i++) {
    osc[i] = new Oscillator();
    osc[i]->attach(pins[i]);
  }
}

//set the trims of each motor for calibration
void DancingServos::setTrims(int tHL, int tHR, int tAL, int tAR) {
  osc[0]->setTrim(tHL);
  osc[1]->setTrim(tHR);
  osc[2]->setTrim(tAL);
  osc[3]->setTrim(tAR);
}


//DANCING FUNCTIONS

/* this functions is the basis of all the other dancing functions
 * set up an oscillation for each of the 4 servos
 * input format:   [hipL, hipR, ankleL, ankleR]
 */
void DancingServos::startOscillation(int amp[4], int off[4], double ph0[4], int period, float cycles) {
  //set the sinusoid parameters for each of the oscillators
  for (int i = 0; i < 4; i++) {
    osc[i]->setAmp(amp[i]);
    osc[i]->setOff(off[i]);
    osc[i]->setPh0(ph0[i]);
    osc[i]->setPer(period);
    osc[i]->startO();
  }
  
  //total oscillation time = (period * cycles)
  if (cycles == -1) {
    endMoveTime = -1;
  }
  else {
    endMoveTime = (period * cycles + millis());
  }
  
  isOsc = true;
}

void DancingServos::loopOscillation() {
  if (isOscillating()) {
    if ((endMoveTime == -1) || (millis() < endMoveTime)) {
      for (int i = 0; i < 4; i++) {
        osc[i]->refreshPos();
      }
    }
    else {
      stopOscillation();  
    }    
  }
}

void DancingServos::stopOscillation() {
  isOsc = false;
  endMoveTime = 0;
  for (int i = 0; i < 4; i++) {
    osc[i]->stopO();
    osc[i]->resetPh();
  }
}

void DancingServos::waitOscillation() {
  while(isOscillating()) {loopOscillation(); yield();}
}

bool DancingServos::isOscillating() {
  return isOsc;
}

String * DancingServos::getDanceMoves() {
  return danceMoves;
}

int DancingServos::getNumDanceMoves() {
  return numDanceMoves;
}

//[hipL, hipR, ankleL, ankleR]

//Move to resting poition
void DancingServos::position0() {
  int zeroi[4] = {0, 0, 0, 0};
  double zerod[4] = {0.0, 0.0, 0.0, 0.0};
  startOscillation(zeroi, zeroi, zerod, 2000, 1.0f);
}

//Move to resting poition
void DancingServos::themAnkles(int cycles) {
  int amp[4] = {0, 0, 20, 20};
  int off[4] = {0, 0, 0, 0};
  double ph0[4] = {0, 0, 0, 0};
  startOscillation(amp, off, ph0, 3000, cycles);
}

//Walk forward, adjust speed with T
void DancingServos::walk(float cycles, int period, bool reverse) {
  int rev = 1;
  if (reverse) {rev = -1;}
  int amp[4] = {18, 18, 15, 15};
  int off[4] = {0, 0, -4, -4};
  double ph0[4] = {0, 0, degToRad(rev * 90), degToRad(rev * 90)};
  startOscillation(amp, off, ph0, period, cycles);
}

//simultaneous ankles
void DancingServos::hop(int height, int cycles) {
  int amp[4] = {0, 0, height, height};
  int off[4] = {0, 0, height, -height};
  double ph0[4] = {0, 0, degToRad(-90), degToRad(90)};
  for (int i = 0; i < 4; i++) {osc[i]->setRev(true);}
  startOscillation(amp, off, ph0, 2000, cycles);
  for (int i = 0; i < 4; i++) {osc[i]->setRev(false);}
}

//simultaneous hips
void DancingServos::wiggle(int angle, int cycles) {
  int amp[4] = {angle, angle, 0, 0};
  int off[4] = {0, 0, 0, 0};
  double ph0[4] = {0, 0, 0, 0};
  startOscillation(amp, off, ph0, 2000, cycles);
}


//DANCE ROUTINES
void DancingServos::demo1() {
  themAnkles(1);
  waitOscillation();
  wiggle(30, 2);
  waitOscillation();
  hop(25, 1);
  waitOscillation();
  walk(4, 1500, false);
  waitOscillation();
  hop(18, 1);
  waitOscillation();
  walk(2, 1500, true);
  waitOscillation();
}

void DancingServos::demo2() {
  walk(2, 1500, false);
  waitOscillation();
  walk(2, 1500, true);
  waitOscillation();
  themAnkles(1);
  waitOscillation();
  wiggle(30, 1);
  waitOscillation();
  hop(25, 2);
  waitOscillation();
}



//MATH
double DancingServos::degToRad(double deg) {
  return (deg * PI) / 180.0;
}
