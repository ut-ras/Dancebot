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
#define ADC_IN 27
#define BAT_EN 4
#define LATCH_OUT 5

//resistor divider
#define R2 100 
#define R3 51
#define VOLTAGE_MAX 8500 //mV
#define VOLTAGE_MIN 5000
#define VOUT(Vin) (((Vin) * R3) / (R2 + R3))

//ADC values
#define ADC_REFERENCE 1100 //mV, from ESP32 docs
#define VOLTAGE_TO_ADC(in) ((ADC_REFERENCE * (in)) / 4096)
#define BATTERY_MAX_ADC VOLTAGE_TO_ADC(VOUT(VOLTAGE_MAX))
#define BATTERY_MIN_ADC VOLTAGE_TO_ADC(VOUT(VOLTAGE_MIN))

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
    digitalWrite(BAT_EN, 0); //disable battery lvl circuit
}

int PowerController::calculateBatteryPercentage(void){
    digitalWrite(BAT_EN, 1); //enable battery lvl checker

    int adcValue = analogRead(ADC_IN);
    batteryPercentage = 100 * (adcValue - BATTERY_MIN_ADC) / (BATTERY_MAX_ADC - BATTERY_MIN_ADC);

    if(batteryPercentage < 0) batteryPercentage = 0;
    if(batteryPercentage > 100) batteryPercentage = 100;

    digitalWrite(BAT_EN, 0); //disbale battery lvl checker
    return batteryPercentage;
}
