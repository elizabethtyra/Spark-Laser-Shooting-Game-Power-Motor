#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define COOLDOWN 5000
#define NPN_PIN 2
#define BUTTON_PIN 4
#define BULLETS 200
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define NUM_BULLETS 200

unsigned int bulletCount;
// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//oled is the name of the OLED object we just constructed

void setup() {
  bulletCount = BULLETS;
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(9600);
    // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  } 
}

void loop() {

  oled.clearDisplay(); //clear display
  oled.setTextSize(4);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(30, 10);        // position to display

  if(bulletCount == 0) {
    digitalWrite(2, LOW);
    Serial.print("Reloading");
    oled.setCursor(30, 10);        // position to display
    oled.println(bulletCount); // text to display
    delay(COOLDOWN);
    bulletCount = BULLETS;
    oled.clearDisplay(); //remember to clear screen
  }

  int8_t btn = digitalRead(BUTTON_PIN);
  
  if(btn == HIGH) {
    bulletCount -= 1;
    oled.setCursor(30, 10);        // position to display
    oled.println(bulletCount); // text to display
    oled.display();
    Serial.println(bulletCount);
    digitalWrite(2, HIGH);
    oled.clearDisplay(); //remember to clear screen
  }
  else {
    digitalWrite(2, LOW);
  }
}
