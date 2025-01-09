/**
 * @file utils.hpp
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Utility methods.
 * @version 0.2.0
 * @date 2023-04-14
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <Arduino.h>

void print_timestamp(Print *_log_output, int log_level);
void print_newline(Print *_log_output, int log_level);