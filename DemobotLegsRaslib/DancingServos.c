/* DancingServos.cpp
 * UT Austin RAS Demobots
 * TO PROGRAM NEW DANCE MOVE FUNCTION
 *    pick the parameters for the oscillating movement
 *    call startOscillation()
 *    (look at walk() for an example)
 */

#ifndef DANCINGSERVOS
  #include "DancingServos.h"
#endif

//SETUP FUNCTIONS
void InitializeDancingServos(tPin hL, tPin hR, tPin aL, tPin aR) {
  InitializeSystemTime();
  pins[0] = hL;
  pins[1] = hR;
  pins[2] = aL;
  pins[3] = aR;
  for (int i = 0; i < 4; i++) {
    osc[i] = new Oscillator(tPin pin);
  }
}

//set the trims of each motor for calibration
void DancingServos::setTrims(int tHL, int tHR, int tAL, int tAR) {
  setTrim(osc[0], tHL);
  setTrim(osc[1], tHR);
  setTrim(osc[2], tAL);
  setTrim(osc[3], tAR);
}


//DANCING FUNCTIONS

/* this functions is the basis of all the other dancing functions
 * set up an oscillation for each of the 4 servos
 * input format:   [hipL, hipR, ankleL, ankleR]
 */
void startOscillation(int amp[4], int off[4], double ph0[4], int period, float cycles) {
  //set the sinusoid parameters for each of the oscillators
  for (int i = 0; i < 4; i++) {
    setAmp(osc[i], amp[i]);
    setOff(osc[i], off[i]);
    setPh0(osc[i], ph0[i]);
    setPer(osc[i], period);
    startO(osc[i]);
  }
  //run the refreshPos() function on each oscillator for the correct time
  //total oscillation time = (period * cycles)
  long t0 = GetTimeUS()*1000;
  for (long t = t0; t < (period * cycles + t0); t = GetTimeUS()) {
    for (int i = 0; i < 4; i++) {
      refreshPos(osc[i]);
    }
  }
  for (int i = 0; i < 4; i++) {
    stopO(osc[i]);
    resetPh(osc[i]);
  }
}

//[hipL, hipR, ankleL, ankleR]

//Move to resting poition
void position0() {
  int zeroi[4] = {0, 0, 0, 0};
  double zerod[4] = {0.0, 0.0, 0.0, 0.0};
  startOscillation(zeroi, zeroi, zerod, 2000, 1.0f);
}

//Move to resting poition
void themAnkles(int cycles) {
  int amp[4] = {0, 0, 20, 20};
  int off[4] = {0, 0, 0, 0};
  double ph0[4] = {0, 0, 0, 0};
  startOscillation(amp, off, ph0, 3000, cycles);
}

//Walk forward, adjust speed with T
void walk(float cycles, int period, int reverse) {
  int rev = 1;
  if (reverse) {rev = -1;}
  int amp[4] = {18, 18, 15, 15};
  int off[4] = {0, 0, -4, -4};
  double ph0[4] = {0, 0, degToRad(rev * 90), degToRad(rev * 90)};
  startOscillation(amp, off, ph0, period, cycles);
}

//simultaneous ankles
void hop(int height, int cycles) {
  int amp[4] = {0, 0, height, height};
  int off[4] = {0, 0, height, -height};
  double ph0[4] = {0, 0, degToRad(-90), degToRad(90)};
  for (int i = 0; i < 4; i++) {osc[i]->setRev(true);}
  startOscillation(amp, off, ph0, 2000, cycles);
  for (int i = 0; i < 4; i++) {osc[i]->setRev(false);}
}

//simultaneous hips
void wiggle(int angle, int cycles) {
  int amp[4] = {angle, angle, 0, 0};
  int off[4] = {0, 0, 0, 0};
  double ph0[4] = {0, 0, 0, 0};
  startOscillation(amp, off, ph0, 2000, cycles);
}

//CALIBRATION
/* //doesnt work without encoders, servo can't read until data has been written
void DancingServos::calibrateTrims() {
  for (int i = 0; i < 4; i++) {
    int angle = positionFromEncoder;
    osc[i]->setTrim(angle);
    //Serial.println("trim " + String(i) + ": " + String(angle));
  }
}*/

//MATH
double degToRad(double deg) {
  return (deg * PI) / 180.0;
}
