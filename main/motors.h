#pragma once

//--------------- Libraries ---------------//'
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

extern Adafruit_PWMServoDriver pwm;
extern uint8_t servonum;

void lowerZombie(int zombieNum);
void raiseZombie(int zombieNum);
void setServoPulse(uint8_t n, double pulse);
