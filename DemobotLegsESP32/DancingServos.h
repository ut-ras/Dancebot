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


class DancingServos {
public:
  DancingServos(int hL, int hR, int aL, int aR);
  void setTrims(int tHL, int tHR, int tAL, int tAR);

  //movements to choose from, calling one of these functions will start a dance
  void position0();
  void walk(float cycles, int period, bool reverse);
  void themAnkles(int cycles);
  void hop(int height, int times);
  void wiggle(int angle, int times);

  //busy-wait demos
  void demo1();
  void demo2();

  //Four Oscillators
  void startOscillation(int amp[4], int off[4], double ph0[4], int period, float cycles);
  void loopOscillation();
  void stopOscillation();
  void waitOscillation();
  bool isOscillating();

  String * getDanceMoves();
  int getNumDanceMoves();
  
private:
  double degToRad(double deg);

  //[hipL, hipR, ankleL, ankleR]
  Oscillator* osc[4];
  int pins[4];
  bool isOsc;
  long endMoveTime = 0;

  int numDanceMoves = 6;
  String danceMoves[6] = {"Stop", "Reset", "Walk", "Hop", "Wiggle", "Ankles"};
  
};

#endif
