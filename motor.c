#include <stdio.h>
#include "gpio.h"
#include <StellarisWare/driverlib/gpio.h>
#include <StellarisWare/driverlib/sysctl.h>

void init() {
	  SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
	//GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
}

void spinMotor() {
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 1);	
}

void stopMotor() {
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2, 0);
}

int main() {
    	init();
	spinMotor();
	SysCtlDelay(5000);
	stopMotor();
	return 0;
}
