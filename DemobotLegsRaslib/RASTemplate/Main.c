#include <RASLib/inc/common.h>
#include <RASLib/inc/gpio.h>
#include <RASLib/inc/time.h>
#include "DancingServos.h"

// Blink the LED to show we're on
tBoolean blink_on = true;

void dancingServosTest();
void setup();
void loop();


void blink(void) {
    SetPin(PIN_F3, blink_on);
    blink_on = !blink_on;
}


// The 'main' function is the entry point of the program
int main(void) {
    // Initialization code can go here
    CallEvery(blink, 0, 0.5);
    setup();

    while (1) {
        loop();
    }
}



void setup() {
  InitializeDancingServos(PIN_D0, PIN_D1, PIN_D2, PIN_D3);
  setTrims(75, 130, 45, 45);
}

void loop() {
  position0();
  WaitUS(1000);

  //dance moves here
  demo1();
  demo2();
  //add more moves
}


//TEST FUNCTIONS

void dancingServosTest() {
  position0();
  WaitUS(500);
  themAnkles(5);
  WaitUS(500);
  position0();
  while(true);
}
