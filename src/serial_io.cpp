#include "serial_io.h"

SerialIO::SerialIO() {}

/*
 * Getter for the board_state
 */
board_state_e 
SerialIO::getBoardState() 
{
    return board_state;
}

/*
 * Function readConfig reads data sent over serial to configure the desired channel
 * and address settings for transmission.
 * 
 * Note that channel must be sent first over transmission, then address.  This is taken
 * into account in config.py script.
 */
void 
SerialIO::readConfig() 
{
  uint8_t curr_byte;
  
  switch (serial_state) {
  case FLUSHING:

    /* Flush until read FLUSH_CONST FLUSH_COUNT  in a row */
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
void 
SerialIO::transmit(volatile char curr_char) 
{
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

void 
SerialIO::sendFile() 
{

}


 /*
  * Function printConfig() Prints the user input settings once configured. 
  * More for debugging purposes.
  */
void 
SerialIO::printConfig() 
{
  Serial.print("Selected channel: ");
  Serial.println(input_channel);
  Serial.print("Selected address: ");
  Serial.println(input_address.num);
}

/*
 * Function printExtension() Prints the extension of the to-be-transmitted file.
 * More for debugging purposes.
 */
void 
SerialIO::printExtension() 
{
    Serial.print("File extension: ");
    for (auto i {0}; i < EXTENSION_BYTES; ++i) {
      Serial.print(file_extension[i]);
    }
    Serial.println();
}

/*
 * Function sendConfirmation() Acknowledge to the computer that we are ready
 * to transmit data.
 */
void 
SerialIO::sendConfirmation() 
{
    Serial.print(CONFIRMATION_CHAR); 
}
