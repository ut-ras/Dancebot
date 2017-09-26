//Oscillator.cpp
//UT Austin RAS Demobots

#ifndef OSCILLATOR
  #include "Oscillator.h"
  #include <math.h>
  #include <Arduino.h>
#endif

Oscillator::Oscillator() {
  Oscillator(40, 0, 0, 2500, false);
}

Oscillator::Oscillator(unsigned int a, unsigned int o, double p0, unsigned int t, bool r) {
  //init helper variables
  isStopped = true;
  servoAttached = false;
  t_lastRefresh = 0;
  ph = 0;

  //default sinusoid values
  samplePeriod = 30;
  setAmp(a);
  setOff(o);
  setPh0(p0);
  setPeriod(t);
  setRev(r);
  setTrim(0);       //can use setTrim() to calibrate if needed
}



//SINUSOID FUNCTIONS
//set servo pos based on sinusoid
void Oscillator::refreshPos() {
  if (servoAttached && checkRefreshTime()) {
    if (!isStopped) {
        //if the refresh time is complete and the motor is not stopped
        //calculate the current pos in the sinusoid
        pos = rev * round(amp * sin(ph + ph0) + off);
        servo.write(pos + trim);
    }
    ph += phInc;    //increment the phase
  }
}
//check if refresh time increment has passed
bool Oscillator::checkRefreshTime() {
  //check if samplePeriod (ms) has passed since last checkRefreshTime() call
  t_current = millis();
  if (t_current - t_lastRefresh > samplePeriod) {
    t_lastRefresh = t_current;
    return true;
  }
  return false;
}


//SERVO SETUP
void Oscillator::attach(int pin) {
  servo.detach();
  servo.attach(pin);
  servoAttached = true;
}
void Oscillator::detach() {
  if (servo.attached()) {
    servo.detach();
    servoAttached = false;
  }
}


//SINUSOID PARAMETERS
//set Amplitude (degrees)
void Oscillator::setAmp(unsigned int a) {amp = a;}
//set Offset (degrees)
void Oscillator::setOff(unsigned int o) { off = o;}
//set Initial Phase (radians)
void Oscillator::setPh0(double p0) {ph0 = p0;}
//set Period (ms)
void Oscillator::setPeriod(unsigned int t) {
  period = t;
  int n = period / samplePeriod;  //n = number of samples
  phInc = 2 * PI / n;
}
//Set Sinusoid Reverse on/off (default off) (sin reverse not servo reverse)
void Oscillator::setRev(bool r) {
  if (r) {rev = -1;}
  else {rev = 1;}
}


//CONTROL
void Oscillator::stop() {isStopped = true;}
void Oscillator::start() {isStopped = false;}
//set Position (degrees)
void Oscillator::setPos(int p) {
  pos = p;
  servo.write(pos + trim);
}
//set Current Phase to 0
void Oscillator::resetPh() {
  ph = 0;
}

//CALIBRATION (if we need it)
void Oscillator::setTrim(int t) {trim = t;}
int Oscillator::getTrim() {return trim;}
