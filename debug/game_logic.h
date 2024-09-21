#pragma once

#include "parameters.h"

extern int playerScore;
extern int playerLives;
extern unsigned long lastHit[NUM_SENSORS];
extern unsigned long zombieState[NUM_SENSORS];
extern unsigned long startTime;

void gameOver();
void processHit(const char* sensor, unsigned long* lastTime, int zombieNum, bool moving);
void printSensorState(int sensorPin);