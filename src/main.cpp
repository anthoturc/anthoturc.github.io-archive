#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include "nRF24L01.h"
#include "serial_io.h"

// // #define BAUD_RATE 115200
#define CE 26
#define CSN 25

using namespace nRF24Module;
// /* create an instance of the radio */
nRF24 radio(CE, CSN);
// SerialIO io;
// volatile bool transmitting {false}; // placeholder for nRF states

// /* prototypes */
// // void ISR_Antenna(void);

// // void configRadio();

// void setup() {
//   /* Antenna is active low */
//   pinMode(A2, PULLUP);
//   // attachInterrupt(digitalPinToInterrupt(A2), ISR_Antenna, RISING);
//   Serial.begin(BAUD_RATE);
  
//   io.setConfig();
//   radio.setChannel(io.getChannel());
//   radio.setAddressWidth(ADDRESS_BYTES);
//   radio.setReadingPipeAddr(0, io.getAddress());
//   radio.setWritingAddress(io.getAddress());
//   radio.setDataRate(DATA_RATE_250KBPS);
// }

// void loop() {
//   if (!transmitting) {
//     // nRF receiving
//     radio.setToTransmitter();
//     // if data available:
//     transmitting = true;
//   } else {
//     io.handShake();
//     io.setExtension();
//     io.handShake();
//     io.setFileChunkSize();

//     /* 
//     * We keep filling and sending our chunks until we do not have enough bytes to
//     * full our entire chunk array, at which point we break the while loop,
//     * reset our array, and send the reamining bit of our file
//     */
//     while (io.getFileChunkSize() == MAX_CHUNK_CHARS) {
//       io.setFileChunk();
//       char * data = io.getFileChunk();
//       for (int i = 0; i < MAX_CHUNK_CHARS/32; ++i) {
//         radio.writeSPI((byte *)data, 32);
//         data += 32;        
//       }
//       radio.writeSPI((byte *)data, io.getFileChunkSize()%32);

//       io.setFileChunkSize();
//     }

//     io.emptyFileChunk();
//     io.setFileChunk();
//     io.send(io.getFileChunk());

//     io.softReset();
//     transmitting = false;
//   }
// }


// // /*
// //  * TODO: ISR for IRQ pin on antenna
// //  */
// // void ISR_Antenna() {
// // }

void setup() {
  Serial.begin(9600);

  uint8_t addr[MAX_ADDRESS_WIDTH] = {0, 0, 0, 0, 1};
  radio.setChannel(8);
  radio.setAddressWidth(MAX_ADDRESS_WIDTH);
  radio.setWritingAddress(addr);
  radio.setDataRate(DATA_RATE_250KBPS);
  radio.setToTransmitter();
}

void loop() {
  char buff[12] = {'H','e', 'l', 'l', 'o', 
                  ' ', 
                  'W', 'o', 'r', 'l', 'd', '\0'};
  radio.writeSPI(buff, 12);
  Serial.println("gimme ya butt cheeks");
  delay(2000);
}
