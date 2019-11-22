#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include "nRF24L01.h"

#define FLUSH_CONST 9  // byte value we expect  when flushing Serial buffer
#define FLUSH_COUNT 5  // number of sequential FLUSH_CONST needed to switch serial_state_e to READING
#define CHANNEL_BYTES 1  // only 126 possible channels so use 1 byte
#define ADDRESS_BYTES 4  // address width
#define EXTENSION_BYTES 10 // expected bytes in our file extension
#define BAUD_RATE 115200
#define CONFIRMATION_CHAR '\t'  // used to communicate state changes between the Arduino and Computer

/* prototypes */
void ISR_Antenna(void);
void readConfig(void);
void sendFile(void);
void transmit(volatile char);
void printConfig(void);
void printExtension(void);

/*
 * States signify whether the board is currently being configured or 
 * has been configures and is now running.
 */
typedef enum {
  CONFIG,
  READY
} board_state_e;

/*
 * States signify whether the serial is active or not (being flushed)
 */
typedef enum {
  FLUSHING,
  READING
} serial_state_e;

/* We are being configured until we have an address and channel */
volatile board_state_e board_state {CONFIG};
volatile serial_state_e serial_state {FLUSHING};

/* Communication configuration variables */
volatile uint8_t sent_config_bytes {0};  
volatile uint8_t input_channel {0};
volatile address input_address;
volatile uint8_t * p_input_address {input_address.bytes};
volatile bool is_config_printed {false};
volatile uint8_t serial_flush_count {0};
volatile char file_extension[EXTENSION_BYTES];
volatile char * p_file_extension {file_extension};
volatile bool fake_transmit {false};
volatile uint8_t sent_transmit_bytes {0};

void setup() {
  SPI.begin();

  /* Antenna is active low */
  pinMode(A2, PULLUP);
  attachInterrupt(digitalPinToInterrupt(A2), ISR_Antenna, RISING);
  Serial.begin(BAUD_RATE);
}

void loop() {
  switch (board_state) {
  case CONFIG:
    while (Serial.available()) {
      readConfig();
    }
    break;
  
  case READY:
    if (!is_config_printed) {
      printConfig();
      is_config_printed = true;
    }

    while (Serial.available()) {
      volatile char curr_char = (char) (Serial.read());
      if (!fake_transmit) {

        /*
         * If the computer tells us it is ready, we respond to tell 
         * it we are now ready to transmit.
         */
        if (curr_char == CONFIRMATION_CHAR) {
          Serial.print(CONFIRMATION_CHAR); 
          fake_transmit = true;
          sleep(1); // so that both computer and Arduino not listening at same time
          break;
        }
      } else {
        transmit(curr_char);
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


/*
 * Function readConfig reads data sent over serial to configure the desired channel
 * and address settings for transmission.
 * 
 * Note that channel must be sent first over transmission, then address.  This is taken
 * into account in config.py script.
 */
void readConfig() {
  uint8_t curr_byte;
  
  switch (serial_state) {
  case FLUSHING:

    /* Flush until read FLUSH_COUNT FLUSH_CONST in a row */
    while (true) {
      curr_byte = (uint8_t) (Serial.read());
      if (curr_byte == FLUSH_CONST) {
        serial_flush_count++;
      } else {
        serial_flush_count = 0;
      }

      if (serial_flush_count == FLUSH_COUNT) {
        serial_state = READING;
        break;
      }
    }
    break;
  
  case READING:
    curr_byte = (uint8_t) (Serial.read());
    if (sent_config_bytes < CHANNEL_BYTES) {
      input_channel = curr_byte;
    } else {
      *p_input_address = curr_byte;
      p_input_address++;
    } 
    
    if (sent_config_bytes == CHANNEL_BYTES + ADDRESS_BYTES - 1) {
      board_state = READY;
    }
    sent_config_bytes++; 
    break;
  
  default:
    break;
  }   
}


/*
 * Function transmit reads data sent over serial to configure the desired file
 * extension so we can decode our sent hex file.
 * 
 * Note that the extension must be sent first over transmission, then the data.  
 * This is taken into account in send_hex.py script.
 * 
 * We will send hex from the Arduino to the transmitter in chunks of 16 bytes.
 * Afterwards, we will send a confirmation back to the computer so that the computer
 * knows to send the next 16 bytes.  This is done because we may not be able to fit
 * our entire file to send in memory.
 */
void transmit(volatile char curr_char) {
  if (sent_transmit_bytes < EXTENSION_BYTES) {
    *p_file_extension = curr_char;
    p_file_extension++;
  } else {
    sendFile();
  } 
  
  if (sent_transmit_bytes == EXTENSION_BYTES-1) {
    printExtension();
  }

  sent_transmit_bytes++; 
}

void sendFile() {
}
 

 /*
  * Function printConfig() Prints the user input settings once configured. 
  * More for debugging purposes.
  */
void printConfig() {
  Serial.print("Selected channel: ");
  Serial.println(input_channel);
  Serial.print("Selected address: ");
  Serial.println(input_address.num);
}

/*
 * Function printExtension() Prints the extension of the to-be-transmitted file.
 * More for debugging purposes.
 */
void printExtension() {
    Serial.print("File extension: ");
    for (auto i {0}; i < EXTENSION_BYTES; ++i) {
      Serial.print(file_extension[i]);
    }
    Serial.println();
}
