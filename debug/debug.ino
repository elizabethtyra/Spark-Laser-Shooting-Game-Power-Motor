#include "displays.h"
#include "motors.h"
#include "game_logic.h"
#include "parameters.h"
// #include <Adafruit_SSD1306.h>

// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64

// Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// void displayText(const char* text) {
//   oled.clearDisplay();       //clear display
//   oled.setTextSize(2);       // text size
//   oled.setTextColor(WHITE);  // text color
//   oled.setCursor(0, 0);      // position to display
//   oled.println(text);
//   oled.display();
// }

void setup() {
  Serial.begin(115200);  // initialize how many bits/s get communicated to the Serial monitor

  // initialize OLED display with address 0x3C for 128x64
  // if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  //   Serial.println("SSD1306 allocation failed");
  //   while (true)
  //     ;
  // }

  // displayText("In Setup");

  // timer2 interrupt setup
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
  Serial.println("TIMER2 Setup Finished.");

  // 7 segment display setup
  timerMatrix.begin(0x70);
  // topScore1.begin(0x71);
  // topScore2.begin(0x72);
  // topScore3.begin(0x73);
  userScore.begin(0x74);
  Serial.println("Matrix Setup Finished.");
  timer_count = 0;
  playerScore = 0;

  // init leaderboard
  // highScores[0] = readEEPROMTopScore(1);
  // highScores[1] = readEEPROMTopScore(2);
  // highScores[2] = readEEPROMTopScore(3);
  // update_7seg(highScores[0], topScore1);
  // update_7seg(highScores[1], topScore2);
  // update_7seg(highScores[2], topScore3);
  update_7seg(0, userScore);

  // writeTopScoreToEEPROM(3, 000);
  // highScores[2] = readEEPROMTopScore(3);
  // update_7seg(highScores[2], topScore3);



  // dot matrix 3 lives setup
  livesMatrix.begin();
  livesMatrix.control(MD_MAX72XX::INTENSITY, 0);
  livesMatrix.clear();

  // displayText("After live matrix setup");


  delay(100);

  drawInitialLives();

  /**************** SENSOR PINS ***************/
  pinMode(SENSOR0_PIN, INPUT);
  pinMode(SENSOR1_PIN, INPUT);
  pinMode(SENSOR2_PIN, INPUT);
  // pinMode(SENSOR3_PIN, INPUT);
  // pinMode(SENSORBOSS_PIN, INPUT);
  // pinMode(SENSORCAR_PIN, INPUT);

  // STEPPER1 SET setup
  pinMode(limitSwitchBoss_1, INPUT_PULLUP);
  pinMode(limitSwitchBoss_2, INPUT_PULLUP);
  pinMode(stepPinBoss, OUTPUT);
  pinMode(dirPinBoss, OUTPUT);
  digitalWrite(dirPinBoss, HIGH);  // starts same direction

  // STEPPER2 SET setup
  pinMode(limitSwitchCar_1, INPUT_PULLUP);
  pinMode(limitSwitchCar_2, INPUT_PULLUP);
  pinMode(stepPinCar, OUTPUT);
  pinMode(dirPinCar, OUTPUT);
  digitalWrite(dirPinCar, HIGH);  // starts same direction



  startTime = timer_count;
  Serial.println("Raising all stationary zombies");

  // pwm.begin();
  // delay(1000);
  // pwm.setOscillatorFrequency(27000000);
  // pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  // displayText("end of setup");



  /**************** RESET STATIONARY ZOMBIES ***************/
  // for (int i = 0; i < STATIONARY_ZOMBIES; i++) {
  //   raiseZombie(i);
  //   zombieState[i] = ZOMBIE_UP;
  // }

  digitalWrite(dirPinBoss, HIGH);  // ASSUME THIS IS THE RIGHT WAY OTHERWISE WILL SWITCH
  digitalWrite(dirPinCar, HIGH);   // ASSUME THIS IS THE RIGHT WAY OTHERWISE WILL SWITCH
}

void loop() {
  /****************GAME END***************/
  // displayText("In loop");

  if ((timer_count >= GAME_TIME) || (playerLives == 0)) {
    // for (int i = 0; i < STATIONARY_ZOMBIES; i++) {
    //   if (zombieState[i] == ZOMBIE_DOWN) {
    //     raiseZombie(i);
    //     zombieState[i] = ZOMBIE_UP;
    //   }
    for (int i = 0; i < STATIONARY_ZOMBIES; i++) {
      lowerZombie(i);
      // zombieState[i] = ZOMBIE_UP;
    }
    // }
    while (1)
      ;
    // gameOver();
  }

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
  }

  if (timer_count - lastHit[1] > COOLDOWN && digitalRead(SENSOR1_PIN)) {
    printSensorState(SENSOR1_PIN);
  }

  if (timer_count - lastHit[2] > COOLDOWN && digitalRead(SENSOR2_PIN)) {
    printSensorState(SENSOR2_PIN);
  }
  // else if (timer_count - lastHit[3] > COOLDOWN) {
  //   if(digitalRead(SENSOR3_PIN)) {
  //     printSensorState(SENSOR3_PIN);
  //   }
  // }
  // else if (digitalRead(SENSORBOSS_PIN)) {
  //   printSensorState(SENSORBOSS_PIN);
  // }
  // else if (digitalRead(SENSORBOSS_PIN2)) {
  //   printSensorState(SENSORBOSS_PIN2);
  // }
  // else if (digitalRead(SENSORCAR_PIN)) {
  //   printSensorState(SENSORCAR_PIN);
  // }
  // else if (digitalRead(SENSORCAR_PIN2)) {
  //   printSensorState(SENSORCAR_PIN2);
  // }


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


  // BOSS ZOMBIE LIMIT SWITCHES
  if (currentStateBoss_1 != previousStateBoss_1) {
    if (currentStateBoss_1 == LOW) {
      digitalWrite(dirPinBoss, HIGH);
      playerLives--;
      updateLives(playerLives);
      zombieState[4] = 1;
    }
  }

  if (currentStateBoss_2 != previousStateBoss_2) {
    if (currentStateBoss_2 == LOW) {
      digitalWrite(dirPinBoss, LOW);
      playerLives--;
      updateLives(playerLives);
      zombieState[4] = 0;
    }
  }

  // CAR ZOMBIE LIMIT SWITCHES
  if (currentStateCar_1 != previousStateCar_1) {
    if (currentStateCar_1 == LOW) {
      digitalWrite(dirPinCar, HIGH);
      zombieState[5] = 1;
    }
  }

  if (currentStateCar_2 != previousStateCar_2) {
    if (currentStateCar_2 == LOW) {
      digitalWrite(dirPinCar, LOW);
      zombieState[5] = 0;
    }
  }

  // Update states
  previousStateBoss_1 = currentStateBoss_1;
  previousStateBoss_2 = currentStateBoss_2;
  previousStateCar_1 = currentStateCar_1;
  previousStateCar_2 = currentStateCar_2;
}
