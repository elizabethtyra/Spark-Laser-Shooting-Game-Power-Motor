#define SENSOR1_PIN 2
// #define SENSOR2_PIN 3
// #define SENSOR3_PIN 18
// #define SENSOR4_PIN 19
// #define SENSOR5_PIN 20
// #define SENSOR6_PIN 21
#define NUM_SENSORS 6
#define COOLDOWN 7000



int playerScore = 0;
unsigned long lastHit[6] = {0};

void processHit(const char* sensor, unsigned long* lastTime) {
    if (millis() - *(lastTime) > COOLDOWN) {
      *(lastTime) = millis();
      playerScore+=10;
      Serial.print(sensor);
      Serial.println(" HIT");
    }
    else {
      Serial.print(sensor);
      Serial.println(" on COOLDOWN");
    }
    return;
}

void printSensorState(int sensorPin) {
  switch (sensorPin) {
    case SENSOR1_PIN: {
      processHit("Sensor 1", &lastHit[0]);
      break;
    }
    // case SENSOR2_PIN: {
    //   processHit("Sensor 2", &lastHit[1]);
    //   break;
    // }
    // case SENSOR3_PIN: {
    //   processHit("Sensor 3", &lastHit[2]);
    //   break;
    // }
    // case SENSOR4_PIN: {
    //   processHit("Sensor 4", &lastHit[3]);
    //   break;
    // }
    // case SENSOR5_PIN: {
    //   processHit("Sensor 5", &lastHit[4]);
    //   break;
    // }
    // case SENSOR6_PIN: {
    //   processHit("Sensor 6", &lastHit[5]);
    //   break;
    // }
    default:
      break;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // initialize how many bits/s get communicated to the Serial monitor
  pinMode(SENSOR1_PIN, INPUT);
  // pinMode(SENSOR2_PIN, INPUT);
  // pinMode(SENSOR3_PIN, INPUT);
  // pinMode(SENSOR4_PIN, INPUT);
  // pinMode(SENSOR5_PIN, INPUT);
  // pinMode(SENSOR6_PIN, INPUT);
  // attachInterrupt(digitalPinToInterrupt(SENSOR1_PIN), []{ printSensorState(SENSOR1_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR2_PIN), []{ printSensorState(SENSOR2_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR3_PIN), []{ printSensorState(SENSOR3_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR4_PIN), []{ printSensorState(SENSOR4_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR5_PIN), []{ printSensorState(SENSOR5_PIN); }, RISING);
  // attachInterrupt(digitalPinToInterrupt(SENSOR6_PIN), []{ printSensorState(SENSOR6_PIN); }, RISING);
  // Testing laser
  digitalWrite(7, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(SENSOR1_PIN)) {
    printSensorState(SENSOR1_PIN);
  }
}
