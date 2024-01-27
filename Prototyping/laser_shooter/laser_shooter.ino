#define COOLDOWN 5000
#define NPN_PIN 2
#define BUTTON 4

#define BULLETS 1000

unsigned int num_bullets;

void setup() {
  num_bullets = BULLETS;
  Serial.begin(9600);
}

void loop() {

  if(num_bullets == 0) {
    digitalWrite(2, LOW);
    delay(COOLDOWN);
    num_bullets = BULLETS;
  }

  int8_t btn = digitalRead(BUTTON);
  
  if(btn == HIGH) {
    num_bullets -= 1;
    Serial.println(num_bullets);
    digitalWrite(2, HIGH);
  }
  else {
    digitalWrite(2, LOW);
  }
}
