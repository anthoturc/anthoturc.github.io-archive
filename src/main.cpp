#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include <nRF24L01.h>

/* 8MHZ */
#define SCK_FREQ_nRF24L01  (8 * 1000000)
#define SS A1

void ISR_Antenna(void);

void setup() {
  Serial.begin(115200);
  SPI.begin();

  /* Antenna is active low */
  pinMode(A0, PULLUP);

  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);

  attachInterrupt(digitalPinToInterrupt(A0), ISR_Antenna, RISING);
  
}

void loop() {
  SPI.beginTransaction(SPISettings(SCK_FREQ_nRF24L01, MSBFIRST, SPI_MODE0));
  digitalWrite(SS, LOW);
  
  /* transfer that data */

  digitalWrite(SS, HIGH);
}

void ISR_Antenna() {
  Serial.println("Hello World");
}