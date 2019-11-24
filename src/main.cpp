#include <Arduino.h>
#include <SoftwareSerial.h>

#include <SPI.h>
#include <stdint.h>
#include "nRF24L01.h"
// #include "serial_io.h"

// // #define BAUD_RATE 115200
#define CE 26
#define CSN 25

// /* create an instance of the radio */
nRF24Module::nRF24 radio(CE, CSN);

void setup() {  
  Serial.begin(9600);
  while (!Serial);

  radio.setChannel(8);
  radio.setToTransmitter();
}

void loop() {
  Serial.println("Getting the channel");
  uint8_t data = radio.getChannel();
  Serial.println(data);
  delay(2000);
}

// SerialIO io;
// volatile bool transmitting {false}; // placeholder for nRF states

// /* prototypes */
// void ISR_Antenna(void);


// void setup() {
//   SPI.begin();

//   /* Antenna is active low */
//   pinMode(A2, PULLUP);
//   attachInterrupt(digitalPinToInterrupt(A2), ISR_Antenna, RISING);
//   Serial.begin(BAUD_RATE);
// }

// void loop() {
//   switch (io.getBoardState()) {
//   case CONFIG:
//     while (Serial.available()) {
//       io.setConfig();
//     }
//     break;
  
//   case READY:

//     if (!transmitting) {
//       io.sendFile();
//       transmitting = true;
//     } else {
//       io.setExtension();
//       io.getExtension();
//       io.getAddress();
//       io.getChannel();
//       transmitting = false;
//     }
//     break;
  
//   /* We have to be in CONFIG or READY */
//   default:
//     break; 
//   } 
// }


// /*
//  * TODO: ISR for IRQ pin on antenna
//  */
// void ISR_Antenna() {
// }
