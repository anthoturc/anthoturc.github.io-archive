#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include <nRF24L01.h>

/* prototypes */
void ISR_Antenna(void);
void readInput(void);
void printConfig(void);

/* Communication configuration variables */
volatile bool is_configured = false;  // turns true only after both channel_sent & address_sent
volatile bool channel_sent = false;
volatile bool address_sent = false;
volatile uint8_t input_channel = 0;
volatile uint32_t input_address = 0;
volatile uint32_t place_val = 1;

bool is_config_printed = false;


void setup() {
  Serial.begin(115200);
  SPI.begin();

  /* Antenna is active low */
  pinMode(A2, PULLUP);
  attachInterrupt(digitalPinToInterrupt(A2), ISR_Antenna, RISING);
}

void loop() {
  while ((!is_configured) && Serial.available()) {
    readInput();
  }
  if (is_configured && (!is_config_printed)) {
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
 * into account in interfase.sh script.
 * 
 * Also note that each transmission must end in a new line character.  This is how we
 * can differrentiate between the channel and address transmissions.
 * 
 * Finally, note that the address and channel must be sent in reverse numerical order 
 * so we can properly handle the place vales of each of our digits.
 * 
 */
void readInput() {
  char currChar = (char) (Serial.read());
    
  /* signify the end of a transmission with \n */
  if (currChar == '\n') {
    if (!channel_sent) {
      channel_sent = true;
      place_val = 1;
    } else if (!address_sent) {
      address_sent = true;
      is_configured = true;
      place_val = 1;
    }
  } else {
    /* 
    * channel is input in reverse character order because
    * we know how to increment the number with each digit
    * when starting at the one's digit, but from the left
    * we are completely uncsure of the place value.  This
    * is taken care of in the bash script.
    */
    if (!channel_sent) {

      /* converting ASCII character to correct integer */
      input_channel += (place_val * (uint8_t) (currChar - '0'));
      place_val *= 10;  // increment the place value with each digit
    } else if (!address_sent) {

      /* converting ASCII character to correct integer */
      input_address += (place_val * (uint32_t) (currChar - '0'));
      place_val *= 10;  // increment the place value with each digit
    }
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
  Serial.println(input_address);
}
