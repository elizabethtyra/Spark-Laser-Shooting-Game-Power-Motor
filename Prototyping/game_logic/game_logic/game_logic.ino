#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <Servo.h>


#define SENSOR0_PIN 19
#define SENSOR1_PIN 20
#define SENSOR2_PIN 21
#define SENSOR3_PIN 22
#define SENSORBOSS_PIN 23
#define SENSORCAR_PIN 24
// #define SENSOR5_PIN 20
// #define SENSOR6_PIN 21

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

// #define NPN_PIN 7
// #define BUTTON_PIN 5
// #define RELOAD_PIN 3
// #define BULLETS 200
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


#define DETECT1 8  // limit switch on right side
#define DETECT2 9  // limit switch on left side

int playerScore = 0;
int playerLives = 3;
unsigned long lastHit[NUM_SENSORS] = { 0 };

// Stationary: 1 = UP, 0 = DOWN
// Moving: 1 = RIGHT, 0 = LEFT
unsigned long zombieState[NUM_SENSORS] = { 1, 1, 1, 1, 1, 1 };
unsigned long startTime = 0;
// int bossZombieState = 1;
// int carZombieState = 1;
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
char firstNameInitial;
char lastNameInitial;
uint8_t topScoreLowByte;
uint8_t topScoreHighByte;
// function prototype declarations
void writeTopScoreToEEPROM();
void readEEPROMTopScore();

/**************** STEPPER PARAMETERS ***************/
// Define stuff for both stepper motors
const int stepsPerRevolution = 1000; 
const int stopDelay = 2000; // How long to wait once reached the end (to handle lives and stuff)
const int stepDelay_high_low = 2500; // Delay for clockwise rotation
const int stepDelay_low_high = 2500; // Delay for counterclockwise rotation
unsigned long delayNum = 2500;

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


void processHit(const char* sensor, unsigned long* lastTime, int zombieNum, bool moving) {

  // Process stationary zombies by updating score + zombie state
  if (!moving) {
    if (millis() - *(lastTime) > COOLDOWN && zombieState[zombieNum] == ZOMBIE_UP) {
      *(lastTime) = millis();
      playerScore += 10;
      Serial.print(sensor);
      Serial.println(" HIT");
      Serial.println("Player Score: ");
      Serial.println(playerScore);
      zombieState[zombieNum] = ZOMBIE_DOWN;
      lowerZombie(zombieNum);
    } 
    else {
    // Serial.print(sensor);
    // Serial.println(" on COOLDOWN");
    }
  }

  // Process moving zombies by changing direction in addition to score
  else if (moving) {
    if (millis() - *(lastTime) > MOVING_COOLDOWN) {
      *(lastTime) = millis();
      playerScore += 10;
      Serial.print(sensor);
      Serial.println(" HIT");
      Serial.println("Player Score: ");
      Serial.println(playerScore);
      // Toggle direction
      zombieState[zombieNum] = 1 - zombieState[zombieNum];
      Serial.println("Direction switched");
      if (zombieNum == 4) {
        digitalWrite(dirPinBoss, zombieState[zombieNum]);  
      }
      else if (zombieNum == 5) {
        digitalWrite(dirPinCar, zombieState[zombieNum]);  
      }
    } 
    else {
    // Serial.print(sensor);
    // Serial.println(" on COOLDOWN");
    }
  }

  return;
}


void printSensorState(int sensorPin) {
  switch (sensorPin) {
    case SENSOR0_PIN:
      {
        processHit("Sensor 0", &lastHit[0], 0, false);
        break;
      }
    case SENSOR1_PIN:
      {
        processHit("Sensor 1", &lastHit[1], 1, false);
        break;
      }
    case SENSOR2_PIN: {
      processHit("Sensor 2", &lastHit[2], 2, false);
      break;
    }
    case SENSOR3_PIN: {
      processHit("Sensor 3", &lastHit[3], 3, false);
      break;
    }
    case SENSORBOSS_PIN: {
      processHit("Sensor Boss", &lastHit[4], 4, true);
      break;
    }
    case SENSORCAR_PIN: {
      processHit("Sensor Car", &lastHit[5], 5, true);
      break;
    }
    default:
      break;
  }
}

