#include "game_logic.h"
#include "displays.h"
#include "motors.h"

int playerScore = 0;
int playerLives = 3;
unsigned long lastHit[NUM_ZOMBIES] = { -1 * COOLDOWN };  // -1*COOLDOWN for inital game start
unsigned long zombieState[NUM_ZOMBIES] = { 0 };
unsigned long startTime = 0;

void (*resetFunc)(void) = 0;

void gameOver() {
  // while (1);

  // if (playerScore > highScores[0]) {
  //   // topHighScore = playerScore;  // update highest score for leaderboard --> need to write this to memory later
  //   //write to memory
  //   highScores[2] = highScores[1];
  //   highScores[1] = highScores[0];
  //   highScores[0] = playerScore;

  //   writeTopScoreToEEPROM(1, highScores[0]);
  //   writeTopScoreToEEPROM(2, highScores[1]);
  //   writeTopScoreToEEPROM(3, highScores[2]);

  //   update_7seg(highScores[0], topScore1);
  //   update_7seg(highScores[1], topScore2);
  //   update_7seg(highScores[2], topScore3);
  // } else if (playerScore > highScores[1]) {
  //   highScores[2] = highScores[1];
  //   highScores[1] = playerScore;

  //   writeTopScoreToEEPROM(2, highScores[1]);
  //   writeTopScoreToEEPROM(3, highScores[2]);

  //   update_7seg(highScores[1], topScore2);
  //   update_7seg(highScores[2], topScore3);
  // } else if (playerScore > highScores[2]) {
  //   highScores[2] = playerScore;

  //   writeTopScoreToEEPROM(3, highScores[2]);

  //   update_7seg(highScores[2], topScore3);
  // }

  // update_7seg(0, userScore);

  // Serial.println("Game Over");
  // Serial.print("Player Score: ");
  // Serial.print(playerScore);

  bool bossResetComplete = false;
  bool carResetComplete = false;

  while (1) {

    // Keep moving the zombies if not reset
    if (!bossResetComplete) {
      digitalWrite(stepPinBoss, HIGH);
      delayMicroseconds(delayNum);
      digitalWrite(stepPinBoss, LOW);
    }

    if (!carResetComplete) {
      digitalWrite(stepPinCar, HIGH);
      delayMicroseconds(delayNum);
      digitalWrite(stepPinCar, LOW);
    }


    // Debouncing switches
    currentStateBoss_1 = digitalRead(limitSwitchBoss_1);
    currentStateBoss_2 = digitalRead(limitSwitchBoss_2);
    currentStateCar_1 = digitalRead(limitSwitchCar_1);
    currentStateCar_2 = digitalRead(limitSwitchCar_2);

    // LOW = CCW = ZOMBIE_UP = 0
    // BOSS ZOMBIE LIMIT SWITCHES
    //TODO: Have to add zombie flipping back
    if (currentStateBoss_1 != previousStateBoss_1 && currentStateBoss_1 == LOW && !bossResetComplete) {
      digitalWrite(dirPinBoss, HIGH);
      lowerZombie(BOSS_ZOMBIE);
    }

    if (currentStateBoss_2 != previousStateBoss_2 && currentStateBoss_2 == LOW && !bossResetComplete) {
      digitalWrite(dirPinBoss, LOW);
      bossResetComplete = true;
      raiseZombie(BOSS_ZOMBIE);
    }

    // CAR ZOMBIE LIMIT SWITCHES
    if (currentStateCar_1 != previousStateCar_1 && currentStateCar_1 == LOW && !carResetComplete) {
      digitalWrite(dirPinCar, HIGH);
      carResetComplete = true;
      lowerZombie(CAR_ZOMBIE);
    }

    if (currentStateCar_2 != previousStateCar_2 && currentStateCar_2 == LOW && !carResetComplete) {
      digitalWrite(dirPinCar, LOW);
      raiseZombie(CAR_ZOMBIE);
    }

    // Update states
    previousStateBoss_1 = currentStateBoss_1;
    previousStateBoss_2 = currentStateBoss_2;
    previousStateCar_1 = currentStateCar_1;
    previousStateCar_2 = currentStateCar_2;

    if (bossResetComplete && carResetComplete) {
      break;
    }
  }

  // Maybe make the zombies move for a bit longer to ensure they're not at the complete end

  resetFunc();
}

void processHit(const char* sensor, unsigned long* lastTime, int zombieNum, bool moving) {
  // Process stationary zombies by updating score + zombie state
  if (!moving) {
    if (zombieState[zombieNum] == ZOMBIE_UP) {
      *(lastTime) = timer_count;
      playerScore += 10;
      Serial.print(sensor);
      Serial.println(" HIT");
      Serial.println("Player Score: ");
      Serial.println(playerScore);
      update_7seg(playerScore, userScore);
      zombieState[zombieNum] = ZOMBIE_DOWN;
      lowerZombie(zombieNum);
    }
  }

  // Process moving zombies by changing direction in addition to score
  else if (moving) {
    *(lastTime) = timer_count;
    playerScore += 10;
    update_7seg(playerScore, userScore);
    Serial.print(sensor);
    Serial.println(" HIT");
    Serial.println("Player Score: ");
    Serial.println(playerScore);
    // Toggle direction (flips between 1 and 0)
    // Then write direction to stepper boss/car
    zombieState[zombieNum] = 1 - zombieState[zombieNum];
    Serial.println("Direction switched");
    zombieState[zombieNum] == ZOMBIE_UP ? raiseZombie(zombieNum) : lowerZombie(zombieNum);
    if (zombieNum == 4) {
      digitalWrite(dirPinBoss, zombieState[zombieNum]);
    } else if (zombieNum == 5) {
      digitalWrite(dirPinCar, zombieState[zombieNum]);
    }
  }
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
    case SENSOR2_PIN:
      {
        processHit("Sensor 2", &lastHit[2], 2, false);
        break;
      }
    case SENSOR3_PIN:
      {
        processHit("Sensor 3", &lastHit[3], 3, false);
        break;
      }
    case SENSORBOSS_PIN:
      {
        processHit("Sensor Boss 1", &lastHit[4], 4, true);
        break;
      }
    case SENSORBOSS_PIN2:
      {
        processHit("Sensor Boss 2", &lastHit[4], 4, true);
        break;
      }
    case SENSORCAR_PIN:
      {
        processHit("Sensor Car 1", &lastHit[5], 5, true);
        break;
      }
    case SENSORCAR_PIN2:
      {
        processHit("Sensor Car 2", &lastHit[5], 5, true);
        break;
      }
    default:
      break;
  }
}
