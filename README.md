# DemobotLegs
Dancing Bot

## Oscillator
Wrapper for the Arduino Servo.h class that inputs sinusoidal parameters instead of a position. Periodically samples the desired sin wave to update the position of the servo. Simple sinusoidal oscillation. 

## DancingServos
Wrapper for four Oscillators, representing a set of legs comprised of four servos. Contains a function that passes sinusoid parameters to each of the four Oscillators. A dance move calls this function with different sin waves on each motor. 

## Microcontrollers
### Teensy 2.0++
Arduino Ide with Teensyduino. Powered controller and servos with 4 AA batteries.
### TI LaunchPad
Energia with Drivers for LaunchPad. Use a 5v regulator when powering the LaunchPad.
