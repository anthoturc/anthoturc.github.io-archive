#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include "nRF24L01.h"
#include "serial_io.h"

// // #define BAUD_RATE 115200
#define CE 26
#define CSN 25
#define DEBUG 0

// /* create an instance of the radio */
nRF24Module::nRF24 radio(CE, CSN);
SerialIO io;
volatile bool transmitting {false}; // placeholder for nRF states

/* prototypes */
// void ISR_Antenna(void);


void setup() {
  SPI.begin();

  /* Antenna is active low */
  pinMode(A2, PULLUP);
  // attachInterrupt(digitalPinToInterrupt(A2), ISR_Antenna, RISING);
  Serial.begin(BAUD_RATE);
  
  io.setConfig();
}

void loop() {

  /* THIS SHOULD BE DONE WITH NRF STATE IN CLASS */
  if (!transmitting) {
    // nRF receiving

    /* THIS SHOULD BE DONE WITH NRF STATE IN CLASS */
    transmitting = true;
  } else {
    io.handShake();
    io.setExtension();

    /* 
     * Shake between every transaction to make signify to the computer that we are
     * ready for our next chunk of data
     */
    io.handShake();
    io.setFileChunkSize();

    /* 
    * We keep filling and sending our chunks until we do not have enough bytes to
    * full our entire chunk array, at which point we break the while loop,
    * reset our array, and send the reamining bit of our file
    */
    while (io.getFileChunkSize() == MAX_CHUNK_CHARS) {
      io.setFileChunk();

      #if DEBUG
      io.send(io.getFileChunk());
      #endif

      /* SEND DATA OVER RADIO AND USE INTERUPT TO CONFIRM READY? */

      io.handShake();  // shake between every transaction
      io.setFileChunkSize();
    }
    io.emptyFileChunk();
    io.setFileChunk();

    #if DEBUG
    io.send(io.getFileChunk());
    #endif

    io.softReset();

    /* THIS SHOULD BE DONE WITH NRF STATE IN CLASS */
    transmitting = false;
  }
}


// /*
//  * TODO: ISR for IRQ pin on antenna
//  */
// void ISR_Antenna() {
// }
