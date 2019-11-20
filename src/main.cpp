#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include "nRF24L01.h"

#define CE 26
#define CSN 25

/* create an instance of the radio */
nRF24Module::nRF24 radio(CE, CSN);

/* prototypes */
void ISR_Antenna(void);
void readInput(void);
void printConfig(void);

/* Communication configuration variables */
volatile uint8_t sent_config_bytes = 0;  
volatile uint8_t input_channel = 0;
volatile address input_address;
volatile uint8_t * p_input_address;
volatile bool is_config_printed = false;


void setup() {
  Serial.begin(115200);
  SPI.begin();
  p_input_address = input_address.bytes;

  /* Antenna is active low */
  pinMode(A2, PULLUP);
  attachInterrupt(digitalPinToInterrupt(A2), ISR_Antenna, RISING);
}

void loop() {
  while ((sent_config_bytes != 5) && Serial.available()) {
    readInput();
  }
  if ((sent_config_bytes == 5) && (!is_config_printed)) {
    printConfig();
    is_config_printed = true;
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
void readInput() {
  uint8_t currByte = (uint8_t) (Serial.read());
  
  if (sent_config_bytes == 0) {
    input_channel = currByte;
  } else { // (sent_config_bytes < 5)
    *p_input_address = currByte;
    p_input_address++;
  }
  sent_config_bytes++;   
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