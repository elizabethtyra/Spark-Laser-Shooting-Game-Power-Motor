#pragma once

#include "parameters.h"

extern int playerScore;
extern int playerLives;
extern unsigned long lastHit[NUM_ZOMBIES];
extern unsigned long zombieState[NUM_ZOMBIES];

void gameOver();
void processHit(const char* sensor, unsigned long* lastTime, int zombieNum, bool moving);
void printSensorState(int sensorPin);