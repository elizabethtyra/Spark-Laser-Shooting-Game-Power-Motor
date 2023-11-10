//- Stepper → moves back and forth, stops when reaches the end and the zombie starts to eat, once done, turns around and walks the other way, stops and turns if it gets hit (turns instantaneously).
//- Servo → when changing direction or starting walking, need to turn the zombie around 180 degrees.
//- Sensor for Reaching End → PIR or Ultrasonic sensor will need to work this in for telling when the zombie has reached the end so that it stops moving and begins eating.
//- 2 Photoresistors + Status LED for the Zombie
//- 6 LEDS → 3 for each flower to indicate when it’s done being eaten. (for these LEDs in terms of mech, we can just tape them to the ends).

//----------------------------------------------------------

#include <Servo.h>

// BOSS ZOMBIE AND ALL PARTS - 2 LDRS, 1 LED, 1 Servo
const int STATUS_LED_PIN = 11;     // Status LED pin
const int SENSOR_PIN = A1;        // Photoresistor pin
const int SENSOR_PIN2 = A0;        // Photoresistor pin the other one
const int SERVO_PIN = 7;
Servo servo1_boss_zombie; // create servo object

// STEPPER - moves side to side
const int stepPin = 3;
const int dirPin = 2;
const int stepsPerRevolution = 2000; // we tried 1000 and it was half of the belt
int curr_pos = stepsPerRevolution/2; // assume starting position is right in the middle

// Setting up objects and values
int lightThres = 200; // Photoresistor reading values
int ZOMBIE_STATE = 1; // 1-moving right/clockwise, 0-moving left/CCW, 2-not walking
int pos_servo1 = 0; // Zombie is turned to the right (walking right)

void setup()
{
	// Declare pins as Outputs (stepper)
	pinMode(stepPin, OUTPUT);
	pinMode(dirPin, OUTPUT);

  // Initialise BOSS ZOMBIE
  pinMode(STATUS_LED_PIN, OUTPUT);
  servo1_boss_zombie.attach(SERVO_PIN);

  // set the zombie into walking to the right mode
  digitalWrite(STATUS_LED_PIN, LOW); // will be high when it's eating
	digitalWrite(dirPin, HIGH); // ASSUME THIS IS THE RIGHT WAY OTHERWISE WILL SWITCH

  Serial.begin(9600);
}


void loop()
{
  // to start the zombie will begin to walk in one direction, it takes one step
  if(ZOMBIE_STATE == 1 ){ // moving right
    digitalWrite(STATUS_LED_PIN, LOW);
    // to start the zombie will begin to walk in one direction, it takes one step
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);
    curr_pos = curr_pos +1;

    // check photo res1 too see if got hit on this side
    int sensorValue = analogRead(SENSOR_PIN);
    bool gotHit = (sensorValue > lightThres);
    if (gotHit) { // zombie now changes direction
      digitalWrite(dirPin, LOW);
      ZOMBIE_STATE = 0;
      servo1_boss_zombie.write(180);
    }

    // turn around automatically if reaches the end
    if (curr_pos == stepsPerRevolution){
      digitalWrite(dirPin, LOW);
      ZOMBIE_STATE = 0;
      servo1_boss_zombie.write(180);
    }
  
  }
  if(ZOMBIE_STATE == 0 ){ // moving left
    digitalWrite(STATUS_LED_PIN, HIGH);
    // to start the zombie will begin to walk in one direction, it takes one step
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000); // longer delay -> walk slower
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);
    curr_pos = curr_pos -1;

    // check photo res2 too see if got hit on this side
    int sensorValue = analogRead(SENSOR_PIN2);
    bool gotHit = (sensorValue > lightThres);
    if (gotHit) { // zombie now changes direction
      digitalWrite(dirPin, HIGH);
      ZOMBIE_STATE = 1;
      servo1_boss_zombie.write(0);
    }

    // turn around automatically if reaches the end
    if (curr_pos == 0){
      digitalWrite(dirPin, HIGH);
      ZOMBIE_STATE = 1;
      servo1_boss_zombie.write(0);
    }
  }
}