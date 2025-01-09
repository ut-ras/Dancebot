//Oscillator.c
//UT Austin RAS Demobots



#ifndef OSCILLATOR
  #include "Oscillator.h"
  //#define PI 3.14159265358979323846
#endif


int samplePeriod;

oscillator* InitializeOscillator(tPin pin) {

  oscillator* osc;
  //init helper variables
  osc->isStopped = 1;
  osc->t_lastRefresh = 0;
  osc->ph = 0;

  //default sinusoid values
  samplePeriod = 50;
  osc->amp = 30;
  osc->off = 0;
  osc->ph0 = 0;
  osc->period = 2500;
  osc->rev = 0;
  osc->trim = 0;

  osc->servo = InitializeServo(pin);

  return osc;
}



//SINUSOID FUNCTIONS
//set servo pos based on sinusoid
void refreshPos(oscillator* osc) {
  if (checkRefreshTime(osc)) {
    //Serial.print("phInc: " + String(this->phInc));
    if (!osc->isStopped) {
        //if the refresh time is complete and the motor is not stopped
        //calculate the current pos in the sinusoid
        int newPos = osc->rev * (int)((double)osc->amp * (double)sin(osc->ph + osc->ph0) + (double)osc->off);
        osc->pos = newPos;
        setPos(osc, newPos);
    }
    osc->ph += osc->phInc;    //increment the phase
    if (osc->ph > 2 * PI) {resetPh(osc);}
  }
}
//check if refresh time increment has passed
bool checkRefreshTime(oscillator* osc) {
  //check if samplePeriod (ms) has passed since last checkRefreshTime() call
  osc->t_current = GetTimeUS() * 1000;
  if (osc->t_current - osc->t_lastRefresh > osc->samplePeriod) {
    osc->t_lastRefresh = osc->t_current;
    return true;
  }
  return false;
}



//SINUSOID PARAMETERS
//set Amplitude (degrees)
void setAmp(oscillator* osc, int a) {osc->amp = a;}
//set Offset (degrees)
void setOff(oscillator* osc, int o) { osc->off = o;}
//set Initial Phase (radians)
void setPh0(oscillator* osc, double p0) {osc->ph0 = p0;}
//set Period (ms)
void setPer(oscillator* osc, int t) {
  osc->period = t;
  double n = (double)osc->period / (double)osc->samplePeriod;  //n = number of samples
  osc->phInc = (2.0 * PI) / n;
  //Serial.print("n: " + String(n) + " phInc: " + String(this->phInc));
}
//Set Sinusoid Reverse on/off (default off) (sin reverse not servo reverse)
void setRev(oscillator* osc, bool r) {
  if (r) {osc->rev = -1;}
  else {osc->rev = 1;}
}


//CONTROL
void stopO(oscillator* osc) {osc->isStopped = true;}
void startO(oscillator* osc) {osc->isStopped = false;}
//set Position (degrees)
void setPos(oscillator* osc, int p) {
  osc->pos = p;
  float newVal =  (float)(p + osc->trim) / (float)(255.0);
  SetServo(osc->servo, newVal);
  //Serial.print("pos: " + String(p) + " ph: " + String(this->ph) + " phInc: " + String(this->phInc) + "amp: " + String(this->amp) + "\n");
}

//set Current Phase to 0
void resetPh(oscillator* osc) {
  osc->ph = 0;
}

//CALIBRATION (if we need it)
void setTrim(oscillator* osc, int t) {osc->trim = t;}
int getTrim(oscillator* osc) {return osc->trim;}


//Math
/*
int round(double x) {
  int xInt = (int)x;
  double frac = x - xInt;
  if (frac >= 0.5) {
    xInt += 1;
  }
  return xInt;
}*/
