/* Oscillator.h
 * UT Austin RAS Demobots
 * Oscillate motors in sinusoidal motion
 * Basically just a wrapper class for the Arduino library servo class
 */

//this prevents errors if Oscillator.h is included in multiple files
#ifndef OSCILLATOR
  #define OSCILLATOR

#include <Servo.h>


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
  void setPos(int p);             //set Position (degrees)

  //control
  void stop();
  void start();

  //calibration (if we need it)
  void setTrim(int t);       //set Trim
  int getTrim();             //get Trim

private:
  //sinusoid functions
  bool checkRefreshTime();    //check if refresh time increment has passed

  //Arduino Servo object
  Servo servo;

  //sinusoid parameters
  unsigned int amp;     //Amplitude (degrees)
  unsigned int off;     //Offset (degrees)
  double ph0;           //Initial Phase (radians)
  unsigned int period;  //Period (ms)

  //servo status variables
  bool isStopped;
  int pos;        //Current Position (degrees)
  double ph;      //Current Phase (radians)
  double phInc;    //Phase increment (radians)
  unsigned int samplePeriod;    //how often to sample servos for pos (ms)

  //calibration (if we need it)
  int trim;     //add to position (degrees)
}
