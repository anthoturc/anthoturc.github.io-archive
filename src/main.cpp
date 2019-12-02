#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include "nRF24L01.h"
#include "serial_io.h"
#include "esp32AtCmdUART.h"

// // #define BAUD_RATE 115200
#define CE 26
#define CSN 25
#define DEBUG 1

// /* create an instance of the radio */
nRF24Module::nRF24 radio(CE, CSN);
SerialIO io;
volatile bool transmitting {false}; // placeholder for nRF states

int itt {0};

/* prototypes */
// void ISR_Antenna(void);

void IRAM_ATTR poop() {
  Serial.println("poop");
  // UART_INT_CLR_REG &= (0 << UART_INT_CLR_REG);

}

void setup() {
  SPI.begin();

  /* Antenna is active low */
  pinMode(A2, PULLUP);
  // attachInterrupt(digitalPinToInterrupt(A2), ISR_Antenna, RISING);
  Serial.begin(BAUD_RATE);
  // io.setConfig();

  UART_AT_CMD_CHAR_REG = (UART_AT_CMD_CHAR_REG & ~UART_CHAR_NUM_MASK) | (TX_CHAR_REPS << UART_CHAR_NUM_BIT);
  UART_AT_CMD_CHAR_REG = (UART_AT_CMD_CHAR_REG & ~UART_AT_CMD_CHAR_MASK) | TX_CHAR;
  UART_INT_ENA_REG |= (1 << UART_AT_CMD_CHAR_DET_INT_ENA_BIT);
  attachInterrupt(16, poop, CHANGE);
  
}


void loop() {

  /* THIS SHOULD BE DONE WITH NRF STATE IN CLASS */
  if (!transmitting) {
    Serial.println(itt++);

    // nRF receiving
    // while available from radio:
      // io.handShake();
      // io.send(transmission);
    
    /* THIS SHOULD BE DONE WITH NRF STATE IN CLASS */
    // transmitting = true;
  } else {
    io.handshake();
    io.setExtension();

    /* 
     * Shake between every transaction to make signify to the computer that we are
     * ready for our next chunk of data
     */
    io.handshake();
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

      io.handshake();  // shake between every transaction
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
