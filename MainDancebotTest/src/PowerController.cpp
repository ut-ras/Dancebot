

#include <Arduino.h>
#include "PowerController.h"

//pin mappings
#define ADC_IN 27
#define BAT_EN 4
#define LATCH_OUT 5

    void PowerController::powerOnSystem(void){
        digitalWrite(LATCH_OUT, 1);
    }

    
    void PowerController::sleepSystem(void){
        
    }

    void PowerController::sleepSystem(int period){

    }

    void PowerController::awakeSystem(void){

    }
    
    int PowerController::getBatteryPercentage(void){

    }
