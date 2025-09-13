#pragma once

//--------------- Libraries ---------------//'
// timer interrupt libraries
#include <avr/io.h>
#include <avr/interrupt.h>

// 7 segment libraries
#include <Wire.h>
#include <Adafruit_LEDBackpack.h>

// access Arduino memory for leaderboard
#include <EEPROM.h> 

// dot matrix libraries
#include <MD_MAX72xx.h>
#include <SPI.h>

//--------------- Dot matrix lives displays ---------------//
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES 3

//Hardware SPI pins for Arduino UNO, change for Arduino Mega 
// CLK Pin  > 13 SCK
// Data Pin > 11 MOSI
#define CS_PIN 53
#define DATA_PIN 51 
#define CLK_PIN 52

extern MD_MAX72XX livesMatrix;

extern byte heart_fill[8];
extern byte heart_empty[8];

//--------------- 7 segment timer, score, and leaderboard displays ---------------//
extern Adafruit_7segment timerMatrix; // timer
extern Adafruit_7segment userScore; // leaderboard 1st place
extern Adafruit_7segment topScore1; // leaderboard 2nd place
extern Adafruit_7segment topScore2; // leaderboard 3rd place
extern Adafruit_7segment topScore3; // current score

// timer variables
extern volatile uint16_t timer_count;
extern volatile int interrupt_counter;

// globals for leaderboard
extern int highScores[4];
extern uint8_t topScoreLowByte;
extern uint8_t topScoreHighByte;

void update_timer();
void update_7seg(int score, Adafruit_7segment matrix);

// leaderboard
int readEEPROMTopScore(int position);
void writeTopScoreToEEPROM(int position, int score);

// dot matrix lives 
void drawInitialLives();
void updateLives(int lives);
