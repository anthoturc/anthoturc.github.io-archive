#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include "nRF24L01.h"
#include "serial_io.h"

#define CE 26
#define CSN 25

/* create an instance of the radio */
nRF24Module::nRF24 radio(CE, CSN);
SerialIO io;

#define BAUD_RATE 115200
#define CONFIRMATION_CHAR '\t'  // used to communicate state changes between the Arduino and Computer

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
      io.readConfig();
    }
    break;
  
  case READY:
    if (!io.is_config_printed) {
      io.printConfig();
      io.is_config_printed = true;
    }

    while (Serial.available()) {
      volatile char curr_char = (char) (Serial.read());
      if (!transmitting) {

        /*
         * If the computer tells us it is ready, we respond to tell 
         * it we are now ready to transmit.
         */
        if (curr_char == CONFIRMATION_CHAR) {
          io.sendConfirmation();
          transmitting = true;
          sleep(1); // so that both computer and Arduino not listening at same time
          break;
        }
      } else {
        io.transmit(curr_char);
      }
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
