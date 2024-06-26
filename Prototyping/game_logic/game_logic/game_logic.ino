#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_LEDBackpack.h>
#include <EEPROM.h>
#include <Adafruit_LEDBackpack.h>
#include <Servo.h>
#include <MD_MAX72xx.h>
#include <SPI.h>


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

// dot matrix lives displays
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES 3

//Hardware SPI pins for Arduino UNO, change for Arduino Mega 
// CLK Pin  > 13 SCK
// Data Pin > 11 MOSI
#define CS_PIN    10

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

byte heart_fill[8] = {0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18};
byte heart_empty[8] = {0x00, 0x66, 0x99, 0x81, 0x81, 0x42, 0x24, 0x18};

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

// timer 7 segment displays
Adafruit_7segment matrix0 = Adafruit_7segment(); // timer
Adafruit_7segment matrix1 = Adafruit_7segment(); // leaderboard 1st place
Adafruit_7segment matrix2 = Adafruit_7segment(); // leaderboard 2nd place
Adafruit_7segment matrix3 = Adafruit_7segment(); // leaderboard 3rd place
Adafruit_7segment matrix4 = Adafruit_7segment(); // current score
volatile uint16_t timer_count;
volatile int interrupt_counter = 0;

// interrupt service routine for timer2
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
  matrix0.writeDigitNum(0, (timer_count / 1000));
  matrix0.writeDigitNum(1, (timer_count / 100) % 10);
  matrix0.writeDigitNum(3, (timer_count / 10) % 10, true);
  matrix0.writeDigitNum(4, timer_count % 10);
 
  matrix0.writeDisplay();
}

void update_7seg(int score, Adafruit_7segment matrix) {
  matrix.writeDigitNum(0, (score / 1000));
  matrix.writeDigitNum(1, (score / 100) % 10);
  matrix.writeDigitNum(3, (score / 10) % 10);
  matrix.writeDigitNum(4, score % 10);
 
  matrix.writeDisplay();
}


// timer 7 segment displays
Adafruit_7segment matrix0 = Adafruit_7segment();
Adafruit_7segment matrix1 = Adafruit_7segment();
volatile uint16_t timer_count;
volatile int interrupt_counter = 0;

// interrupt service routine for timer2
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
  matrix0.writeDigitNum(0, (timer_count / 1000));
  matrix0.writeDigitNum(1, (timer_count / 100) % 10);
  matrix0.writeDigitNum(3, (timer_count / 10) % 10, true);
  matrix0.writeDigitNum(4, timer_count % 10);
 
  matrix0.writeDisplay();
}

//globals for leaderboard
int highScores[3] = {0};
// char firstNameInitial;
// char lastNameInitial;
uint8_t topScoreLowByte;
uint8_t topScoreHighByte;
// function prototype declarations
void writeTopScoreToEEPROM(int position);
int readEEPROMTopScore(int position);



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
      update_7seg(0, matrix4);
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
      update_7seg(0, matrix4);
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
  if (playerScore > highScores[0]) {
    // topHighScore = playerScore;  // update highest score for leaderboard --> need to write this to memory later
    //write to memory
    highScores[2] = highScores[1];
    highScores[1] = highScores[0];
    highScores[0] = playerScore;

    writeTopScoreToEEPROM(1, highScores[0]);
    writeTopScoreToEEPROM(2, highScores[1]);
    writeTopScoreToEEPROM(3, highScores[2]);
    
    update_7seg(highScores[0], matrix1);
    update_7seg(highScores[1], matrix2);
    update_7seg(highScores[2], matrix3);
  }
  else if (playerScore > highScores[1]) {
    highScores[2] = highScores[1];
    highScores[1] = playerScore;

    writeTopScoreToEEPROM(2, highScores[1]);
    writeTopScoreToEEPROM(3, highScores[2]);
    
    update_7seg(highScores[1], matrix2);
    update_7seg(highScores[2], matrix3);
  }
  else if (playerScore > highScores[2]) {
    highScores[2] = playerScore;

    writeTopScoreToEEPROM(3, highScores[2]);

    update_7seg(highScores[2], matrix3);
  }

  update_7seg(0, matrix4);

  Serial.println("Game Over");
  Serial.print("Player Score: ");
  Serial.print(playerScore);
  Serial.print("Top Score: ");
  Serial.print(topHighScore);
  while (1);
}

//read top player high score from EEPROM
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

// dot matrix lives function
void drawInitialLives() {
  for (int i = 0; i < 8; i++) {
    mx.setRow(0, 0, i, heart_fill[i]);
  }

  for (int i = 0; i < 8; i++) {
    mx.setRow(1, 1, i, heart_fill[i]);
  }

  for (int i = 0; i < 8; i++) {
    mx.setRow(2, 2, i, heart_empty[i]); // change later
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

// dot matrix lives function
void drawInitialLives() {
  for (int i = 0; i < 8; i++) {
    mx.setRow(0, 0, i, heart_fill[i]);
  }
  
  for (int i = 0; i < 8; i++) {
    mx.setRow(1, 1, i, heart_fill[i]);
  }

  for (int i = 0; i < 8; i++) {
    mx.setRow(2, 2, i, heart_empty[i]); // change later
  }

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


  // timer interrupts setup
  TCCR0B = 0;
  TCCR2A = 0;
  OCR2A = 0;
  TCNT2  = 0;
  TCCR2B = 0;
  TCCR2A = 1 << WGM21;
  OCR2A = 400;
  TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
  TIMSK2 = (1 << OCIE2A);
  sei();
  Serial.println("TIMER2 Setup Finished.");


  // timer 7 segment setup
  matrix0.begin(0x70);
  matrix1.begin(0x71);
  // Not sure if below has correct addresses
  matrix2.begin(0x7e);
  matrix3.begin(0x85);
  matrix4.begin(0x8c);
  Serial.println("Matrix Setup Finished.");
  timer_count = 0;

  
  // firstNameInitial = 'a';  // testing EEPROM
  // lastNameInitial = 'b';
  // for testing: writing above intials to EEPROM to test if we can read from it after
  // writeTopScoreToEEPROM();

  /************ RESET LEADERBOARD *************/

  // To reset leaderboard scores, uncomment the code below

  // writeTopScoreToEEPROM(1, 0);
  // writeTopScoreToEEPROM(2, 0);
  // writeTopScoreToEEPROM(3, 0);

  /************ INIT LEADERBOARD *************/
  writeTopScoreToEEPROM(1, 100);
  highScores[0] = readEEPROMTopScore(1);
  highScores[1] = readEEPROMTopScore(2);
  highScores[2] = readEEPROMTopScore(3);
  update_7seg(highScores[0], matrix1);
  delay(10);
  update_7seg(highScores[1], matrix2);
  delay(10);
  update_7seg(highScores[2], matrix3);
  delay(10);
  update_7seg(0, matrix4);
  delay(10);


  // dot matrix 3 lives setup
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 0);
  mx.clear();

  drawInitialLives();
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
