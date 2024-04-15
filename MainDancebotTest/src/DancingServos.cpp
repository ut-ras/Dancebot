/* DancingServos.cpp
 * UT Austin RAS Demobots
 * TO PROGRAM NEW DANCE MOVE FUNCTION
 *    pick the parameters for the oscillating movement
 *    call startOscillation()
 *    (look at walk() for an example)
 */

#include <Arduino.h>
#include "DancingServos.h"
#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN1     25
#define LED_PIN2     26

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  1

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 50 // Set BRIGHTNESS to about 1/5 (max = 255)

Adafruit_NeoPixel pixels;

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

//NEOPIXEL LED FUNCTIONS
void DancingServos::setupNeopixel(Adafruit_NeoPixel pixels_){
  pixels = pixels_;
  pixels.begin();
  pixels.setBrightness(40); // 1/3 brightness
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t DancingServos::wheel(int8_t WheelPos) {
  if(WheelPos < 85) {
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void DancingServos::rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, wheel((i+j) & 255));
    }
    pixels.show();
    //delay(wait);
  }
}

void DancingServos::rainbowCycle(uint8_t wait) {
  uint16_t r, j;

  for(j=0; j<256*6; j++) { // 6 cycles of all colors on wheel
    for(r=0; r< pixels.numPixels(); r++) {
      pixels.setPixelColor(r, wheel(((r * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    //delay(wait);
  }
}
void DancingServos::rainbowCycleslow(uint8_t wait) {
  uint16_t r, j;

  for(j=0; j<256*3; j++) { // 3 cycles of all colors on wheel
    for(r=0; r< pixels.numPixels(); r++) {
      pixels.setPixelColor(r, wheel(((r * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    //delay(wait);
  }
}
void DancingServos::rainbowHold(uint8_t wait) {
  uint16_t r, j;

  for(j=0; j<256*1; j++) { // 3 cycles of all colors on wheel
    for(r=0; r< pixels.numPixels(); r++) {
      pixels.setPixelColor(r, wheel(((r * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    //delay(wait);
  }
}


//DANCE MOVE FUNCTIONS

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
  while(isOscillating()) {
    loopOscillation(); 
    delay(50);
    yield();
  }
}

bool DancingServos::isOscillating() {
  return isOsc;
}



//DANCE ROUTINE FUNCTIONS

void DancingServos::loopDanceRoutines() {
  if (doDanceRoutine) {
    //(((DancingServos*)this)->DancingServos::danceRoutineFunctions[currentDanceRoutine])();    //TODO
    switch(currentDanceRoutine) {
      case 0:
        demo1(); break;
      case 1:
        demo2(); break;

      // dev notes: UPDATE FOR NEW DANCE ROUTINES
      case 2:
        demo3(); break;
      case 3: 
        demo4(); break;
    }
  }
}

void DancingServos::enableDanceRoutine(bool dance) {
  doDanceRoutine = dance;
}

void DancingServos::setDanceRoutine(int dance) {
  currentDanceRoutine = dance;
}





//DANCE MOVES

//wrappers for startOscillation()
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
  // startOscillation(amp, off, ph0, 3000, cycles);
  startOscillation(amp, off, ph0, 1500, cycles);

}

//Walk forward, adjust speed with T
// walk backwards by calling this with reverse = true
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

  // dev notes: preferably angle set to 30
  int amp[4] = {0, 0, height, -height};
  int off[4] = {0, 0, height, -height};
  double ph0[4] = {0, 0, 0, 0};
  startOscillation(amp, off, ph0, 1500, cycles);

  // dev notes: below is code for the big dance bot
  // int amp[4] = {0, 0, height, height};
  // int off[4] = {0, 0, -height, height};
  // double ph0[4] = {0, 0, degToRad(-90), degToRad(90)};
  // for (int i = 0; i < 4; i++) {osc[i]->setRev(true);}
  // startOscillation(amp, off, ph0, 2000, cycles);
  // for (int i = 0; i < 4; i++) {osc[i]->setRev(false);}
}

//simultaneous hips
void DancingServos::wiggle(int angle, int cycles) {
  int amp[4] = {angle, angle, 0, 0};
  int off[4] = {0, 0, 0, 0};
  double ph0[4] = {0, 0, 0, 0};
  startOscillation(amp, off, ph0, 2000, cycles);
}

// dev notes: goals for the future
// 1. add new dance moves and refine
// 2. overhaul dance servo system(?)
// 3. coordinate multiple bots together
//    (modifications in wifi and beacons)

// "hip toe" dance move
void DancingServos::heel_toe(int cycles, bool left_direction) {
  int amp[4] = {20, -20, 0, 0};

  if (left_direction) {
    amp[2] = -40;
    amp[3] = -40;
  } else {
    amp[2] = 40;
    amp[3] = 40;
  }

  // since toes initially pointed inwards?
  int off[4] = {0, 0, 0, 0};
  double ph0[4] = {0, 0, 0, 0};

  startOscillation(amp, off, ph0, 2000, cycles);
  // only reverse the ankles for second part?
  // for (int i = 2; i < 4; i++) {osc[i]->setRev(true);}
}


// "stank" dance move
void DancingServos::stank(int cycles, bool left_ankle) {
  int amp[4] = {0, 0, 0, 0};
  double ph0[4] = {0, 0, degToRad(90), 0};

  if (left_ankle) {
    amp[0] = -40;
    amp[2] = 30;

    ph0[2] = degToRad(90);
  } else {
    amp[1] = 40;
    amp[3] = 30;

    ph0[3] = degToRad(90);
  }

  // presumably start from resting position
  int off[4] = {0, 0, 0, 0};
  // double ph0[4] = {0, 0, degToRad(90), 0};

  startOscillation(amp, off, ph0, 2000, cycles);
}


// "wave" dance move
void DancingServos::wave(int angle, int cycles) {

  // dev notes: slightly dumb way of specifying degree of motion
  // but for wave, the angle ~ 45 deg

  int amp[4] = {0, 0, angle, angle};
  int off[4] = {0, 0, -angle, angle};
  // int off[4] = {0, 0, angle, -angle}; for the big dance bot
  double ph0[4] = {0, 0, 0, degToRad(90)};
  startOscillation(amp, off, ph0, 2000, cycles);
}

// void DancingServos::shimmy(int angle, int cycles)
// void DancingServos::ketou(int cycles) 

// dev notes: TEST MOVES
void DancingServos::ankles(int cycles) {
  int amp[4] = {0, 0, 45, -45};
  int off[4] = {0, 0, 0, 0};
  double ph0[4] = {0, 0, 0, 0};
  startOscillation(amp, off, ph0, 2000, cycles);
}

void DancingServos::ankles_phase(int cycles) {
  int amp[4] = {0, 0, 40, -40};
  int off[4] = {0, 0, 0, 0};
  double ph0[4] = {0, 0, 0, degToRad(90)};
  startOscillation(amp, off, ph0, 2000, cycles);
}

void DancingServos::ankles_offset(int angle, int cycles) {
  // dev notes: preferably angle set to 40
  int amp[4] = {0, 0, angle, -angle};
  int off[4] = {0, 0, angle, -angle};
  double ph0[4] = {0, 0, 0, 0};
  startOscillation(amp, off, ph0, 2000, cycles);
}

void DancingServos::legs(int cycles) {
  int amp[4] = {40, -40, 0, 0};
  int off[4] = {0, 0, 0, 0};
  double ph0[4] = {0, 0, 0, 0};
  startOscillation(amp, off, ph0, 2000, cycles);
}

void DancingServos::legs_phase(int cycles) {
  int amp[4] = {40, -40, 0, 0};
  int off[4] = {0, 0, 0, 0};
  double ph0[4] = {0, degToRad(90), 0, 0};
  startOscillation(amp, off, ph0, 2000, cycles);
}

void DancingServos::legs_offset(int cycles) {
  int amp[4] = {20, 20, 0, 0};
  int off[4] = {20, 0, 0, 0};
  double ph0[4] = {0, 0, 0, 0};
  startOscillation(amp, off, ph0, 2000, cycles);
}

//DANCE ROUTINES

void DancingServos::demo1() {
  static int i = 0;
  static int numMoves = 6;
  if (!isOscillating()) {
    if (i == 0)      {  themAnkles(1);  }
    else if (i == 1) {  wiggle(30, 2);  }
    else if (i == 2) {  hop(25, 1);     }
    else if (i == 3) {  walk(4, 1500, false); }
    else if (i == 4) {  hop(18, 1);     }
    else if (i == 5) {  walk(2, 1500, true);  }
    i = (i + 1) % numMoves;
  }
}

void DancingServos::demo2() {
  static int i = 0;
  static int numMoves = 5;
  if (!isOscillating()) {
    if (i == 0)      {  walk(2, 1500, false);   }
    else if (i == 1) {  walk(2, 1500, true);    }
    else if (i == 2) {  themAnkles(1);    }
    else if (i == 3) {  wiggle(30, 1);    }
    else if (i == 4) {  hop(25, 2);       }
    i = (i + 1) % numMoves;
  }
}

// dev notes: new demos for showcase?
void DancingServos::demo3() {
  static int i = 0;
  static int numMoves = 9;
  if (!isOscillating()) {
    // dev notes: walk is now reverse?
    if (i == 0)         walk(2, 1500, false);
    else if (i == 1)    heel_toe(1, true);
    else if (i == 2)    heel_toe(1, false);
    else if (i == 3)    walk(2, 1500, true);
    else if (i == 4)    stank(1, true);
    else if (i == 5)    stank(1, false);
    else if (i == 6)    wave(40, 1);
    else if (i == 7)    hop(40, 2);
    else if (i == 8)    wiggle(30, 2);
    i = (i + 1) % numMoves;
  }
}

void DancingServos::demo4() {
  static int i = 0;
  static int numMoves = 6;
  if (!isOscillating()) {
    if (i == 0)         walk(1, 1500, false);
    else if (i == 1)    walk(1, 1500, true);
    else if (i == 2)    wiggle(30, 2);
    else if (i == 3)    wave(40, 2);
    else if (i == 4)    hop(40, 2);
    else if (i == 5) {  themAnkles(1);    }
    i = (i + 1) % numMoves;
  }
}


//INFO

String * DancingServos::getDanceMoves() {
  return danceMoves;
}

int DancingServos::getNumDanceMoves() {
  return numDanceMoves;
}

String * DancingServos::getDanceRoutines() {
  return danceRoutines;
}

int DancingServos::getNumDanceRoutines() {
  return numDanceRoutines;
}



//MATH
double DancingServos::degToRad(double deg) {
  return (deg * PI) / 180.0;
}



//TEST FUNCTIONS
void dancingServosTest(DancingServos* bot) {
  bot->position0();
  delay(500);
  bot->themAnkles(5);
  delay(500);
  bot->position0();
  while(true);
}
