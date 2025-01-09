/* PowerController.h
 * UT Austin RAS Demobots
 * 
 *
 *
*/

#ifndef DANCINGSERVOS
#define DANCINGSERVOS

class PowerController {
public:
    //when ESP32 turns on, latch the power to continue being on (refer to sch)
    void powerOnSystem(void);

    void startWiFiPowerSave(void);
    void endWiFiPowerSave(void);

    void batteryADCInit(void);
    //set and get battery % between 0 - 100
    int calculateBatteryPercentage(void); 
    

private:
    //0 - 100 %
    int batteryPercentage;

};

#endif