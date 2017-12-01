/* Oscillator.h
 * UT Austin RAS Demobots
 * Oscillate non-continuous servos in sinusoidal motion
 * Basically just a wrapper  for servo.h
 */

//this prevents errors if Oscillator.h is included in multiple files
#ifndef OSCILLATOR
  #define OSCILLATOR
  #include <RASLib/inc/servo.h>
  #include <stdbool.h>
  #define PI 3.1415
#endif


//TODO create OscillatorTest, update these steps if needed

/* HOW TO USE OSCILLATOR CLASS
 * 1) create Oscillator object
 * 2) attach it to a pin
 * 3) set desired sinusoid parameters, calibrate and set trim if needed
 *         amplitude is basiaclly the degree of rotation for the oscillating movement
 * 4) run refreshPos() on the main loop
 * 5) enable oscillation with start()
 */

   typedef struct oscillator {
     long t_current;             //current time (ms)
     long t_lastRefresh;         //time of last refreshPos() (ms)

     //Servo object
     tServo* servo;

     //sinusoid parameters
     int amp;     //Amplitude (degrees)
     int off;     //Offset (degrees)
     double ph0;           //Initial Phase (radians)
     int period;           //Period (ms)
     int rev;              //Reverse Sinusoid multiplier (1 = no rev, -1 = rev)

     //servo status variables
     char isStopped;
     int pos;        //Current Position (degrees)
     double ph;      //Current Phase (radians)
     double phInc;    //Phase increment (radians)
     int samplePeriod;    //how often to sample servos for pos (ms)

     //calibration (if we need it)
     int trim;     //add to position (degrees)
   } oscillator;

  oscillator* InitializeOscillator(tPin pin);


  //sinusoid functions
  void refreshPos(oscillator* osc);              //set servo pos based on sinusoid
  //sinusoid parameters
  void setAmp(oscillator* osc, int a);              //set Amplitude (degrees)
  void setOff(oscillator* osc, int o);              //set Offset (degrees)
  void setPh0(oscillator* osc, double p0);          //set Initial Phase (radians)
  void setPer(oscillator* osc, int t);              //set Period (ms)
  void setRev(oscillator* osc, bool r);             //Set Reverse on/off (default off)

  //control
  void stopO(oscillator* osc);
  void startO(oscillator* osc);
  void setPos(oscillator* osc, int p);            //set Position (degrees)
  void resetPh(oscillator* osc);                  //set Current Phase to 0

  //calibration (if we need it, built into refreshPos, just call setTrim)
  void setTrim(oscillator* osc, int t);       //set Trim (degrees)
  int getTrim(oscillator* osc);             //get Trim (degrees)


  //sinusoid functions
  bool checkRefreshTime(oscillator* osc);    //check if refresh time increment has passed