void gameOver() {
  if (playerScore > topHighScore) {
    topHighScore = playerScore;  // update highest score for leaderboard --> need to write this to memory later
    //write to memory
    writeTopScoreToEEPROM();
  }

  Serial.println("Game Over");
  Serial.print("Player Score: ");
  Serial.print(playerScore);
  Serial.print("Top Score: ");
  Serial.print(topHighScore);
  while (1)
    ;
}
//read top player high score from EEPROM
void readEEPROMTopScore() {
    //read from EEPROM
  firstNameInitial = EEPROM.read(0);
  lastNameInitial = EEPROM.read(1);
  topScoreLowByte = EEPROM.read(2);
  topScoreHighByte = EEPROM.read(3);
  topHighScore = topScoreLowByte + (topScoreHighByte << 8);

}
void writeTopScoreToEEPROM() {

  EEPROM.update(0, firstNameInitial);
  EEPROM.update(1, lastNameInitial);

  // Put topHighScore (16 bits?) into two 8-bit variables and then store into memory
  topScoreLowByte = (uint8_t)topHighScore;
  topScoreHighByte = (uint8_t)(topHighScore >> 8);
  EEPROM.put(2, topScoreLowByte);
  EEPROM.put(3, topScoreHighByte);
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


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  // initialize how many bits/s get communicated to the Serial monitor

  /**************** SENSOR PINS ***************/
  pinMode(SENSOR0_PIN, INPUT);
  pinMode(SENSOR1_PIN, INPUT);
  pinMode(SENSOR2_PIN, INPUT);
  pinMode(SENSOR3_PIN, INPUT);
  pinMode(SENSORBOSS_PIN, INPUT);
  pinMode(SENSORCAR_PIN, INPUT);



  /**************** MOVING ZOMBIES ***************/

  // No idea why the switches are INPUT_PULLUP (which means 1 or HIGH by default when the switch is OPEN) but it seems to work when we tested it
  // If for some reason this stops working in the future, change the if statements under 'BOSS ZOMBIE LIMIT SWITCHES' and 'CAR ZOMBIE LIMIT SWITCHES' in 'loop()' to '== LOW'
  // Switches are connected like so:
  // GND --- | | --- PIN
  //          O
  //     --- | | ---


  // STEPPER1 SET setup
  pinMode(limitSwitchBoss_1, INPUT_PULLUP);
  pinMode(limitSwitchBoss_2, INPUT_PULLUP);
  pinMode(stepPinBoss, OUTPUT);
	pinMode(dirPinBoss, OUTPUT);
  digitalWrite(dirPinBoss, HIGH); // starts same direction

  // STEPPER2 SET setup
  pinMode(limitSwitchCar_1, INPUT_PULLUP);
  pinMode(limitSwitchCar_2, INPUT_PULLUP);
  pinMode(stepPinCar, OUTPUT);
	pinMode(dirPinCar, OUTPUT);
  digitalWrite(dirPinCar, HIGH); // starts same direction


  // pinMode(SENSOR5_PIN, INPUT);
  // pinMode(SENSOR6_PIN, INPUT);
  // attachInterrupt(digitalPinToInterrupt(SENSOR1_PIN), []{ printSensorState(SENSOR1_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR2_PIN), []{ printSensorState(SENSOR2_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR3_PIN), []{ printSensorState(SENSOR3_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR4_PIN), []{ printSensorState(SENSOR4_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR5_PIN), []{ printSensorState(SENSOR5_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR6_PIN), []{ printSensorState(SENSOR6_PIN); }, RISING);

  // Testing laser - Uncomment below code if laser is connected to the same board as game logic for testing purposes
  // digitalWrite(7, HIGH);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  firstNameInitial = 'a';  // testing EEPROM
  lastNameInitial = 'b';
  // for testing: writing above intials to EEPROM to test if we can read from it after
  writeTopScoreToEEPROM();
  readEEPROMTopScore();
  delay(10);

  startTime = millis();
  Serial.println("Raising all stationary zombies");

  /**************** RESET STATIONARY ZOMBIES ***************/
  for (int i = 0; i < STATIONARY_ZOMBIES; i++) {
    raiseZombie(i);
    zombieState[i] = ZOMBIE_UP;
  }


  // // Declare pins as Outputs (stepper)
  // pinMode(stepPin, OUTPUT);
  // pinMode(dirPin, OUTPUT);

  // // Initialise BOSS ZOMBIE
  // pinMode(STATUS_LED_PIN, OUTPUT);
  // servo1_boss_zombie.attach(SERVO_PIN);

  // set the zombie into walking to the right mode
  // digitalWrite(STATUS_LED_PIN, LOW);  // will be high when it's eating
  digitalWrite(dirPinBoss, HIGH);         // ASSUME THIS IS THE RIGHT WAY OTHERWISE WILL SWITCH
  digitalWrite(dirPinCar, HIGH);         // ASSUME THIS IS THE RIGHT WAY OTHERWISE WILL SWITCH


  // // define input pins for limit switches
  // pinMode(DETECT1, INPUT);
  // pinMode(DETECT2, INPUT);
}

