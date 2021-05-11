#include <Wire.h>
#include <SPI.h>
#include <U8g2lib.h>
#include "logo.h"
#include "bien.h"
#include "pasbien.h"
U8G2_SH1107_SEEED_128X128_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
const int analogInPin = A0;
int sensorValue = 0;


  
// End of constructor list


void setup(void) {
  Serial.begin(115200);
  pinMode(PIN_GROVE_POWER, OUTPUT);
  digitalWrite(PIN_GROVE_POWER, 1);
  u8g2.begin();
  delay(2000);
  drawLogo();
  delay(2000);
}

void loop(void) {
  sensorValue = analogRead(analogInPin);
  if (sensorValue > 500){
  drawBien();
  delay(1000);
  }
  if (sensorValue < 500){
  drawpasBien();
  delay(1000);
  }
}

void drawLogo() {
  u8g2.firstPage();
  do {
    u8g2.drawXBMP(0, 0, logo_width, logo_height, logo);
  } while ( u8g2.nextPage() );
}

void drawBien() {
  u8g2.firstPage();
  do {
    u8g2.drawXBMP(0, 0, logo_width, logo_height, bien);
  } while ( u8g2.nextPage() );
}
void drawpasBien() {
  u8g2.firstPage();
  do {
    u8g2.drawXBMP(0, 0, logo_width, logo_height, pasbien);
  } while ( u8g2.nextPage() );
}
