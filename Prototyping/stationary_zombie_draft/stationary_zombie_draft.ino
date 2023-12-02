#include <Servo.h>
Servo servo1_stationaryZombie; //assuming stationary zombie is attached to 1 servo motor
const int SERVO_PIN;
const int STATUS_LED_PIN;
const int SENSOR_PIN;

int lightThres = 200; // Photoresistor reading values
int ZOMBIE_STATE = 0; //0 = facing upwards towards user (default), 1 = facing downwards (after hit)
int gameScore = 0;
void setup() {
  // put your setup code here, to run once:
    pinMode(STATUS_LED_PIN, OUTPUT);
    servo1_stationaryZombie.attach(SERVO_PIN);
    Serial.begin(9600);
    /* This was mainly copied from boss zombie code so I don't know if this is correct for this context
    */
}

void loop() {
  // put your main code here, to run repeatedly:
  if(ZOMBIE_STATE == 0) { //zombie is in default position (up)
    int sensorValue = analogRead(SENSOR_PIN);
    bool gotHit = (sensorValue > lightThres);
    if (gotHit) { // zombie should rotate downwards
      ZOMBIE_STATE = 1; 
      servo1_stationaryZombie.write(90);
      gameScore += 1;
    }
  }
  else if(ZOMBIE_STATE == 1) {
    delayMicroseconds(3000); //hold down position for three seconds
    servo1_stationaryZombie.write(-90);
    ZOMBIE_STATE = 0;
  }
}
