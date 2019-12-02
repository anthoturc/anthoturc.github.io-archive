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

void foo() {
  // while (true)
  // {
  //   Serial.println("foo");
  // }

  transmitting = true;

  // Serial.print("int status gpio app: ");
  // Serial.println(*reinterpret_cast<uint32_t *>(0x3FF000F8));

  // Serial.print("int val2: ");
  // Serial.println(*reinterpret_cast<uint32_t *>(0x3FF00160));

  // cli();

  

}

void setup() {
  SPI.begin();

  /* Antenna is active low */
  pinMode(A2, PULLUP);
  // attachInterrupt(digitalPinToInterrupt(CE), foo, CHANGE);
  Serial.begin(BAUD_RATE);
  // io.setConfig();

  UART_AT_CMD_CHAR_REG = (UART_AT_CMD_CHAR_REG & ~UART_CHAR_NUM_MASK) | (TX_CHAR_REPS << UART_CHAR_NUM_BIT);
  UART_AT_CMD_CHAR_REG = (UART_AT_CMD_CHAR_REG & ~UART_AT_CMD_CHAR_MASK) | TX_CHAR;
  // UART_INT_ENA_REG |= (1 << UART_AT_CMD_CHAR_DET_INT_ENA_BIT); 

  // *reinterpret_cast<uint32_t *>(0x3FF0018C) = 2; //app
  // *reinterpret_cast<uint32_t *>(0x3FF002A0) = 2; // pro
  // *reinterpret_cast<uint32_t *>(0x3FF1F000) = 0b1111110; // PID enable

  
  // *reinterpret_cast<uint32_t *>(0x3FF1F004) = reinterpret_cast<uint32_t>(&foo); //lv1 addr: 
  // 0x3FF1F008
}


void loop() {

  /* THIS SHOULD BE DONE WITH NRF STATE IN CLASS */
  if (!((UART_INT_RAW_REG & (1<<18))>>18)) {

    Serial.println("bar");

    //   // Serial.println(itt);
    //   Serial.print("PID ena: ");
    //   Serial.println(*reinterpret_cast<uint32_t *>(0x3FF1F000)); 

    //   Serial.print("lv1 addr: ");
    //   Serial.printf("%p", *reinterpret_cast<uint32_t *>(0x3FF1F004)); 
    //   Serial.println();

    //   Serial.printf("%p", reinterpret_cast<uint32_t>(foo));
    // }


    // nRF receiving
    // while available from radio:
      // io.handShake();
      // io.send(transmission);
    
    /* THIS SHOULD BE DONE WITH NRF STATE IN CLASS */
    // transmitting = true;
  } else {
    Serial.println("hit");
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
    UART_INT_CLR_REG |= (1 << 18);
  }
}

// /*
//  * TODO: ISR for IRQ pin on antenna
//  */
// void ISR_Antenna() {
// }
