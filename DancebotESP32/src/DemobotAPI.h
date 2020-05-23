/**
 * Author: Matthew Yu
 * Last Modified: 05/22/20
 * Project: Dancebot Swarm
 * File: DancebotAPI.h
 * Description: Dancebot API to drive hardware with using a state structure
 * Organization: UT IEEE RAS
 */
#ifndef DEMOBOTAPI_H
#define DEMOBOTAPI_H

#include "DancingServos.h"
#include "RobotConfig.h"

void initializeRobot();
void move();
void updateState(String state);
bool isOscillating();

#endif