#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES 3

//Hardware SPI pins for Arduino UNO
// CLK Pin  > 13 SCK
// Data Pin > 11 MOSI

#define CS_PIN    10

// Hardware SPI connection
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

byte heart_fill[8] = {0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18};
byte heart_empty[8] = {0x00, 0x66, 0x99, 0x81, 0x81, 0x42, 0x24, 0x18};

void setup() {  
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 0);
  mx.clear();

}

void loop() {
  drawShape();
  
}

void drawShape() {
  for (int i = 0; i < 8; i++) {
    mx.setRow(0, 0, i, heart_fill[i]);
  }
  
  for (int i = 0; i < 8; i++) {
    mx.setRow(1, 1, i, heart_empty[i]);
  }

  for (int i = 0; i < 8; i++) {
    mx.setRow(2, 2, i, heart_empty[i]);
  }

}
