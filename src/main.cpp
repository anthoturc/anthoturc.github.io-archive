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


void setup() {
  SPI.begin();
  Serial.begin(BAUD_RATE);
  // io.setConfig();

  UART_AT_CMD_CHAR_REG = (UART_AT_CMD_CHAR_REG & ~UART_CHAR_NUM_MASK) | (TX_CHAR_REPS << UART_CHAR_NUM_BIT);
  UART_AT_CMD_CHAR_REG = (UART_AT_CMD_CHAR_REG & ~UART_AT_CMD_CHAR_MASK) | TX_CHAR;
}


void loop() {
  if (!((UART_INT_RAW_REG & (1 << UART_AT_CMD_CHAR_DET_INT_CLR_BIT)) >> UART_AT_CMD_CHAR_DET_INT_CLR_BIT)) {

    // nRF receiving
    // while available from radio:
      // io.handShake();
      // io.send(transmission);
    
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

    UART_INT_CLR_REG |= (1 << UART_AT_CMD_CHAR_DET_INT_CLR_BIT);  // transmitting == false
  }
}
