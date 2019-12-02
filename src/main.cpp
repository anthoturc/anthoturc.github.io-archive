#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include "nRF24L01.h"
#include "serial_io.h"
#include "esp32AtCmdUART.h"

#define CE 26
#define CSN 25
#define DEBUG 1

// /* create an instance of the radio */
nRF24Module::nRF24 radio(CE, CSN);
SerialIO io;


void setup() {
  SPI.begin();
  Serial.begin(BAUD_RATE);
  io.setConfig(TX_CHAR_REPS, TX_CHAR);

  // UART_AT_CMD_CHAR_REG = (UART_AT_CMD_CHAR_REG & ~UART_CHAR_NUM_MASK) | (TX_CHAR_REPS << UART_CHAR_NUM_BIT);
  // UART_AT_CMD_CHAR_REG = (UART_AT_CMD_CHAR_REG & ~UART_AT_CMD_CHAR_MASK) | TX_CHAR;
  UART_AT_CMD_PRECNT_REG = (UART_PRE_IDLE_NUM_MASK & ~UART_AT_CMD_CHAR_MASK) | 0x0;
  UART_AT_CMD_POSTCNT_REG = (UART_POST_IDLE_NUM_MASK & ~UART_AT_CMD_CHAR_MASK) | 0x0;
}


void loop() {
  if (io.getExpectedRadioState() == RX_MODE) {

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

    /* change getExpectedRadioState to RX_MODE */
    io.clearInterruptUART(UART_AT_CMD_CHAR_DET_INT_CLR_BIT);
  }
}
