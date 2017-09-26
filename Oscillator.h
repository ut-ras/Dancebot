/* Oscillator.h
 * UT Austin RAS Demobots
 * Oscillate servos in sinusoidal motion
 * Basically just a wrapper class for the Arduino library servo class
 */

//this prevents errors if Oscillator.h is included in multiple files
#ifndef OSCILLATOR
  #define OSCILLATOR
  #define PI 3.14159265358979323846

#include <Servo.h>

//TODO create OscillatorTest, update these steps if needed

/* HOW TO USE OSCILLATOR CLASS
 * 1) create Oscillator object
 * 2) attach it to a pin
 * 3) set desired sinusoid parameters, calibrate and set trim if needed
 * 4) run refreshPos() on the main loop
 * 5) enable oscillation with start()
 */


class Oscillator {
public:
  Oscillator();

  //setup functions from Servo class
  void attach(int pin);
  void detach();

  //sinusoid functions
  void refreshPos();              //set servo pos based on sinusoid
  //sinusoid parameters
  void setAmp(unsigned int a);    //set Amplitude (degrees)
  void setOff(unsigned int o);    //set Offset (degrees)
  void setPh0(double p0);         //set Initial Phase (radians)
  void setPeriod(unsigned int t); //set Period (ms)
  void setRev(bool r);            //Set Reverse on/off (default off)

  //control
  void stop();
  void start();
  void setPos(int p);             //set Position (degrees)
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
  Servo servo;
  bool servoAttached;         //true when a servo is attached

  //sinusoid parameters
  unsigned int amp;     //Amplitude (degrees)
  unsigned int off;     //Offset (degrees)
  double ph0;           //Initial Phase (radians)
  unsigned int period;  //Period (ms)
  int rev;              //Reverse Sinusoid multiplier (1 = no rev, -1 = rev)

  //servo status variables
  bool isStopped;
  int pos;        //Current Position (degrees)
  double ph;      //Current Phase (radians)
  double phInc;    //Phase increment (radians)
  unsigned int samplePeriod;    //how often to sample servos for pos (ms)

  //calibration (if we need it)
  int trim;     //add to position (degrees)
}
