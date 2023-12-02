#include <time.h>

#define COOLDOWN 3
#define ACTIVE 1

void setup() {
  // put your setup code here, to run once:
  pinMode(2,OUTPUT);
  pinMode(4, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Turning on");
  digitalWrite(2, HIGH);
  // if(analogRead(4) == HIGH) {

  // }
  delay(5000);
  Serial.println("Turning OFF");
  digitalWrite(2, LOW);
  delay(5000);
}
