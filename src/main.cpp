#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include "nRF24L01.h"
#include "serial_io.h"

#define BAUD_RATE 115200
#define CE 26
#define CSN 25

/* create an instance of the radio */
nRF24Module::nRF24 radio(CE, CSN);
SerialIO io;
volatile bool transmitting {false}; // placeholder for nRF states

/* prototypes */
void ISR_Antenna(void);


void setup() {
  SPI.begin();

  /* Antenna is active low */
  pinMode(A2, PULLUP);
  attachInterrupt(digitalPinToInterrupt(A2), ISR_Antenna, RISING);
  Serial.begin(BAUD_RATE);
}

void loop() {
  switch (io.getBoardState()) {
  case CONFIG:
    while (Serial.available()) {
      io.setConfig();
    }

    break;
  
  case READY:

    if (!transmitting) {
      io.sendFile();
      transmitting = true;
    } else {
      io.setExtension();
      char * ext = io.getExtension();
      uint8_t * addr = io.getAddress();
      uint8_t channel = io.getChannel();

      radio.setToTransmitter();
      radio.setListeningAddr(addr);
      radio.setChannel(channel);
      delay(1000);
      io.print(radio.getChannel());

      delay(1000);
      io.print(radio.getChannel());

      delay(1000);
      io.print(radio.getChannel());

      delay(1000);
      io.print(radio.getChannel());

      delay(1000);
      io.print(radio.getChannel());

      while (true) {
        radio.writeSPI((byte *)ext, EXTENSION_BYTES);
      }
      transmitting = false;
    }
    break;
  
  /* We have to be in CONFIG or READY */
  default:
    break; 
  } 
}


/*
 * TODO: ISR for IRQ pin on antenna
 */
void ISR_Antenna() {
}
