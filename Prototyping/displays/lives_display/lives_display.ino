<<<<<<< HEAD
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

void setup() {
  Serial.begin(9600);
  Serial.println("8x8 LED Matrix Test");
  
  matrix.begin(0x70);  // pass in the address
}

static const uint8_t PROGMEM
  heart_empty[] =
   {
    
     B00000000,
     B01100110,
     B10011001,
     B10000001,
     B10000001,
     B01000010,
     B00100100,
     B00011000,
    
     },

     heart_fill[] =
     {
      B00000000,
      B01100110,
     B11111111,
     B11111111,
     B11111111,
     B01111110,
     B00111100,
     B00011000,
     };
 

void loop() {
  matrix.clear();
  matrix.drawBitmap(0, 0, heart_empty, 8, 8, LED_RED);
 // matrix.drawBitmap(0, 0, heart_fill, 8, 8, LED_RED);
  matrix.writeDisplay();
  delay(500);
   matrix.setRotation(0);
}
=======
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

void setup() {
  Serial.begin(9600);
  Serial.println("8x8 LED Matrix Test");
  
  matrix.begin(0x70);  // pass in the address
}

static const uint8_t PROGMEM
  smile_bmp[] =
   {
    
     B00000000,
     B01100110,
     B10011001,
     B10000001,
     B10000001,
     B01000010,
     B00100100,
     B00011000,
     
    
     };
 

void loop() {
  matrix.clear();
  matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_RED);
  matrix.writeDisplay();
  delay(500);
   matrix.setRotation(0);
}
>>>>>>> 5fd669d15092ac3f45d189075c8a3ef7abcf4501
