/* DancingServos.h
 * UT Austin RAS Demobots
 * Control class for a set of 4 oscillating servos
 * There are 2 servos on each leg of the bot: one at the hip and one at the ankle
 * Functions to oscillate dance moves
 * standard input order for functions should be [hipL, hipR, ankleL, ankleR]
 * 
 * To use, call loopOscillation each loop().
 * Call a dance move function or startOscillation to begin a move. 
 * Use a demo function in the loop to call a new dance move every time one stops.
 */

#ifndef DANCINGSERVOS
#define DANCINGSERVOS

#include "Oscillator.h"


class DancingServos {
public:
  DancingServos(int hL, int hR, int aL, int aR);
  void setTrims(int tHL, int tHR, int tAL, int tAR);

  //dance moves - movements to choose from, calling one of these functions will start a dance, loopOscillation will run it each loop
  void position0();
  void walk(float cycles, int period, bool reverse);
  void themAnkles(int cycles);
  void hop(int height, int times);
  void wiggle(int angle, int times);

  void heel_toe(int cycles, bool left_direction);
  void stank(int cycles, bool left_ankle);
  void simpler_hop(int angle, int cycles);
  void wave(int angle, int cycles);

  // TEST MOVES
  void ankles(int cycles);
  void ankles_phase(int cycles);
  void ankles_offset(int cycles);
  void legs(int cycles);
  void legs_phase(int cycles);
  void legs_offset(int cycles);

  //dance routines - run once per loop, also call loopOscillation as normal
  void demo1();
  void demo2();

  //functions to interact with the four Oscillators
  void startOscillation(int amp[4], int off[4], double ph0[4], int period, float cycles);
  void loopOscillation();
  void stopOscillation();
  void waitOscillation();       //this does not currently work, arduino doesn't like this type of loop
  bool isOscillating();

  //get info about the dance moves
  String * getDanceMoves();
  int getNumDanceMoves();

  //get info about the dance routines
  String * getDanceRoutines();
  int getNumDanceRoutines();

  //run the dance routines
  void loopDanceRoutines();               //call once per loop, checks the current dance routine and activates its function
  void enableDanceRoutine(bool dance);
  void setDanceRoutine(int dance);
  
private:
  double degToRad(double deg);

  //[hipL, hipR, ankleL, ankleR]
  Oscillator* osc[4];
  int pins[4];
  bool isOsc;
  long endMoveTime = 0;

  // dev notes: new moves below:
  int numDanceMoves = 12;
  String danceMoves[12] = {"Stop", "Reset", "Walk", "Hop", "Wiggle", "Ankles", "Left Heel Toe", "Right Heel Toe", "Left Stank", "Right Stank", "Backwards Walk", "Wave"};
  // String danceMoves[18] = {"Stop", "Reset", "Walk", "Hop", "Wiggle", "Ankles", "Left Heel Toe", "Right Heel Toe", "Left Stank", "Right Stank", "Backwards Walk", "Wave", 
  //     "ankles", "ankles_phase", "ankles_offset", "legs", "legs_phase", "legs_offset"};
  // void  (*danceMoveFunctions[])() = {};   //TODO standardize dance move function input parameters

  bool doDanceRoutine = false;
  int currentDanceRoutine = 0;
  int numDanceRoutines = 2;
  String danceRoutines[2] = {"Demo 1", "Demo 2"};
  void (DancingServos::* danceRoutineFunctions[2])() = {&DancingServos::demo1, &DancingServos::demo2};    //TODO use in loopDanceRoutines
  
};

void dancingServosTest(DancingServos* bot);

#endif
