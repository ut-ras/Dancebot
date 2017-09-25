//Oscillator.cpp
//UT Austin RAS Demobots

#include "Arduino.h"

#ifndef OSCILLATOR
  #include "Oscillator.h"


Oscillator::Oscillator() {
  isStopped = false;
  t_lastRefresh = 0;
}


//SERVO SETUP
void Oscillator::attach(int pin) {
  servo.detach();
  servo.attach(pin);
}
void Oscillator::detach() {
  if (servo.attatched()) {
    servo.detach();
  }
}


//SINUSOID FUNCTIONS
//set servo pos based on sinusoid
void Oscillator::refreshPos() {
  //TODO
  //sample a sin function
  //check current servo position
  //update variables

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


//SINUSOID PARAMETERS
//set Amplitude (degrees)
void Oscillator::setAmp(unsigned int a) {amp = a;}
//set Offset (degrees)
void Oscillator::setOff(unsigned int o) { off = o;}
//set Initial Phase (radians)
void Oscillator::setPh0(double p0) {ph0 = p0;}
//set Position (degrees)
void Oscillator::setPos(int p) {pos = p;}


//CONTROL
void Oscillator::stop() {
  isStopped = true;
}
void Oscillator::start() {
  isStopped = false;
}


//CALIBRATION (if we need it)
void Oscillator::setTrim(int t) {
  trim = t;
}
int Oscillator::getTrim() {
  return trim;
}
