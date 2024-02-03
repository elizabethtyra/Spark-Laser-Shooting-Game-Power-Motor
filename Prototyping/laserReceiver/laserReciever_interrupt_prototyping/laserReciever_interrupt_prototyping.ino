#define SENSOR1_PIN 2
#define SENSOR2_PIN 3

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // initialize how many bits/s get communicated to the Serial monitor
  pinMode(SENSOR1_PIN, INPUT);
  pinMode(SENSOR2_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SENSOR1_PIN), printSensorState, RISING);
  attachInterrupt(digitalPinToInterrupt(SENSOR2_PIN), printSensorState, RISING);
}

void printSensorState() {
  if(digitalRead(SENSOR1_PIN)) {
    Serial.println("SENSOR 1: ");
    Serial.println(digitalRead(SENSOR1_PIN));
  }
  if(digitalRead(SENSOR2_PIN)) {
    Serial.println("SENSOR 2: ");
    Serial.println(digitalRead(SENSOR2_PIN));
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(7, HIGH);
  // if(digitalRead(SENSOR2_PIN)) {
  //   Serial.println("SENSOR 2: ");
  //   Serial.println(digitalRead(SENSOR2_PIN));
  // }
}
