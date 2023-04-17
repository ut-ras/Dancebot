# DemobotESPNetcode

Netcode for setting up arbitrary networks, servers, and clients for Demobot
robots running on the ESP32 platform.

## Prerequisites

The minimum libraries required to run the examples provided are the following:

- [arduino-esp32](https://github.com/espressif/arduino-esp32)
- [asyncHTTPrequest](https://github.com/boblemaire/asyncHTTPrequest)
- [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)

This code has been tested (in so far that the examples work, but no unit tests
or stress tests in the pipeline as of now) solely with the ESP32. Potentially,
parts of the code can work with the ESP8266. The AsyncTCP library may need to be
replaced with the ESPAsyncTCP library.

## Usage

This library will be by default, bundled with the DemobotESPTemplate repository
for IEEE RAS's noncompetitive Demobot committee (and anyone else who is
interested in making their own IoT device or robot with the ESP32). Further
usage instructions may be found in that repo.

In particular, the build system used is plerup's
[makeEspArduino](https://github.com/plerup/makeEspArduino); you can follow his
instructions or the curated set of instructions I've provided in the
[DEVELOPMENT.md](https://github.com/ut-ras/Dancebot/blob/feature/Dancebot/DEVELOPMENT.md)
markdown file.