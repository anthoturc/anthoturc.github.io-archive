#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include <RF24.h>
#include "serial_io.h"

#define CE 26
#define CSN 25
#define DEBUG 0

// /* create an instance of the radio */
RF24 radio(CE, CSN);
SerialIO io;


void setup() {
  SPI.begin();
  Serial.begin(BAUD_RATE);
}


void loop() {
  io.setConfig();
  radio.begin();
  radio.setAddressWidth(ADDRESS_BYTES);
  radio.setChannel(io.getChannel());
  radio.openWritingPipe(io.getAddressBytes());
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();

  io.handshake();
  io.setExtension();

  /* Send extension */
  radio.write(io.getExtension(), FIFO_SIZE_BYTES);
  delay(1000);

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

    // TODO: make this a function
    uint8_t curr_chunk_size = io.getFileChunkSize();
    char * curr_chunk = io.getFileChunk();
    
    int i {0};
    while (i < curr_chunk_size - FIFO_SIZE_BYTES) {
      radio.write(curr_chunk + i, FIFO_SIZE_BYTES);
      delay(1000);
      i += FIFO_SIZE_BYTES;
    }

    io.handshake();  // shake between every transaction
    io.setFileChunkSize();
  }
  io.emptyFileChunk();
  io.setFileChunk();

  uint8_t curr_chunk_size = io.getFileChunkSize();
  char * curr_chunk = io.getFileChunk();
  
  int i {0};
  while (i < curr_chunk_size - FIFO_SIZE_BYTES) {
    radio.write(curr_chunk + i, FIFO_SIZE_BYTES);
    delay(1000);
    i += FIFO_SIZE_BYTES;
  }

  /* Signify that we are done to the other Arduino */
  radio.write(io.END_TX_CHUNK, FIFO_SIZE_BYTES);
  delay(1000);

  io.softReset();
}
