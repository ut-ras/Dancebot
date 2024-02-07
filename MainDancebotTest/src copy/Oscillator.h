/* Oscillator.h
 * UT Austin RAS Demobots
 * Oscillate non-continuous servos in sinusoidal motion
 * Basically just a wrapper class for the Arduino library servo class.
 * Each loop, it sets the servo motor to the current value in the sine wave. 
 */

#ifndef OSCILLATOR_H
#define OSCILLATOR_H

/* HOW TO USE OSCILLATOR CLASS
 * 1) create Oscillator object
 * 2) attach it to a pin
 * 3) set desired sinusoid parameters, calibrate and set trim if needed
 *         amplitude is basiaclly the degree of rotation for the oscillating movement
 * 4) run refreshPos() on the main loop
 * 5) enable oscillation with start()
 */
#include <ESP32Servo.h>

//These values depend on the servo motors, check the data sheet for minUs and maxUs. Used for Oscillator::attach().
#define MIN_US 900
#define MAX_US 2100


class Oscillator {
public:
  Oscillator();
  Oscillator(int a, int o, double p0, int t, bool r);

  //setup functions from Servo class
  void attach(int pin);
  void detach();

  //sinusoid functions
  void refreshPos();              //set servo pos based on sinusoid
  
  //sinusoid parameters
  void setAmp(int a);             //set Amplitude (degrees)
  void setOff(int o);             //set Offset (degrees)
  void setPh0(double p0);         //set Initial Phase (radians)
  void setPer(int t); //set Period (ms)
  void setRev(bool r);            //Set Reverse on/off (default off)

  //control
  void stopO();
  void startO();
  void setPos(int p);             //set Position (degrees)
  int getPos();                 
  void resetPh();                 //set Current Phase to 0

  //calibration (if we need it, built into refreshPos, just call setTrim)
  void setTrim(int t);       //set Trim (degrees)
  int getTrim();             //get Trim (degrees)

private:
  //sinusoid functions
  bool checkRefreshTime();    //check if refresh time increment has passed
  long t_current;             //current time (ms)
  long t_lastRefresh;         //time of last refreshPos() (ms)

  //Arduino Servo object
  Servo* servo;
  bool servoAttached;         //true when a servo is attached

  //sinusoid parameters
  int amp;     //Amplitude (degrees)
  int off;     //Offset (degrees)
  double ph0;           //Initial Phase (radians)
  int period;           //Period (ms)
  int rev;              //Reverse Sinusoid multiplier (1 = no rev, -1 = rev)

  //servo status variables
  bool isStopped;
  int pos;        //Current Position (degrees)
  double ph;      //Current Phase (radians)
  double phInc;    //Phase increment (radians)
  int samplePeriod;    //how often to sample servos for pos (ms)

  //calibration (if we need it)
  int trim;     //add to position (degrees)
};

void oscillatorTest();
void servoTest();

#endif

