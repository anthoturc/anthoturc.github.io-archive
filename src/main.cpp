#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include <nRF24L01.h>

/* prototypes */
void ISR_Antenna(void);
void readConfig(void);
void readInput(void);
void printConfig(void);

/* Communication configuration variables */
volatile uint8_t sent_config_bytes = 0;  
volatile uint8_t input_channel = 0;
volatile address input_address;
volatile uint8_t * p_input_address;
volatile bool is_config_printed = false;
volatile uint8_t serial_flush_count = 0;
volatile bool configured = false;

volatile char stored;


void setup() {
  SPI.begin();
  p_input_address = input_address.bytes;

  /* Antenna is active low */
  pinMode(A2, PULLUP);
  attachInterrupt(digitalPinToInterrupt(A2), ISR_Antenna, RISING);
  Serial.begin(115200);
}

int curr;
void loop() {
  while ((sent_config_bytes != 5) && Serial.available()) {
    readConfig();
  }

  if ((sent_config_bytes == 5) && (!is_config_printed)) {
    printConfig();
    is_config_printed = true;
  }

  while (configured && Serial.available()) {
    stored = (char) (Serial.read());
    if (stored == '\t') {
      Serial.print("ready");
    }
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
  
  if (serial_flush_count == 5) {
    Serial.println("here");
    currByte = (uint8_t) (Serial.read());
    if (sent_config_bytes == 0) {
      input_channel = currByte;
    } else { // (sent_config_bytes < 5)
      *p_input_address = currByte;
      p_input_address++;
    }
    sent_config_bytes++;   
  } else {
    while (true) {
      currByte = (uint8_t) (Serial.read());
      if (currByte == 9) {
        serial_flush_count++;
      } else {
        serial_flush_count = 0;
        break;
      }

      if (serial_flush_count == 5) {
        configured = true;
        break;
      }
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
  Serial.println(input_address.num);
}