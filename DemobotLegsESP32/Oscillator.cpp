//Oscillator.cpp
//UT Austin RAS Demobots

#include <math.h>
#include <Arduino.h>
  
#include "Oscillator.h"

//#define PI 3.14159265358979323846


Oscillator::Oscillator() {
  //init helper variables
  this->isStopped = true;
  this->servoAttached = false;
  this->t_lastRefresh = 0;
  this->ph = 0;

  //default sinusoid values
  samplePeriod = 50;
  this->setAmp(30);
  this->setOff(0);
  this->setPh0(0);
  this->setPer(2500);
  this->setRev(false);

  servo = new Servo();
  servo->setPeriodHertz(50);
}

//not currently being directly used
Oscillator::Oscillator( int a,  int o, double p0,  int t, bool r) {
  //init helper variables
  this->isStopped = true;
  this->servoAttached = false;
  this->t_lastRefresh = 0;
  this->ph = 0;

  //default sinusoid values
  samplePeriod = 30;
  this->setAmp(a);
  this->setOff(o);
  this->setPh0(p0);
  this->setPer(t);
  this->setRev(r);

  servo = new Servo();
  servo->setPeriodHertz(50);
}



//SINUSOID FUNCTIONS
//set servo pos based on sinusoid
void Oscillator::refreshPos() {
  if (this->servoAttached && this->checkRefreshTime()) {
    //Serial.print("phInc: " + String(this->phInc));
    if (!this->isStopped) {
        //if the refresh time is complete and the motor is not stopped
        //calculate the current pos in the sinusoid
        int newPos = this->rev * round(double(this->amp) * double(sin(this->ph + this->ph0)) + double(this->off));
        this->setPos(newPos);
    }
    this->ph += this->phInc;    //increment the phase
    if (this->ph > 2 * PI) {this->resetPh();}
  }
}
//check if refresh time increment has passed
bool Oscillator::checkRefreshTime() {
  //check if samplePeriod (ms) has passed since last checkRefreshTime() call
  this->t_current = millis();
  if (this->t_current - this->t_lastRefresh > this->samplePeriod) {
    this->t_lastRefresh = this->t_current;
    return true;
  }
  return false;
}


//SERVO SETUP
void Oscillator::attach(int pin) {
  servo->detach();
  servo->attach(pin, MIN_US, MAX_US);
  this->servoAttached = true;
}
void Oscillator::detach() {
  if (servo->attached()) {
    servo->detach();
    this->servoAttached = false;
  }
}


//SINUSOID PARAMETERS
//set Amplitude (degrees)
void Oscillator::setAmp( int a) {this->amp = a;}
//set Offset (degrees)
void Oscillator::setOff( int o) { this->off = o;}
//set Initial Phase (radians)
void Oscillator::setPh0(double p0) {this->ph0 = p0;}
//set Period (ms)
void Oscillator::setPer( int t) {
  this->period = t;
  double n = double(this->period) / double(this->samplePeriod);  //n = number of samples
  this->phInc = (2.0 * PI) / n;
  //Serial.print("n: " + String(n) + " phInc: " + String(this->phInc));
}
//Set Sinusoid Reverse on/off (default off) (sin reverse not servo reverse)
void Oscillator::setRev(bool r) {
  if (r) {this->rev = -1;}
  else {this->rev = 1;}
}


//CONTROL
void Oscillator::stopO() {this->isStopped = true;}
void Oscillator::startO() {this->isStopped = false;}
//set Position (degrees)
void Oscillator::setPos(int p) {
  this->pos = p;
  servo->write(p + this->trim);
  //Serial.print("pos: " + String(p) + " ph: " + String(this->ph) + " phInc: " + String(this->phInc) + "amp: " + String(this->amp) + "\n");
}
int Oscillator::getPos() {
  //this only reads the most recent data sent to it -- wont work until data has been written
  int angle = servo->read();
  return angle;
}
//set Current Phase to 0
void Oscillator::resetPh() {
  this->ph = 0;
}

//CALIBRATION
void Oscillator::setTrim(int t) {this->trim = t;}
int Oscillator::getTrim() {return this->trim;}



//TEST FUNCTIONS
void servoTest(int angle) {
  Oscillator osc;
  osc.attach(12);
  osc.startO();
  osc.setAmp(40);
  osc.setOff(50);
  osc.setPer(2500);
  delay(2000);
  int x = osc.getPos();
  Serial.print(String(x));
  osc.setPos(angle);
  osc.stopO();
  while(true);
}

void oscillatorTest() {
  Oscillator osc;
  osc.attach(12);
  osc.startO();
  long period = 2500;
  long cycles = 5;
  long t0 = millis();
  for (long t = t0; t < (period * cycles + t0); t = millis()) {
    delay(30);
    osc.refreshPos();
    yield();
  }
  osc.stopO();
  while(true);
}
