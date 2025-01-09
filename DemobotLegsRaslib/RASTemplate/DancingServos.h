/* DancingServos.h
 * UT Austin RAS Demobots
 * Control class for a set of 4 oscillating servos
 * There are 2 servos on each leg of the bot: one at the hip and one at the ankle
 * Functions to oscillate dance moves
 * standard input order for functions should be [hipL, hipR, ankleL, ankleR]
 */

#ifndef DANCINGSERVOS
  #define DANCINGSERVOS
  #include "Oscillator.h"
  #include <RASLib/inc/common.h>
  #include <RASLib/inc/time.h>
#endif

  void InitializeDancingServos(tPin hL, tPin hR, tPin aL, tPin aR);
  void setTrims(int tHL, int tHR, int tAL, int tAR);

  //movements to choose from, calling one of these functions will start a dance
  void position0();
  void walk(float cycles, int period, bool reverse);
  void themAnkles(int cycles);
  void hop(int height, int times);
  void wiggle(int angle, int times);

  //dance routines
  void demo1();
  void demo2();

  void startOscillation(int amp[4], int off[4], double ph0[4], int period, float cycles);
  //void calibrateTrims();
  double degToRad(double deg);

  //[hipL, hipR, ankleL, ankleR]
  oscillator* osc[4];
  tPin pins[4];
