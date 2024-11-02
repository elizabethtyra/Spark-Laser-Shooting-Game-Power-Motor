#pragma once

//--------------- Parameters ---------------//'
#define SENSOR0_PIN 22
#define SENSOR1_PIN 23
#define SENSOR2_PIN 24
#define SENSOR3_PIN 25

#define SENSORBOSS_PIN 26
#define SENSORBOSS_PIN2 27
#define SENSORCAR_PIN 28
#define SENSORCAR_PIN2 29

// 4 Stationary + 2 Moving
#define NUM_SENSORS 8
#define NUM_SERVOS 5 // Change to 6 once we add car servo
#define STATIONARY_ZOMBIES 4
#define COOLDOWN 50 // tenths of a second
#define COOLDOWN2 40
#define MOVING_COOLDOWN 30
#define GAME_TIME 1200
#define SERVOMIN 150   // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600   // This is the 'maximum' pulse length count (out of 4096)
#define USMIN 600      // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX 2400     // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz updates
#define ZOMBIE_DOWN 0
#define ZOMBIE_UP 1
#define BOSS_ZOMBIE 4
#define CAR_ZOMBIE 5

/**************** STEPPER PARAMETERS ***************/
// Define stuff for both stepper motors
extern const int stepsPerRevolution; 
extern const int stopDelay; // How long to wait once reached the end (to handle lives and stuff)
extern const int stepDelay_high_low; // Delay for clockwise rotation
extern const int stepDelay_low_high; // Delay for counterclockwise rotation
extern unsigned long delayNum;

// Define stuff for only STEPPER1 (Boss Zombie)
extern const int stepPinBoss;
extern const int dirPinBoss;
extern const int limitSwitchBoss_1; // Limit switch for stepper 1 in one direction
extern const int limitSwitchBoss_2; // Limit switch for stepper 1 in the other direction

// Define stuff for only STEPPER2 (Car Zombie)
extern const int stepPinCar;
extern const int dirPinCar;
extern const int limitSwitchCar_1; // Limit switch for stepper 2 in one direction
extern const int limitSwitchCar_2; // Limit switch for stepper 2 in the other direction

// int currentHigh;
// int currentLow;
// int previousHigh;
// int previousLow; 

extern int currentStateBoss_1;
extern int previousStateBoss_1;
extern int currentStateBoss_2;
extern int previousStateBoss_2;
extern int currentStateCar_1;
extern int previousStateCar_1;
extern int currentStateCar_2;
extern int previousStateCar_2;