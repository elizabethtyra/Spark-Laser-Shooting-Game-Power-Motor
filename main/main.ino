#include "displays.h"
#include "motors.h"
#include "game_logic.h"

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
  delay(10);
  update_7seg(highScores[1], topScore2);
  delay(10);
  update_7seg(highScores[2], topScore3);
  delay(10);
  update_7seg(0, userScore);
  delay(10);

  // dot matrix 3 lives setup
  livesMatrix.begin();
  livesMatrix.control(MD_MAX72XX::INTENSITY, 0);
  livesMatrix.clear();

  drawInitialLives();


}

void loop() {


}
