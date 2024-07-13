#include "displays.h"
#include "motors.h"
#include "game_logic.h"
#include "parameters.h"

void setup() {
  Serial.begin(9600);  // initialize how many bits/s get communicated to the Serial monitor

  // timer2 interrupt setup
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

  // 7 segment display setup
  timerMatrix.begin(0x70);
  topScore1.begin(0x71);
  topScore2.begin(0x72);
  topScore3.begin(0x73);
  userScore.begin(0x74);
  Serial.println("Matrix Setup Finished.");
  timer_count = 0;
  playerScore = 0;

  // init leaderboard
  highScores[0] = readEEPROMTopScore(1);
  highScores[1] = readEEPROMTopScore(2);
  highScores[2] = readEEPROMTopScore(3);
  update_7seg(highScores[0], topScore1);
  update_7seg(highScores[1], topScore2);
  update_7seg(highScores[2], topScore3);
  update_7seg(0, userScore);

  // dot matrix 3 lives setup
  livesMatrix.begin();
  livesMatrix.control(MD_MAX72XX::INTENSITY, 0);
  livesMatrix.clear();

  drawInitialLives();

  /**************** SENSOR PINS ***************/
  pinMode(SENSOR0_PIN, INPUT);
  pinMode(SENSOR1_PIN, INPUT);
  pinMode(SENSOR2_PIN, INPUT);
  pinMode(SENSOR3_PIN, INPUT);
  pinMode(SENSORBOSS_PIN, INPUT);
  pinMode(SENSORCAR_PIN, INPUT);

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

  digitalWrite(dirPinBoss, HIGH);         // ASSUME THIS IS THE RIGHT WAY OTHERWISE WILL SWITCH
  digitalWrite(dirPinCar, HIGH);         // ASSUME THIS IS THE RIGHT WAY OTHERWISE WILL SWITCH

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
  else if (digitalRead(SENSORBOSS_PIN2)) {
    printSensorState(SENSORBOSS_PIN2);
  }
  else if (digitalRead(SENSORCAR_PIN)) {
    printSensorState(SENSORCAR_PIN);
  }
  else if (digitalRead(SENSORCAR_PIN2)) {
    printSensorState(SENSORCAR_PIN2);
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
