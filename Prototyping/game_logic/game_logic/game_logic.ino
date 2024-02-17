#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SENSOR1_PIN 2
#define SENSOR2_PIN 3
// #define SENSOR3_PIN 18
// #define SENSOR4_PIN 19
// #define SENSOR5_PIN 20
// #define SENSOR6_PIN 21
#define NUM_SENSORS 6
#define COOLDOWN 7000
#define GAME_TIME 120000
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define ZOMBIE_DOWN 0
#define ZOMBIE_UP 1

#define NPN_PIN 7
#define BUTTON_PIN 5
#define RELOAD_PIN 3
#define BULLETS 200
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

int playerScore = 0;
int playerLives = 3;
unsigned long lastHit[NUM_SENSORS] = {0};
unsigned long zombieState[NUM_SENSORS] = {1, 1, 1, 1, 1, 1};
unsigned long startTime = 0;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
// our servo # counter
uint8_t servonum = 12;  

// intialize globals for bullet count
int8_t btn;
unsigned int bulletCount;
// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//oled is the name of the OLED object we just constructed


void lowerZombie(int zombieNum) {
  Serial.println("Lowering zombie ");
  Serial.print(zombieNum);
  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
    pwm.setPWM(servonum + zombieNum - 1, 0, pulselen);
  }
}

void raiseZombie(int zombieNum) {
  Serial.println("Raising zombie ");
  Serial.print(zombieNum);
  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
    pwm.setPWM(servonum + zombieNum - 1, 0, pulselen);
  }
}


void processHit(const char* sensor, unsigned long* lastTime, int zombieNum) {
    if (millis() - *(lastTime) > COOLDOWN  && zombieState[zombieNum-1] == ZOMBIE_UP) {
      *(lastTime) = millis();
      playerScore+=10;
      Serial.print(sensor);
      Serial.println(" HIT");
      Serial.println("Player Score: ");
      Serial.print(playerScore);
      zombieState[zombieNum-1] = ZOMBIE_DOWN;
      lowerZombie(zombieNum);
    }
    else {
      // Serial.print(sensor);
      // Serial.println(" on COOLDOWN");
    }
    return;
}

void printSensorState(int sensorPin) {
  switch (sensorPin) {
    case SENSOR1_PIN: {
      processHit("Sensor 1", &lastHit[0], 1);
      break;
    }
    case SENSOR2_PIN: {
      processHit("Sensor 2", &lastHit[1], 2);
      break;
    }
    // case SENSOR3_PIN: {
    //   processHit("Sensor 3", &lastHit[2], 3);
    //   break;
    // }
    // case SENSOR4_PIN: {
    //   processHit("Sensor 4", &lastHit[3], 4);
    //   break;
    // }
    // case SENSOR5_PIN: {
    //   processHit("Sensor 5", &lastHit[4], 5);
    //   break;
    // }
    // case SENSOR6_PIN: {
    //   processHit("Sensor 6", &lastHit[5], 6);
    //   break;
    // }
    default:
      break;
  }
}

void gameOver() {
    Serial.println("Game Over");
    Serial.print("Player Score: ");
    Serial.print(playerScore);
    while(1);
}

// You can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. It's not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= SERVO_FREQ;   // Analog servos run at ~60 Hz updates
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

//for bullet count display
void displayBullets(int bullets) {
  oled.clearDisplay(); //remember to clear screen
  oled.setTextSize(4);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(30, 10);        // position to display
  oled.println(bullets); // text to display
  oled.display();
}

void displayText(const char* text) {
  oled.clearDisplay(); //clear display
  oled.setTextSize(2);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0, 0);        // position to display
  oled.println(text);
  oled.display();
}

void reload() {
    Serial.println("Reloading");
    displayText("Reloading");
    delay(1000);
    displayBullets(200);
    bulletCount = BULLETS;
}

void checkBulletCount() {
  if(bulletCount == 0) {
    digitalWrite(NPN_PIN, LOW);
    Serial.println("bullet count 0");
    displayText("Reloading");
    delay(COOLDOWN);
    bulletCount = BULLETS;
    displayBullets(bulletCount);
  }

  btn = digitalRead(BUTTON_PIN);
  
  if(btn == HIGH && bulletCount > 0) {
    bulletCount -= 1;
    displayBullets(bulletCount);
    Serial.println(bulletCount);
    digitalWrite(NPN_PIN, HIGH);
  }
  else {
    digitalWrite(NPN_PIN, LOW);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // initialize how many bits/s get communicated to the Serial monitor
  pinMode(SENSOR1_PIN, INPUT);
  pinMode(SENSOR2_PIN, INPUT);
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
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);
  startTime = millis();

    bulletCount = BULLETS;
    pinMode(BUTTON_PIN, INPUT);
    Serial.begin(9600);
      // initialize OLED display with address 0x3C for 128x64
    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 allocation failed"));
      while (true);
    }
    //digitalWrite(8, HIGH); // comment out when real laser is used
    pinMode(RELOAD_PIN, INPUT);
    displayBullets(200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(SENSOR1_PIN)) {
    printSensorState(SENSOR1_PIN);
  }
  else if (digitalRead(SENSOR2_PIN)) {
    printSensorState(SENSOR2_PIN);
  }

  for(int i = 0; i < NUM_SENSORS; i++) {
    if(millis() - lastHit[i] > COOLDOWN && zombieState[i] == ZOMBIE_DOWN) {
      raiseZombie(i+1);
      zombieState[i] = ZOMBIE_UP;
    }
  }

  if(digitalRead(RELOAD_PIN)) {
    reload();
  }

  checkBulletCount();

  if((millis() - startTime >= GAME_TIME) || (playerLives == 0)) {
    gameOver();
  }
}