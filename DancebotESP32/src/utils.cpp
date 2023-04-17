/**
 * @file utils.cpp
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Utility methods.
 * @version 0.2.0
 * @date 2023-04-14
 * @copyright Copyright (c) 2023
 *
 */
#include "utils.hpp"
#include <ArduinoLog.h>

void print_timestamp(Print *_log_output, int log_level) {
    char c[12];
    snprintf(c, 12, "%10lu ", millis());
    _log_output->print(c);
}

void print_newline(Print *_log_output, int log_level) {
    _log_output->print('\n');
}
