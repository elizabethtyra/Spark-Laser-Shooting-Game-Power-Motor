#include <Stepper.h>

// Define stuff for both stepper motors
const int stepsPerRevolution = 1000; 
const int stopDelay = 2000; // How long to wait once reached the end (to handle lives and stuff)
const int stepDelay_high_low = 2500; // Delay for clockwise rotation
const int stepDelay_low_high = 2500; // Delay for counterclockwise rotation
unsigned long delayNum = 2500;


// Define stuff for only STEPPER1
const int stepPin1 = 4;
const int dirPin1 = 5;
const int limitSwitch1_1 = 6; // Limit switch for stepper 1 in one direction
const int limitSwitch2_1 = 8; // Limit switch for stepper 1 in the other direction
// Stepper stepper1(stepsPerRevolution, stepPin1, dirPin1);

// Define stuff for only STEPPER2
const int stepPin2 = 2;
const int dirPin2 = 3;
const int limitSwitch1_2 = 7; // Limit switch for stepper 2 in one direction
const int limitSwitch2_2 = 9; // Limit switch for stepper 2 in the other direction
// Stepper stepper2(stepsPerRevolution, stepPin2, dirPin2);


void setup() {

  // STEPPER1 SET setup
  pinMode(limitSwitch1_1, INPUT_PULLUP);
  pinMode(limitSwitch2_1, INPUT_PULLUP);
  pinMode(stepPin1, OUTPUT);
	pinMode(dirPin1, OUTPUT);
  digitalWrite(dirPin1, HIGH); // starts same direction

  // STEPPER2 SET setup
  pinMode(limitSwitch1_2, INPUT_PULLUP);
  pinMode(limitSwitch2_2, INPUT_PULLUP);
  pinMode(stepPin2, OUTPUT);
	pinMode(dirPin2, OUTPUT);
  digitalWrite(dirPin2, HIGH); // starts same direction

  Serial.begin(9600);
}


void loop() {

  // Step both STEPPER1 and STEPPER2
  // digitalWrite(stepPin1, HIGH);
  // digitalWrite(stepPin2, HIGH);
  // delayMicroseconds(stepDelay_high_low);
  // digitalWrite(stepPin1, LOW);
  // digitalWrite(stepPin2, LOW);
  // delayMicroseconds(stepDelay_low_high);

    for(int x = 0; x < stepsPerRevolution; x++)
    {
        digitalWrite(stepPin1, HIGH);
        digitalWrite(stepPin2, HIGH);

        delayMicroseconds(delayNum);
        digitalWrite(stepPin1, LOW);
        digitalWrite(stepPin2, LOW);

        delayMicroseconds(delayNum);

        // Check STEPPER1 Limit Switches
        if (digitalRead(limitSwitch1_1) == HIGH) {
          // delay(stopDelay); // wait for the zombie to do stuff -> call zombie function here
          digitalWrite(dirPin1, HIGH);  
          // delay(stopDelay); // wait for the zombie to do stuff -> call zombie function here
          // digitalWrite(stepPin1, HIGH);
          // delayMicroseconds(2000);
          // digitalWrite(stepPin1, LOW);
          delay(stopDelay);
          }
        if (digitalRead(limitSwitch2_1) == HIGH) {
          digitalWrite(dirPin2, LOW);  
          delay(stopDelay); // wait for the zombie to do stuff -> call zombie function here
          }

        // // Check STEPPER2 Limit Switches
        if (digitalRead(limitSwitch1_2) == HIGH) {
          // delay(stopDelay);
          digitalWrite(dirPin1, LOW);  
          delay(stopDelay); // wait for the zombie to do stuff -> call zombie function here
        }
        // if (digitalRead(limitSwitch2_2) == LOW) {
        //   // delay(stopDelay);
        //   digitalWrite(dirPin2, LOW);  
        //   delay(stopDelay); // wait for the zombie to do stuff -> call zombie function here
        // }
    }

}
