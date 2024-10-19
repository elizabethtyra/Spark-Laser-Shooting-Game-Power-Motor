#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define COOLDOWN 7000
#define NPN_PIN 7
#define BUTTON_PIN 9
#define RELOAD_PIN 3
#define BULLETS 200
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

int8_t btn;
unsigned int bulletCount;
// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//oled is the name of the OLED object we just constructed

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
    displayBullets(BULLETS);
    bulletCount = BULLETS;
}

void setup() {
  bulletCount = BULLETS;
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(9600);
    // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  digitalWrite(8, HIGH); // comment out when real laser is used
  pinMode(RELOAD_PIN, INPUT);
  displayBullets(BULLETS);

}

void loop() {
  if(digitalRead(RELOAD_PIN)) {
    reload();
  }

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