void loop() {

  /****************GAME END***************/
  if ((millis() - startTime >= GAME_TIME) || (playerLives == 0)) {
    for (int i = 0; i < STATIONARY_ZOMBIES; i++) {
      if (zombieState[i] == ZOMBIE_DOWN) {
        raiseZombie(i);
        zombieState[i] = ZOMBIE_UP;
      }
    }
    gameOver();
  }

  /**************** DETECT ZOMBIE HIT ***************/
  if (digitalRead(SENSOR0_PIN)) {
    printSensorState(SENSOR0_PIN);
  }
  else if (digitalRead(SENSOR1_PIN)) {
    printSensorState(SENSOR1_PIN);
  }
  else if (digitalRead(SENSOR2_PIN)) {
    printSensorState(SENSOR2_PIN);
  }
  else if (digitalRead(SENSOR3_PIN)) {
    printSensorState(SENSOR3_PIN);
  }
  else if (digitalRead(SENSORBOSS_PIN)) {
    printSensorState(SENSORBOSS_PIN);
  }
  else if (digitalRead(SENSORCAR_PIN)) {
    printSensorState(SENSORCAR_PIN);
  }


  /****************RAISING STATIONARY ZOMBIES***************/
  for (int i = 0; i < STATIONARY_ZOMBIES; i++) {
    if (millis() - lastHit[i] > COOLDOWN && zombieState[i] == ZOMBIE_DOWN) {
      raiseZombie(i);
      zombieState[i] = ZOMBIE_UP;
    }
  }

  /**************** MOVING ZOMBIES ***************/

  // Stepping one at a time. If this does not work, replace the entire code within the loop function with an extra for loop like so:
  /*
  void loop() {

    for(int x = 0; x < stepsPerRevolution; x++)
    {

        All the code in loop()

    }

  
  }
  */

  digitalWrite(stepPinBoss, HIGH);
  digitalWrite(stepPinCar, HIGH);
  delayMicroseconds(delayNum);

  digitalWrite(stepPinBoss, LOW);
  digitalWrite(stepPinCar, LOW);
  delayMicroseconds(delayNum);


  // BOSS ZOMBIE LIMIT SWITCHES
  if (digitalRead(limitSwitchBoss_1) == HIGH) {
    digitalWrite(dirPinBoss, HIGH);
    playerLives--;
    zombieState[4] = 1;  
    delay(stopDelay);
  }
  if (digitalRead(limitSwitchBoss_2) == HIGH) {
    digitalWrite(dirPinBoss, LOW);
    playerLives--;
    zombieState[4] = 0;  
    delay(stopDelay); // wait for the zombie to do stuff -> call zombie function here
  }

  // CAR ZOMBIE LIMIT SWITCHES
  if (digitalRead(limitSwitchCar_1) == HIGH) {
    digitalWrite(dirPinCar, HIGH);
    zombieState[5] = 1;  
    delay(stopDelay); // wait for the zombie to do stuff -> call zombie function here
  }
  if (digitalRead(limitSwitchCar_2) == HIGH) {
    digitalWrite(dirPinCar, LOW);
    zombieState[5] = 0;
    delay(stopDelay); // wait for the zombie to do stuff -> call zombie function here
  }

}