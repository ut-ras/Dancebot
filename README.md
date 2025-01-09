# Mothership

This branch contains the code for the mothership.


Outdated comments:

This documentation covers how to connect a **PS4** controller to the ESP32, the ESP32 to the motor driver, and the driver to the moters.
// PLEASE NOTE THAT HARDWARE ON LEFT JOYSTICK FOR REMOTE WAS BROKEN

ESP32 -> Driver:
- Original motor drivers broke (Region 5 Competition drivers), so we are using Adafruit Motor Shield V2.3 connected to ESP32 Wroom (Pico-D4 not compatible with PS4 library)
- Wire I2C ESP32 pins to respective SDA and SCL pins on driver, ground to ground, and 5v to 5v (4 connections)

ESP32   |   Driver
  SCL   |    SCL (GPIO22)
  SDA   |    SDA (GPIO21)
  5v    |    5v
  GND   |    GND

Controller Bluetooth (BT) -> ESP32:
PLEASE NOTE THAT ESP32FLASH MEMORY MAY NEED TO BE WIPED IF BLUETOOTH DOES NOT CONNECT (see https://razndomnerdtutorials.com/esp32-erase-flash-memory/)
- It seems you need to wipe the ESP32 flash if you connect the remote to another device (i.e. laptop) because something something BT client keys gone (it just works)
- PS4 remote MUST be configured to search for MAC address of ESP32 through external application "Sixaxis Pair Tool"
- Specific MAC address pairs that worked for me (notice how ESP32's last octet is 2 less than the remote's):
  - ESP32 address the one printed through WiFi library
  - Remote address is one in SixAxis Pair Tool
    |  ESP32:  c8:f0:9e:f1:da:28
    |  Remote: c8:f0:9e:f1:da:2a
    |
    |  ESP32:  c8:f0:9e:f1:da:24
    |  Remote: c8:f0:9e:f1:da:26
- Look into WiFi libraries to print and edit (?) ESP32 MAC address if necessary :D


Driver -> Motors:
- only motor wires connected are RED and BLACK
- when looking at motor driver with long set of header pins and protoboard holes at the top, 
  the RED wires are always on top of the BLACK wires

 !!!RED and BLACK wires for M2 and M4 are swapped!!!

        FRONT
  -------------------                
O |  M3         M2  |O
  |                 |
  |                 |
O |  M4         M1  |O
   ------------------
        BACK

Motor pinout:
- we are only using RED and BLACK because we don't need precise motion

white - ENC BOUT
yellow - ENC AOUT
blue - ENC VCC
green - ENC GND
BLACK - PWR B
RED - PWR A

*/
