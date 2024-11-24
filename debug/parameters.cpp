#include "parameters.h"

/**************** STEPPER PARAMETERS ***************/
// Define stuff for both stepper motors
const int stepsPerRevolution = 1000; 
const int stopDelay = 10; // How long to wait once reached the end (to handle lives and stuff)
const int stepDelay_high_low = 2500; // Delay for clockwise rotation
const int stepDelay_low_high = 2500; // Delay for counterclockwise rotation
unsigned long delayNum = 400;

// Define stuff for only STEPPER1 (Boss Zombie)
const int stepPinBoss = 4;
const int dirPinBoss = 5;
const int limitSwitchBoss_1 = 6; // Limit switch for stepper 1 in one direction
const int limitSwitchBoss_2 = 7; // Limit switch for stepper 1 in the other direction

// Define stuff for only STEPPER2 (Car Zombie)
const int stepPinCar = 2;
const int dirPinCar = 3;
const int limitSwitchCar_1 = 8; // Limit switch for stepper 2 in one direction
const int limitSwitchCar_2 = 9; // Limit switch for stepper 2 in the other direction

int currentStateBoss_1 = 0;
int previousStateBoss_1 = 0;
int currentStateBoss_2 = 0;
int previousStateBoss_2 = 0;
int currentStateCar_1 = 0;
int previousStateCar_1 = 0;
int currentStateCar_2 = 0;
int previousStateCar_2 = 0;
int previousResetState = 0;
int currentResetState = 0;
int gameStart = false; // 0
int timerStart = ('NUL' - 'NUL')*0;
