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

    //sleeping functions
    void sleepSystem(void);
    void sleepSystem(int period);
    void awakeSystem(void);

    //set and get battery % between 0 - 100
    int getBatteryPercentage(void); 
    

private:
    //0 - 100 %
    int batteryPercentage;

};

#endif