#pragma once

//--------------- Parameters ---------------//'
#define SENSOR0_PIN 19
#define SENSOR1_PIN 20
#define SENSOR2_PIN 21
#define SENSOR3_PIN 22

#define SENSORBOSS_PIN 23
#define SENSORCAR_PIN 24

// 4 Stationary + 2 Moving
#define NUM_SENSORS 6
#define STATIONARY_ZOMBIES 4
#define COOLDOWN 7000
#define MOVING_COOLDOWN 3000
#define GAME_TIME 120000
#define SERVOMIN 150   // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600   // This is the 'maximum' pulse length count (out of 4096)
#define USMIN 600      // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX 2400     // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz updates
#define ZOMBIE_DOWN 0
#define ZOMBIE_UP 1

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