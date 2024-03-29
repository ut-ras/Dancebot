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
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN1     25
#define LED_PIN2     26

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  1

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 50 // Set BRIGHTNESS to about 1/5 (max = 255)

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

//LED Functions
void setup() {
  pixels.begin();
  pixels.setBrightness(40); // 1/3 brightness
  prevTime; //Need to figure out millisecond counter here
}

void loop() {
  uint8_t  i;
  uint32_t t;

  switch(mode) {
    
   case 0: //rainbow hold
    rainbowHold(20);
    //delay(500); make loop for delay
    break;
    
   case 1: //rainbow cycle slow
    rainbowCycleslow(20);
    //delay(50);
    break;
       
   case 2: //rainbow cycle fast 
    rainbowCycle(5);
    //delay(50);
    break;
  }

  //t = millis(); figure out milisecond counter
  if((t - prevTime) > 8000) {      // Every 8 seconds...
    mode++;                        // Next mode
    if(mode > 3) {                 // End of modes?
      mode = 0;                    // Start modes over
      color >>= 8;                 // Next color R->G->B
      if(!color) color = 0xB300A4; // Reset color
    }
    for(i=0; i<32; i++) pixels.setPixelColor(i, 0);
    prevTime = t;
    
  }

  
}
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    //delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t r, j;

  for(j=0; j<256*6; j++) { // 6 cycles of all colors on wheel
    for(r=0; r< pixels.numPixels(); r++) {
      pixels.setPixelColor(r, Wheel(((r * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    //delay(wait);
  }
}
void rainbowCycleslow(uint8_t wait) {
  uint16_t r, j;

  for(j=0; j<256*3; j++) { // 3 cycles of all colors on wheel
    for(r=0; r< pixels.numPixels(); r++) {
      pixels.setPixelColor(r, Wheel(((r * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    //delay(wait);
  }
}
void rainbowHold(uint8_t wait) {
  uint16_t r, j;

  for(j=0; j<256*1; j++) { // 3 cycles of all colors on wheel
    for(r=0; r< pixels.numPixels(); r++) {
      pixels.setPixelColor(r, Wheel(((r * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    //delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(int8_t WheelPos) {
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



//DANCE ROUTINE FUNCTIONS

void DancingServos::loopDanceRoutines() {
  if (doDanceRoutine) {
    //(((DancingServos*)this)->DancingServos::danceRoutineFunctions[currentDanceRoutine])();    //TODO
    switch(currentDanceRoutine) {
      case 0:
        demo1(); break;
      case 1:
        demo2(); break;
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
  int off[4] = {0, 0, -height, height};
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
