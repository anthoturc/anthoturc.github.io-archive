#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include "nRF24L01.h"

#define FLUSH_CONST 9  // byte value we expect  when flushing Serial buffer
#define FLUSH_COUNT 5  // number of sequential FLUSH_CONST needed to switch serial_state_e to ACTIVE
#define CHANNEL_BYTES 1  // only 126 possible channels so use 1 byte
#define ADDRESS_BYTES 4  // address width

/* prototypes */
void ISR_Antenna(void);
void readConfig(void);
void printConfig(void);

/*
 * States signify whether the board is currently being configured or 
 * has been configures and is now running.
 */
typedef enum {
  CONFIG,
  RUNNING
} board_state_e;

/*
 * States signify whether the serial is active or not (being flushed)
 */
typedef enum {
  FLUSHING,
  ACTIVE
} serial_state_e;

/* We are being configured until we have an address and channel */
volatile board_state_e board_state = CONFIG;
volatile serial_state_e serial_state = FLUSHING;

/* Communication configuration variables */
volatile uint8_t sent_config_bytes = 0;  
volatile uint8_t input_channel = 0;
volatile address input_address;
volatile uint8_t * p_input_address;
volatile bool is_config_printed = false;
volatile uint8_t serial_flush_count = 0;

void setup() {
  SPI.begin();
  p_input_address = input_address.bytes;

  /* Antenna is active low */
  pinMode(A2, PULLUP);
  attachInterrupt(digitalPinToInterrupt(A2), ISR_Antenna, RISING);
  Serial.begin(115200);
}

void loop() {
  switch (board_state) {
  case CONFIG:
    while (Serial.available()) {
      readConfig();
    }
    break;
  
  case RUNNING:
    if (!is_config_printed) {
      printConfig();
      is_config_printed = true;
    }

    // also our nRF needs to be receiving
    while (Serial.available()) {
      volatile char stored = (char) (Serial.read());
      if (stored == '\t') {
        Serial.print("ready"); // now transmit
        break;
      }
    }
    break;
  
  default:
    break; // have to be in CONFIG or RUNNING
  } 
}


/*
 * TODO: ISR for IRQ pin on antenna
 */
void ISR_Antenna() {
}


/*
 * Function readInput reads data sent over serial to configure the desired channel
 * and address settings for transmission.
 * 
 * Note that channel must be sent first over transmission, then address.  This is taken
 * into account in interface.sh script.
 * 
 * Also note that each transmission must end in a new line character.  This is how we
 * can differrentiate between the channel and address transmissions.
 */
void readConfig() {
  uint8_t currByte;
  
  switch (serial_state) {
  case FLUSHING:

    /* Flush until read FLUSH_COUNT FLUSH_CONST in a row */
    while (true) {
      currByte = (uint8_t) (Serial.read());
      if (currByte == FLUSH_CONST) {
        serial_flush_count++;
      } else {
        serial_flush_count = 0;
      }

      if (serial_flush_count == FLUSH_COUNT) {
        serial_state = ACTIVE;
        break;
      }
    }
    break;
  
  case ACTIVE:
    currByte = (uint8_t) (Serial.read());
    if (sent_config_bytes < CHANNEL_BYTES) {
      input_channel = currByte;
    } else {
      *p_input_address = currByte;
      p_input_address++;
    } 
    
    if (sent_config_bytes == CHANNEL_BYTES + ADDRESS_BYTES - 1) {
      board_state = RUNNING;
    }
    sent_config_bytes++; 
    break;
  
  default:
    break;
  }   
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
