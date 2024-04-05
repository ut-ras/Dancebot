/* PowerController.h
 * UT Austin RAS Demobots
 * 
 *
 *
*/

#ifndef POWERCONTROLLER
#define POWERCONTROLLER

class PowerController {
public:
    PowerController(void);
    //when ESP32 turns on, latch the power to continue being on (refer to sch)
    void powerOnSystem(void);

    void startWiFiPowerSave(void);
    void endWiFiPowerSave(void);

    void batteryADCInit(void);
    //set and get battery % between 0 - 100
    float calculateBatteryPercentage(void); 
    

private:
    //0 - 100 %
    float batteryPercentage;

};

#endif