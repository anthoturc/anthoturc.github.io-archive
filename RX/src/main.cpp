#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include <RF24.h>
#include "serial_io.h"

#define CE 26
#define CSN 25

char FIFO_BUFFER[32] {"g"};  // arbitrary non-hex char

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
  radio.openReadingPipe(0, io.getAddressBytes());
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();

  /* Send file and extension in chunks until told to stop */
  while (FIFO_BUFFER[0] != END_CHAR) {
    if (radio.available(0)) {
      radio.read(FIFO_BUFFER, FIFO_SIZE_BYTES);

      if (FIFO_BUFFER[0] != END_CHAR) {
        io.handshake();
        io.send(FIFO_BUFFER);
      }
    }
  }

  io.handshake();
  io.send(END_CHAR); // transmission over
}
