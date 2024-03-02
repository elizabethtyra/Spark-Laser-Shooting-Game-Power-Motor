#include <ESP32Servo.h>

// defines where all the pins need to be connected
const int OFF_BUTTON_PIN = 5;      // Off button pin
const int RESET_BUTTON_PIN = 17;   // Reset button pin
const int STATUS_LED_PIN = 16;     // Status LED pin
const int SENSOR_PIN = A10;        // Photoresistor pin
const int MOVER_PIN = 2;           // Servo motor control pin

// setting up objects and valuess
Servo mover;
int zombieUp = 90;
int zombieDown = 0;
int lightThres = 200;
int ZOMBIE_STATE = 1; // 1-operating, 0-hit/down, 2-done

void setup() {

  // initialise everything
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP); // internal pull-up resistor
  pinMode(OFF_BUTTON_PIN, INPUT_PULLUP);   // internal pull-up resistor
  mover.attach(MOVER_PIN);

  // set the zombie into operating mode (zombie is up, status is on)
  digitalWrite(STATUS_LED_PIN, HIGH);
  mover.write(zombieUp);

  Serial.begin(9600);
}

void loop() {

  // IN HIT/DOWN MODE - wait till reset /////////////////////////////
  if (ZOMBIE_STATE == 0) {
    // read reset button to see if the zombies can go back up now
    if (digitalRead(RESET_BUTTON_PIN) == HIGH) {
      ZOMBIE_STATE = 1;
      digitalWrite(STATUS_LED_PIN, HIGH);
      mover.write(zombieUp);
    }
  }

  // IN OPERATING MODE - wait till hit //////////////////////////////
  if (ZOMBIE_STATE == 1) {
    // read photoresistor to see if the zombie is getting hit now
    int sensorValue = analogRead(SENSOR_PIN);
    bool gotHit = (sensorValue > lightThres);

    //Serial.println("LIGHT: " + String(sensorValue));

    if (gotHit) { // zombie now goes into hit/down mode
      ZOMBIE_STATE = 0;
      digitalWrite(STATUS_LED_PIN, LOW);
      mover.write(zombieDown);
    }
  }

  if (digitalRead(OFF_BUTTON_PIN) == HIGH) { // any time OFF pressed, we go into this infinite done state
    ZOMBIE_STATE = 2;
    digitalWrite(STATUS_LED_PIN, LOW);
    mover.write(zombieDown);
  }

  //Serial.println(digitalRead(OFF_BUTTON_PIN));
  //Serial.println(digitalRead(RESET_BUTTON_PIN));
  //Serial.println(ZOMBIE_STATE);
  //delay(500);
}