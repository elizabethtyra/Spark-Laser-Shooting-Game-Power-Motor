#include "game_logic.h"
#include "displays.h"
#include "motors.h"

int playerScore = 0;
int playerLives = 3;
unsigned long lastHit[NUM_ZOMBIES] = { -1*COOLDOWN }; // -1*COOLDOWN for inital game start
unsigned long zombieState[NUM_ZOMBIES] = {0}; 
unsigned long startTime = 0;

void gameOver() {
  // while (1);

  if (playerScore > highScores[0]) {
    // topHighScore = playerScore;  // update highest score for leaderboard --> need to write this to memory later
    //write to memory
    highScores[2] = highScores[1];
    highScores[1] = highScores[0];
    highScores[0] = playerScore;

    writeTopScoreToEEPROM(1, highScores[0]);
    writeTopScoreToEEPROM(2, highScores[1]);
    writeTopScoreToEEPROM(3, highScores[2]);

    update_7seg(highScores[0], topScore1);
    update_7seg(highScores[1], topScore2);
    update_7seg(highScores[2], topScore3);
  } else if (playerScore > highScores[1]) {
    highScores[2] = highScores[1];
    highScores[1] = playerScore;

    writeTopScoreToEEPROM(2, highScores[1]);
    writeTopScoreToEEPROM(3, highScores[2]);

    update_7seg(highScores[1], topScore2);
    update_7seg(highScores[2], topScore3);
  } else if (playerScore > highScores[2]) {
    highScores[2] = playerScore;

    writeTopScoreToEEPROM(3, highScores[2]);

    update_7seg(highScores[2], topScore3);
  }

  update_7seg(0, userScore);

  Serial.println("Game Over");
  Serial.print("Player Score: ");
  Serial.print(playerScore);
  while (1)
    ;
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
