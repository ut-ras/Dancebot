/* ESP32 Power Controller for Dancebot
 *
 * Implementation for checking battery level and going into modem power saving mode.
 * Espressif documentation for WiFi power saving mode:
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#esp32-wi-fi-power-saving-mode
 *
 * To see resistor divider and/or battery level checker, look for PCB schematic on Github
 * 
 */

#include <Arduino.h>
#include <esp_wifi.h>
#include <PowerController.h>

//pin mappings
//#define ADC_IN 27 //ADC pin 27 doesn't work when WiFi is running....
#define ADC_IN 33 //CHANGED FROM PCB SCH, pin 33 works w/ WiFi
#define BAT_EN 4
#define LATCH_OUT 5

//resistor divider
#define R2 100 
#define R3 51
#define VOLTAGE_MAX 8.5 //mV
#define VOLTAGE_MIN 6.5
#define VOUT(Vin) (((Vin) * R3) / (R2 + R3))

PowerController::PowerController(void){
    batteryPercentage = 100;
}

void PowerController::powerOnSystem(void){
    digitalWrite(LATCH_OUT, 1);
}

//TO DO: Test if power consumption decreases
void PowerController::startWiFiPowerSave(void){
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM); //sets to Modem-sleep mode
}

void PowerController::endWiFiPowerSave(void){
    esp_wifi_set_ps(WIFI_PS_NONE);  //disable modem-sleep mode
}

void PowerController::batteryADCInit(void){
    adcAttachPin(ADC_IN);
    pinMode(BAT_EN, OUTPUT);
    digitalWrite(BAT_EN, 0); //disable battery lvl circuit  CHANGE THIS BACK TO 0
}

float PowerController::calculateBatteryPercentage(void){
    digitalWrite(BAT_EN, 1); //enable battery lvl checker
    int adcValue = analogRead(ADC_IN);
    Serial.print("ADC value: "); Serial.println(adcValue);

    //ADC % -> Voltage [0, 3.3V] -> Battery Voltage {6.5, 8.4}
    batteryPercentage = ((adcValue * 3.3 * 1.1 * 3) / 4095) * 100;  //resistor divides by 3, 1.1 is scale factor to correct ADC reading
    batteryPercentage = batteryPercentage / VOLTAGE_MAX;
    //batteryPercentage = 100 * (adcValue - BATTERY_MIN_ADC) / (BATTERY_MAX_ADC - BATTERY_MIN_ADC);

    if(batteryPercentage < 0) batteryPercentage = 0;
    if(batteryPercentage > 100) batteryPercentage = 100;

    digitalWrite(BAT_EN, 0); //disable battery lvl checker
    Serial.print("Battery level: "); Serial.println(batteryPercentage);
    return batteryPercentage;
}
