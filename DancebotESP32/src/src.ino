/**
 * @file src.ino
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Entry point and main for dancebot swarm. Dance Dance Dance! https://youtu.be/ciEHtGpSviE
 * @version 0.2.0
 * @date 2023-04-21
 * @copyright Copyright (c) 2023
 *
 */
#include <Arduino.h>
#include <ArduinoLog.h>
#include "Dancebot.hpp"
#include "utils.hpp"
#include "SPIFFS.h"

Dancebot *dancebot;

void setup_logging(void) {
    Serial.println("Hello world! Initializing the logger...");
    Log.begin(LOG_LEVEL_TRACE, &Serial);
    Log.setPrefix(print_timestamp);
    Log.setSuffix(print_newline);
}

bool setup_SPIFFS(void) {
    Log.notice("Setting up SPIFFS and checking for necessary files.");
    if (!SPIFFS.begin()) {
        Log.notice("An error has occurred while mounting SPIFFS.");
        return false;
    }

    /* Check for html and js file that is to be served by the server if set up. */
    const char *html_path = "/test.html";
    File html = SPIFFS.open(html_path);
    if (!html) {
        Log.notice("%s does not exist.", html_path);
        return false;
    }

    const char *js_path = "/test.js";
    File js = SPIFFS.open(js_path);
    if (!js) {
        Log.notice("%s does not exist.", js_path);
        return false;
    }

    Log.notice("All relevant files found.");
    return true;
}

void setup(void) {
    Serial.begin(9600);
    while (!Serial);

    /** Give the user some time to open up the serial monitor. */
    delay(2500); // 2.5 seconds

    setup_logging();
    setup_SPIFFS();

    dancebot = new Dancebot(
        "mono_no_aware",
        Dancebot::WALK,
        Dancebot::UT,
        Dancebot::BLUE
    );

    Log.trace("Shutting down...");
    free(dancebot);
}

void loop(void) {}