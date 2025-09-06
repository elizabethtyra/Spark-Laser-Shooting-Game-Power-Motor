#include "displays.h"
#include "motors.h"
#include "game_logic.h"
#include "parameters.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void displayText(const char* text) {
  oled.clearDisplay();       //clear display
  oled.setTextSize(2);       // text size
  oled.setTextColor(WHITE);  // text color
  oled.setCursor(0, 0);      // position to display
  oled.println(text);
  oled.display();
}

void setup() {
  Serial.begin(115200);  // initialize how many bits/s get communicated to the Serial monitor
  //   writeTopScoreToEEPROM(1, 999);
  // writeTopScoreToEEPROM(2, 888);
  // writeTopScoreToEEPROM(3, 777);

  delayNum = 800;
  playerLives = 3;
  pwm.begin();
  delay(1000);
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  // dot matrix 3 lives setup
  livesMatrix.begin();
  livesMatrix.control(MD_MAX72XX::INTENSITY, 0);

  livesMatrix.clear();



  drawInitialLives();


  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // Serial.println("SSD1306 allocation failed");
    while (true)
      ;
  }

  displayText("Done drawing lives");


  // // timer2 interrupt setup
  TCCR0B = 0;
  TCCR2A = 0;
  OCR2A = 0;
  TCNT2 = 0;
  TCCR2B = 0;
  TCCR2A = 1 << WGM21;
  OCR2A = 400;
  TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
  TIMSK2 = (1 << OCIE2A);
  sei();
  // Serial.println("TIMER2 Setup Finished.");



  displayText("After live matrix setup");

  // 7 segment display setup
  timerMatrix.begin(0x70);
  topScore1.begin(0x71);
  topScore2.begin(0x72);
  topScore3.begin(0x73);
  userScore.begin(0x74);
  displayText("Matrix Setup Finished.");
  timer_count = GAME_TIME;
  playerScore = 0;

  update_timer();

  // init leaderboard
  highScores[0] = readEEPROMTopScore(1);
  highScores[1] = readEEPROMTopScore(2);
  highScores[2] = readEEPROMTopScore(3);
  update_7seg(highScores[0], topScore1);
  update_7seg(highScores[1], topScore2);
  update_7seg(highScores[2], topScore3);
  update_7seg(0, userScore);

  // writeTopScoreToEEPROM(3, 000);
  // highScores[2] = readEEPROMTopScore(3);
  // update_7seg(highScores[2], topScore3);



  // for(int i = 0; i < 3; i++) {
  // digitalWrite(LED_BUILTIN, HIGH);
  // delay(1000);
  // digitalWrite(LED_BUILTIN, LOW);
  // delay(1000);


  // }



  /**************** SENSOR PINS ***************/
  pinMode(SENSOR0_PIN, INPUT);
  pinMode(SENSOR1_PIN, INPUT);
  pinMode(SENSOR2_PIN, INPUT);
  pinMode(SENSOR3_PIN, INPUT);
  pinMode(SENSORBOSS_PIN, INPUT);
  pinMode(SENSORBOSS_PIN2, INPUT);
  pinMode(SENSORCAR_PIN, INPUT);
  pinMode(SENSORCAR_PIN2, INPUT);

  // STEPPER1 (BOSS) SET setup
  pinMode(limitSwitchBoss_1, INPUT_PULLUP);
  pinMode(limitSwitchBoss_2, INPUT_PULLUP);
  pinMode(stepPinBoss, OUTPUT);
  pinMode(dirPinBoss, OUTPUT);
  digitalWrite(dirPinBoss, LOW);  // starts counterclockwise, init zombie state = 0 = ZOMBIE_UP

  // STEPPER2 (CAR) SET setup
  pinMode(limitSwitchCar_1, INPUT_PULLUP);
  pinMode(limitSwitchCar_2, INPUT_PULLUP);
  pinMode(stepPinCar, OUTPUT);
  pinMode(dirPinCar, OUTPUT);
  digitalWrite(dirPinCar, HIGH);  // starts clockwise, init zombie state = 0 = ZOMBIE_UP

  pinMode(START_PIN, INPUT_PULLUP);



  // startTime = timer_count;
  // Serial.println("Raising all stationary zombies");
  displayText("end of setup");
  gameStart = false;
  timerStart = false;
  // while ('0');



  /**************** RESET STATIONARY ZOMBIES ***************/
  for (int i = 0; i < NUM_ZOMBIES; i++) {
    raiseZombie(i);
    zombieState[i] = ZOMBIE_UP;
  }

  // digitalWrite(dirPinBoss, LOW);  // ASSUME THIS IS THE RIGHT WAY OTHERWISE WILL SWITCH
  // digitalWrite(dirPinCar, LOW);   // ASSUME THIS IS THE RIGHT WAY OTHERWISE WILL SWITCH

  displayText("Done Setup");
}


