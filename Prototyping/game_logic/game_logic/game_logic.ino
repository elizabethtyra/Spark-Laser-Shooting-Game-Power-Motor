#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <EEPROM.h>

#include <Servo.h>


#define SENSOR1_PIN 2
#define SENSOR2_PIN 3
// #define SENSOR3_PIN 18
// #define SENSOR4_PIN 19
// #define SENSOR5_PIN 20
// #define SENSOR6_PIN 21
#define NUM_SENSORS 6
#define COOLDOWN 7000
#define GAME_TIME 120000
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define ZOMBIE_DOWN 0
#define ZOMBIE_UP 1

#define NPN_PIN 7
#define BUTTON_PIN 5
#define RELOAD_PIN 3
#define BULLETS 200
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


#define DETECT1 8 // limit switch on right side
#define DETECT2 9 // limit switch on left side

int playerScore = 0;

int playerLives = 3;
unsigned long lastHit[NUM_SENSORS] = {0};
unsigned long zombieState[NUM_SENSORS] = {1, 1, 1, 1, 1, 1};
unsigned long startTime = 0;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
// our servo # counter
uint8_t servonum = 12;  

// intialize globals for bullet count
int8_t btn;
unsigned int bulletCount;
// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//oled is the name of the OLED object we just constructed


//globals for leaderboard
int topHighScore;
char initial1;
char initial2;


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


void lowerZombie(int zombieNum) {
  Serial.println("Lowering zombie ");
  Serial.print(zombieNum);
  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
    pwm.setPWM(servonum + zombieNum - 1, 0, pulselen);
  }
}

void raiseZombie(int zombieNum) {
  Serial.println("Raising zombie ");
  Serial.print(zombieNum);
  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
    pwm.setPWM(servonum + zombieNum - 1, 0, pulselen);
  }
}


void processHit(const char* sensor, unsigned long* lastTime, int zombieNum) {
    if (millis() - *(lastTime) > COOLDOWN  && zombieState[zombieNum-1] == ZOMBIE_UP) {
      *(lastTime) = millis();
      playerScore+=10;
      Serial.print(sensor);
      Serial.println(" HIT");
      Serial.println("Player Score: ");
      Serial.print(playerScore);
      zombieState[zombieNum-1] = ZOMBIE_DOWN;
      lowerZombie(zombieNum);
    }
    else {
      // Serial.print(sensor);
      // Serial.println(" on COOLDOWN");
    }
    return;
}

void printSensorState(int sensorPin) {
  switch (sensorPin) {
    case SENSOR1_PIN: {
      processHit("Sensor 1", &lastHit[0], 1);
      break;
    }
    case SENSOR2_PIN: {
      processHit("Sensor 2", &lastHit[1], 2);
      break;
    }
    // case SENSOR3_PIN: {
    //   processHit("Sensor 3", &lastHit[2], 3);
    //   break;
    // }
    // case SENSOR4_PIN: {
    //   processHit("Sensor 4", &lastHit[3], 4);
    //   break;
    // }
    // case SENSOR5_PIN: {
    //   processHit("Sensor 5", &lastHit[4], 5);
    //   break;
    // }
    // case SENSOR6_PIN: {
    //   processHit("Sensor 6", &lastHit[5], 6);
    //   break;
    // }
    default:
      break;
  }
}

void gameOver() {
    if(playerScore > topHighScore) {
      topHighScore = playerScore; // update highest score for leaderboard --> need to write this to memory later
      
      //write to memory
      EEPROM.update(0, initial1);
      EEPROM.update(1, initial2);
      EEPROM.put(3, topHighScore);
    }

    Serial.println("Game Over");
    Serial.print("Player Score: ");
    Serial.print(playerScore);
    Serial.print("Top Score: ");
    Serial.print(topHighScore);
    while(1);
}

// You can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. It's not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= SERVO_FREQ;   // Analog servos run at ~60 Hz updates
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // initialize how many bits/s get communicated to the Serial monitor
  pinMode(SENSOR1_PIN, INPUT);
  pinMode(SENSOR2_PIN, INPUT);
  // pinMode(SENSOR3_PIN, INPUT);
  // pinMode(SENSOR4_PIN, INPUT);
  // pinMode(SENSOR5_PIN, INPUT);
  // pinMode(SENSOR6_PIN, INPUT);
  // attachInterrupt(digitalPinToInterrupt(SENSOR1_PIN), []{ printSensorState(SENSOR1_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR2_PIN), []{ printSensorState(SENSOR2_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR3_PIN), []{ printSensorState(SENSOR3_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR4_PIN), []{ printSensorState(SENSOR4_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR5_PIN), []{ printSensorState(SENSOR5_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR6_PIN), []{ printSensorState(SENSOR6_PIN); }, RISING);
  // Testing laser
  digitalWrite(7, HIGH);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  
  initial1 = 'a'; // testing
  initialb = 'b';
  EEPROM.update(3, 0);
  topHighScore = 0;
  delay(10);
  startTime = millis();
    //digitalWrite(8, HIGH); // comment out when real laser is used

    // Declare pins as Outputs (stepper)
	pinMode(stepPin, OUTPUT);
	pinMode(dirPin, OUTPUT);

  // Initialise BOSS ZOMBIE
  pinMode(STATUS_LED_PIN, OUTPUT);
  servo1_boss_zombie.attach(SERVO_PIN);

  // set the zombie into walking to the right mode
  digitalWrite(STATUS_LED_PIN, LOW); // will be high when it's eating
	digitalWrite(dirPin, HIGH); // ASSUME THIS IS THE RIGHT WAY OTHERWISE WILL SWITCH

  // define input pins for limit switches
  pinMode(DETECT1, INPUT);
  pinMode(DETECT2, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(SENSOR1_PIN)) {
    printSensorState(SENSOR1_PIN);
  }
  else if (digitalRead(SENSOR2_PIN)) {
    printSensorState(SENSOR2_PIN);
  }

  for(int i = 0; i < NUM_SENSORS; i++) {
    if(millis() - lastHit[i] > COOLDOWN && zombieState[i] == ZOMBIE_DOWN) {
      raiseZombie(i+1);
      zombieState[i] = ZOMBIE_UP;
    }
  }


  if((millis() - startTime >= GAME_TIME) || (playerLives == 0)) {
    gameOver();
  }

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

      //check if zombie has reached the end !!
  int detected1 = digitalRead(DETECT1); // read right limit switch !!

  if (detected1 == 1) // if zombie reaches right side, turn to go to the left !!
  {
    playerLives -= 1;
    digitalWrite(dirPin, LOW);
    ZOMBIE_STATE = 0;
    servo1_boss_zombie.write(180);
    // add code for sunflower to go down, and then come up again
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

    int detected2 = digitalRead(DETECT2); // read left limit switch !!
  if(detected2 == 1) // if zombie reaches left side, turn to go to the right !!
  {
    playerLives -= 1;
    digitalWrite(dirPin, HIGH);
    ZOMBIE_STATE = 1;
    servo1_boss_zombie.write(0);
    // add code for sunflower to go down, and then come up again
  }

  }

}