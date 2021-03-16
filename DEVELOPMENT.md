# Development

The current iteration of this project uses the ESP32-Pico-Kit, a small
microcontroller built on the Arduino platform with WiFi capability.

Previously, this robots was compiled and flashed using the Arduino IDE (and
still can, probably). As of now, we are using the [makeEspArduino
project](https://github.com/plerup/makeEspArduino).

---

## Installing and Building

Below are instructions for each OS (if provided) for building the project and
flashing it to your device.

---

### Linux

#### Setup

```bash
#1. Install the Dancebot repo.
cd <PATH> # for example, ~/Github/RAS/
git clone https://github.com/ut-ras/Dancebot.git

#2. Install required python dependencies.
cd <PATH>/Dancebot/
pip3 install -r requirements.txt

#3. Clone the makeEspArduino repo and ESP32 library somewhere near.
cd <PATH>
git clone https://github.com/plerup/makeEspArduino.git
git clone https://github.com/espressif/arduino-esp32.git esp32

#4. Install the ESP environmental tools.
cd <PATH>/esp32
git submodule update --init
cd <PATH>/esp32/tools
python3 get.py
```

#### Demo Test

```bash
# The next set of commands tests whether you can flash to the board successfully.
cd <PATH>/makeEspArduino
make -f makeEspArduino.mk ESP_ROOT=<PATH>/esp32 CHIP=esp32 DEMO=1 flash

# Then try connecting to the device after seeing it hard reset with the screen command:
screen /dev/ttyUSB0 115200 # Your port may be different from /dev/ttyUSB0.

# You should see the results of the example WiFiScan.ino sketch, which can be
# found here: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiScan/WiFiScan.ino.

# To exit the screen utility, type ctrl+a, and then `:exit` and press enter.
```

#### Setting Up the espmake32 Command

```bash
# Now we can alias a make command for compiling and flashing binaries on demand.
cd <PATH>/makeEspArduino
# A suggestion for the path is to use the absolute path instead of relative
# (i.e., ../). This will allow you to be able to call this command anywhere.
make -f makeEspArduino.mk ESP_ROOT=<PATH>/esp32 CHIP=esp32 install

# Run the compiling command on one of their examples.
espmake32 SKETCH="<PATH>/esp32/libraries/WiFi/examples/WiFiScan/WiFiScan.ino"

# This compiles the .ino into an executable and stashes it in a place like
# `/tmp/mkESP/WiFiScan_esp32/WiFiScan.bin`.
# To flash, run the same command but with the flash suffix like follows:
espmake32 SKETCH="<PATH>/libraries/WiFi/examples/WiFiScan/WiFiScan.ino" flash

# Try connecting again with the screen command after the ESP flashes
screen /dev/ttyUSB0 115200 # Your port may be different from /dev/ttyUSB0.

# An alternative is to use the command espmake32 monitor.
espmake32 monitor # ctrl+] to exit.

# If it works, you should be set up for building your ino sketches natively!
```

#### Building the Dancebot Repository

```bash
# Currently, we can only build the repo from esp32 folder. Once I figure out how
# to use the command with the proper boards.txt reference, this will be updated.
cd <PATH>/Dancebot
espmake32 SKETCH="./src/DancebotESP32.ino" LIBS="./inc/" BUILD_DIR="./build/"

# You should be able to see the output of the build files in the build directory
# for the Dancebot project.
```

#### Using the Built-in Script

The previous steps described above are a helpful primer on what goes on when
setting up the build tools and environment.

In the spirit of the Automation in R(A)S, I've provided a build script for once
you complete step 'Building the Dancebot Repository' for subsequent builds.

```bash
bash run.sh -h # Provides help information.
bash run.sh -c # Compiles the project and puts build artifacts into /inc/.
bash run.sh -f # Compiles and flashes the project to the ESP32 if it is connected.
bash run.sh -m # Opens the serial monitor in the CLI if the ESP32 is connected.
```