void loop() {
  /****************GAME END***************/
  // displayText("In loop");
  while (!gameStart) {
    currentResetState = digitalRead(START_PIN);
    if (currentResetState != previousResetState && currentResetState == LOW) {
      displayText("Reset Button Pressed");
      gameStart = true;
      timerStart = stepPinCar;
    }
    previousResetState = currentResetState;
  }

  if ((timer_count <= 0) || (playerLives == 0)) {
    // while ('0')
    //   ;
    gameStart = false;
    timerStart = (stepPinBoss + dirPinBoss - limitSwitchCar_2);  //for tyra
    gameOver();
  }

  // speed up stepper motors over time
  if (timer_count < 300 && delayNum != 200) delayNum = 200;
  else if (timer_count < 600 && delayNum != 400) delayNum = 400;
  else if (timer_count < 900 && delayNum != 600) delayNum = 600;

  /****************RAISING STATIONARY ZOMBIES***************/
  for (int i = 0; i < STATIONARY_ZOMBIES; i++) {
    if (timer_count - lastHit[i] > COOLDOWN2 && zombieState[i] == ZOMBIE_DOWN) {
      raiseZombie(i);
      zombieState[i] = ZOMBIE_UP;
    }
  }

  /**************** DETECT ZOMBIE HIT ***************/
  if (timer_count - lastHit[0] > COOLDOWN && digitalRead(SENSOR0_PIN)) {
    printSensorState(SENSOR0_PIN);
    displayText("sensor 0 hit");

  }

  else if (timer_count - lastHit[1] > COOLDOWN && digitalRead(SENSOR1_PIN)) {
    printSensorState(SENSOR1_PIN);
  displayText("sensor 1 hit");

  }

  else if (timer_count - lastHit[2] > COOLDOWN && digitalRead(SENSOR2_PIN)) {
    printSensorState(SENSOR2_PIN);
    displayText("sensor 2 hit");

  }

  else if (timer_count - lastHit[3] > COOLDOWN && digitalRead(SENSOR3_PIN)) {
    displayText("sensor 3 hit");

    printSensorState(SENSOR3_PIN);
  }

  else if ((timer_count - lastHit[5] > MOVING_COOLDOWN) && (zombieState[CAR_ZOMBIE] == ZOMBIE_UP) && (digitalRead(SENSORCAR_PIN))) {
    printSensorState(SENSORCAR_PIN);
    displayText("car π hit");
  }

  else if ((timer_count - lastHit[5] > MOVING_COOLDOWN) && (zombieState[CAR_ZOMBIE] == ZOMBIE_DOWN) && (digitalRead(SENSORCAR_PIN2))) {
    printSensorState(SENSORCAR_PIN2);
    displayText("car * hit");
  }

  else if ((timer_count - lastHit[4] > MOVING_COOLDOWN) && (zombieState[BOSS_ZOMBIE] == ZOMBIE_UP) && (digitalRead(SENSORBOSS_PIN))) {
    printSensorState(SENSORBOSS_PIN);
    displayText("boss π hit");
  }

  else if ((timer_count - lastHit[4] > MOVING_COOLDOWN) && (zombieState[BOSS_ZOMBIE] == ZOMBIE_DOWN) && (digitalRead(SENSORBOSS_PIN2))) {
    printSensorState(SENSORBOSS_PIN2);
    displayText("boss * hit");
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

  // Debouncing switches
  currentStateBoss_1 = digitalRead(limitSwitchBoss_1);
  currentStateBoss_2 = digitalRead(limitSwitchBoss_2);
  currentStateCar_1 = digitalRead(limitSwitchCar_1);
  currentStateCar_2 = digitalRead(limitSwitchCar_2);

  // LOW = CCW = ZOMBIE_UP = 0
  // BOSS ZOMBIE LIMIT SWITCHES
  if (currentStateBoss_1 != previousStateBoss_1 && currentStateBoss_1 == LOW && zombieState[BOSS_ZOMBIE] == ZOMBIE_UP) {

    displayText("boss π hit");
    digitalWrite(dirPinBoss, HIGH);
    playerLives--;
    updateLives(playerLives);
    zombieState[BOSS_ZOMBIE] = ZOMBIE_DOWN;
    lowerZombie(BOSS_ZOMBIE);
  }

  if (currentStateBoss_2 != previousStateBoss_2 && currentStateBoss_2 == LOW && zombieState[BOSS_ZOMBIE] == ZOMBIE_DOWN) {

    displayText("boss * hit");
    digitalWrite(dirPinBoss, LOW);
    playerLives--;
    updateLives(playerLives);
    zombieState[BOSS_ZOMBIE] = ZOMBIE_UP;
    raiseZombie(BOSS_ZOMBIE);
  }

  // CAR ZOMBIE LIMIT SWITCHES
  if (currentStateCar_1 != previousStateCar_1 && currentStateCar_1 == LOW && zombieState[CAR_ZOMBIE] == ZOMBIE_UP) {

    displayText("car π hit");
    digitalWrite(dirPinCar, LOW); // Flipped because car starts CW (Boss starts CCW)
    zombieState[CAR_ZOMBIE] = ZOMBIE_DOWN;
    lowerZombie(CAR_ZOMBIE);
  }

  if (currentStateCar_2 != previousStateCar_2 && currentStateCar_2 == LOW && zombieState[CAR_ZOMBIE] == ZOMBIE_DOWN) {
    displayText("car * hit");
    digitalWrite(dirPinCar, HIGH);
    zombieState[CAR_ZOMBIE] = ZOMBIE_UP;
    raiseZombie(CAR_ZOMBIE);
  }

  // Update states
  previousStateBoss_1 = currentStateBoss_1;
  previousStateBoss_2 = currentStateBoss_2;
  previousStateCar_1 = currentStateCar_1;
  previousStateCar_2 = currentStateCar_2;
}
