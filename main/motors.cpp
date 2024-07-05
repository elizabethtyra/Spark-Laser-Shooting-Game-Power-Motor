#include "motors.h"
#include "parameters.h"

//--------------- Libraries ---------------//'
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
uint8_t servonum = 12;

void lowerZombie(int zombieNum) {
  Serial.println("Lowering zombie ");
  Serial.println(zombieNum);
  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
    pwm.setPWM(servonum + zombieNum, 0, pulselen);
  }
}

void raiseZombie(int zombieNum) {
  Serial.println("Raising zombie ");
  Serial.println(zombieNum);
  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
    pwm.setPWM(servonum + zombieNum, 0, pulselen);
  }
}

// You can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. It's not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;

  pulselength = 1000000;      // 1,000,000 us per second
  pulselength /= SERVO_FREQ;  // Analog servos run at ~60 Hz updates
  Serial.print(pulselength);
  Serial.println(" us per period");
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength);
  Serial.println(" us per bit");
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}