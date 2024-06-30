#include "game_logic.h"
#include "displays.h"
#include "motors.h"

int playerScore;

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
    
    update_7seg(highScores[0], topScore1);
    update_7seg(highScores[1], topScore2);
    update_7seg(highScores[2], topScore3);
  }
  else if (playerScore > highScores[1]) {
    highScores[2] = highScores[1];
    highScores[1] = playerScore;

    writeTopScoreToEEPROM(2, highScores[1]);
    writeTopScoreToEEPROM(3, highScores[2]);
    
    update_7seg(highScores[1], topScore2);
    update_7seg(highScores[2], topScore3);
  }
  else if (playerScore > highScores[2]) {
    highScores[2] = playerScore;

    writeTopScoreToEEPROM(3, highScores[2]);

    update_7seg(highScores[2], topScore3);
  }

  update_7seg(0, userScore);

  Serial.println("Game Over");
  Serial.print("Player Score: ");
  Serial.print(playerScore);
  while (1);
}
