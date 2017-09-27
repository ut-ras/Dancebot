/* DancingServos.cpp
 * UT Austin RAS Demobots
 */

#include <Arduino.h>

#ifndef DANCINGSERVOS
  #include "DancingServos.h"
#endif

//SETUP FUNCTIONS
DancingServos::DancingServos(int hL, int hR, int aL, int aR) {
  pins[0] = hL;
  pins[1] = hR;
  pins[2] = aL;
  pins[3] = aR;
  for (int i = 0; i < 4; i++) {
    osc[i].attach(pins[i]);
  }
  //TODO if we save trims to EEPROM, load from memory
}

//set the trims of each motor for calibration
void DancingServos::setTrims(int tHL, int tHR, int tAL, int tAR) {
  trim[0] = tHL;
  trim[1] = tHR;
  trim[2] = tAL;
  trim[3] = tAR;
  //TODO call set trim function, maybe save on EEPROM in between sessions
}


//DANCING FUNCTIONS

/* this functions is the basis of all the other dancing functions
 * set up an oscillation for each of the 4 servos
 * input format:   [hipL, hipR, ankleL, ankleR]
 */
void DancingServos::startOscillation(int amp[4], int off[4], double ph0[4], int period, float cycles) {
  //set the sinusoid parameters for each of the oscillators
  for (int i = 0; i < 4; i++) {
    osc[i].setAmp(amp[i]);
    osc[i].setOff(off[i]);
    osc[i].setPh0(ph0[i]);
    osc[i].setPer(period);
  }
  //run the refreshPos() function on each oscillator for the correct time
  //total oscillation time = (period * cycles)
  long t0 = millis();
  for (long t = t0; t < (period * cycles + t0); t = millis()) {
    for (int i = 0; i < 4; i++) {
      osc[i].refreshPos();
    }
  }
}

//Move to resting poition
void DancingServos::position0() {
  int zeroi[4] = {0, 0, 0, 0};
  double zerod[4] = {0.0, 0.0, 0.0, 0.0};
  startOscillation(zeroi, zeroi, zerod, 500, 1.0f);
}

//Walk forward
void DancingServos::walk(float cycles, int period, bool reverse) {
  int rev = 1;
  if (reverse) {rev = -1;}
  int amp[4] = {30, 30, 20, 20};
  int off[4] = {0, 0, -4, -4};
  double ph0[4] = {0, 0, degToRad(rev * -90), degToRad(rev * -90)};
  startOscillation(amp, off, ph0, period, cycles);
}


//MATH
double DancingServos::degToRad(double deg) {
  return (deg * PI) / 180.0;
}
