#include "displays.h"

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
MD_MAX72XX livesMatrix = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

byte heart_fill[8] = {0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18};
byte heart_empty[8] = {0x00, 0x66, 0x99, 0x81, 0x81, 0x42, 0x24, 0x18};

//--------------- 7 segment timer, score, and leaderboard displays ---------------//
Adafruit_7segment timerMatrix = Adafruit_7segment(); // timer
Adafruit_7segment userScore = Adafruit_7segment(); // leaderboard 1st place
Adafruit_7segment topScore1 = Adafruit_7segment(); // leaderboard 2nd place
Adafruit_7segment topScore2 = Adafruit_7segment(); // leaderboard 3rd place
Adafruit_7segment topScore3 = Adafruit_7segment(); // current score

volatile uint16_t timer_count = 0;
volatile int interrupt_counter = 0;

int highScores[3] = {0};

uint8_t topScoreLowByte;
uint8_t topScoreHighByte;

// interrupt service routine for timer2, motor uses timer1
ISR(TIMER2_COMPA_vect) {
  sei();  
  if (interrupt_counter < 10)
  {
    interrupt_counter++;
  }
  else{
    timer_count++;
  update_timer();
  interrupt_counter = 0;
  }
}

void update_timer() {
  timerMatrix.writeDigitNum(0, (timer_count / 1000));
  timerMatrix.writeDigitNum(1, (timer_count / 100) % 10);
  timerMatrix.writeDigitNum(3, (timer_count / 10) % 10, true);
  timerMatrix.writeDigitNum(4, timer_count % 10);
 
  timerMatrix.writeDisplay();
}

void update_7seg(int score, Adafruit_7segment matrix) {
  matrix.writeDigitNum(0, (score / 1000));
  matrix.writeDigitNum(1, (score / 100) % 10);
  matrix.writeDigitNum(3, (score / 10) % 10);
  matrix.writeDigitNum(4, score % 10);
 
  matrix.writeDisplay();
}

// read top player high score from EEPROM
int readEEPROMTopScore(int position) {
  //read from EEPROM
  if(position == 1) {
    topScoreLowByte = EEPROM.read(0);
    topScoreHighByte = EEPROM.read(1);
  }
  //read from EEPROM
  else if(position == 2) {
    topScoreLowByte = EEPROM.read(2);
    topScoreHighByte = EEPROM.read(3);
  }
  //read from EEPROM
  else if(position == 3) {
    topScoreLowByte = EEPROM.read(4);
    topScoreHighByte = EEPROM.read(5);
  }

  return topScoreLowByte + (topScoreHighByte << 8);
}

// write top player high score to EEPROM
void writeTopScoreToEEPROM(int position, int score) {
  // Put topHighScore (16 bits?) into two 8-bit variables and then store into memory
  topScoreLowByte = (uint8_t)score;
  topScoreHighByte = (uint8_t)(score >> 8);

  if(position == 1) {
    EEPROM.put(0, topScoreLowByte);
    EEPROM.put(1, topScoreHighByte);
  }
  else if(position == 2) {
    EEPROM.put(2, topScoreLowByte);
    EEPROM.put(3, topScoreHighByte);
  }
  else if(position == 3) {
    EEPROM.put(4, topScoreLowByte);
    EEPROM.put(5, topScoreHighByte);
  }
}

// inital dot matrix lives 
void drawInitialLives() {
  for (int i = 0; i < 8; i++) {
    livesMatrix.setRow(0, 0, i, heart_fill[i]);
  }

  for (int i = 0; i < 8; i++) {
    livesMatrix.setRow(1, 1, i, heart_fill[i]);
  }

  for (int i = 0; i < 8; i++) {
    livesMatrix.setRow(2, 2, i, heart_fill[i]); 
  }
}

