//- Stepper → moves back and forth, stops when reaches the end and the zombie starts to eat, once done, turns around and walks the other way, stops and turns if it gets hit (turns instantaneously).
//- Servo → when changing direction or starting walking, need to turn the zombie around 180 degrees.
//- Sensor for Reaching End → PIR or Ultrasonic sensor will need to work this in for telling when the zombie has reached the end so that it stops moving and begins eating.
//- 2 Photoresistors + Status LED for the Zombie
//- 6 LEDS → 3 for each flower to indicate when it’s done being eaten. (for these LEDs in terms of mech, we can just tape them to the ends).

// zombie changes direction when hits limit switch, sunflower goes down, 3 health before dead !!

//----------------------------------------------------------

#include <Servo.h>

// pins for limit switches !!
#define DETECT1 6  // limit switch on right side
#define DETECT2 7  // limit switch on left side
// #define DETECT3 8 // limit switch on right side
// #define DETECT4 9 // limit switch on left side

// BOSS ZOMBIE AND ALL PARTS - 2 LDRS, 1 LED, 1 Servo
const int STATUS_LED_PIN = 11;  // Status LED pin
const int SENSOR_PIN = A1;      // Photoresistor pin
const int SENSOR_PIN2 = A0;     // Photoresistor pin the other one
const int SERVO_PIN = 7;
Servo servo1_boss_zombie;  // create servo object

// STEPPER - moves side to side
const int stepPin = 2;
const int dirPin = 3;
// Define stuff for only STEPPER2
const int stepPin2 = 4;
const int dirPin2 = 5;
const int stepsPerRevolution = 1000;    // we tried 1000 and it was half of the belt
int curr_pos = stepsPerRevolution / 2;  // assume starting position is right in the middle

// Setting up objects and values
int lightThres = 200;  // Photoresistor reading values
int ZOMBIE_STATE = 1;  // 1-moving right/clockwise, 0-moving left/CCW, 2-not walking
int pos_servo1 = 0;    // Zombie is turned to the right (walking right)
int health = 3;        //health status

bool GAME_OVER = false;

void setup() {
  // Declare pins as Outputs (stepper)
  pinMode(stepPin, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(dirPin2, OUTPUT);

  // Initialise BOSS ZOMBIE
  pinMode(STATUS_LED_PIN, OUTPUT);
  servo1_boss_zombie.attach(SERVO_PIN);

  // set the zombie into walking to the right mode
  digitalWrite(STATUS_LED_PIN, LOW);  // will be high when it's eating
  digitalWrite(dirPin, HIGH);         // ASSUME THIS IS THE RIGHT WAY OTHERWISE WILL SWITCH
  digitalWrite(dirPin2, HIGH);        // ASSUME THIS IS THE RIGHT WAY OTHERWISE WILL SWITCH


  // define input pins for limit switches !!
  pinMode(DETECT1, INPUT_PULLUP);
  pinMode(DETECT2, INPUT_PULLUP);

  Serial.begin(9600);
}


void loop() {
  // to start the zombie will begin to walk in one direction, it takes one step
  if (ZOMBIE_STATE == 1) {  // moving right
    digitalWrite(STATUS_LED_PIN, LOW);
    // to start the zombie will begin to walk in one direction, it takes one step
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(2500);
    digitalWrite(stepPin, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(2500);
    curr_pos = curr_pos + 1;

    // check photo res1 too see if got hit on this side
    int sensorValue = analogRead(SENSOR_PIN);
    bool gotHit = (sensorValue > lightThres);
    if (gotHit) {  // zombie now changes direction
      digitalWrite(dirPin, LOW);
      digitalWrite(dirPin2, LOW);
      ZOMBIE_STATE = 0;
      servo1_boss_zombie.write(180);
    }

    // // turn around automatically if reaches the end
    //  if (curr_pos == stepsPerRevolution){
    //    digitalWrite(dirPin, LOW);
    //    ZOMBIE_STATE = 0;
    //    servo1_boss_zombie.write(180);
    //  }

    //check if zombie has reached the end !!
    int detected1 = digitalRead(DETECT1);  // read right limit switch !!

    //Serial.println(detected1);
    //Serial.println(detected2);
    if (detected1 == 1)  // if zombie reaches right side, turn to go to the left !!
    {
      health -= 1;
      digitalWrite(dirPin, LOW);
      digitalWrite(dirPin2, LOW);
      ZOMBIE_STATE = 0;
      servo1_boss_zombie.write(180);
      // add code for sunflower to go down, and then come up again
    }
  }
  if (ZOMBIE_STATE == 0) {  // moving left
    digitalWrite(STATUS_LED_PIN, HIGH);
    // to start the zombie will begin to walk in one direction, it takes one step
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin2, HIGH);

    delayMicroseconds(2500);  // longer delay -> walk slower
    digitalWrite(stepPin, LOW);
    digitalWrite(stepPin2, LOW);

    delayMicroseconds(2500);
    curr_pos = curr_pos - 1;

    // check photo res2 too see if got hit on this side
    int sensorValue = analogRead(SENSOR_PIN2);
    bool gotHit = (sensorValue > lightThres);
    if (gotHit) {  // zombie now changes direction
      digitalWrite(dirPin, HIGH);
      digitalWrite(dirPin2, HIGH);

      ZOMBIE_STATE = 1;
      servo1_boss_zombie.write(0);
    }

    // // turn around automatically if reaches the end
    //  if (curr_pos == 0){
    //    digitalWrite(dirPin, HIGH);
    //    ZOMBIE_STATE = 1;
    //    servo1_boss_zombie.write(0);
    //  }

    int detected2 = digitalRead(DETECT2);  // read left limit switch !!
    if (detected2 == 1)                    // if zombie reaches left side, turn to go to the right !!
    {
      health -= 1;
      digitalWrite(dirPin, HIGH);
      digitalWrite(dirPin2, HIGH);
      ZOMBIE_STATE = 1;
      servo1_boss_zombie.write(0);
      // add code for sunflower to go down, and then come up again
    }
  }

  if (health == 0) {
    GAME_OVER = true;
  }
}