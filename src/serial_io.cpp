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

    /* Flush until read FLUSH_CONST FLUSH_COUNT in a row */
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
 * Function handShake() Waits for comuter to say it is ready, then we say we are ready back
 * by sending and receiving CONFIRMATION_CHAR over Serial
 */
void
SerialIO::handShake()
{
  char curr_char;

  /* stay in while loop until computer says it is ready */
  while (true) {
    while (Serial.available()) {
      curr_char = (char) (Serial.read());

      /*
      * If the computer tells us it is ready, we break to respond 
      * that we are now ready to transmit.
      */
      if (curr_char == CONFIRMATION_CHAR) {
        break;
      }
    }

    if (curr_char == CONFIRMATION_CHAR) {
      break;
    }
  }

  /* sleep first so that both computer and Arduino not listening at same time */
  sleep(1); 
  Serial.print(CONFIRMATION_CHAR);
}


/*
 * Function receive() takes care of RX functionality and breaks if we are to switch
 * to TX state.
 */
void
SerialIO::receive()
{
  // TODO: READING TRANSMISSION
}


/*
 * Function transmit reads data sent over serial to configure the desired file
 * extension so we can decode our sent hex file.
 * 
 * Note that the extension must be sent first over transmission, then the data.  
 * This is taken into account in send_hex.py script.
 * 
 */
void 
SerialIO::transmit() 
{
  handShake();
  sendExtension();
  handShake();
  sendFile();
}

/*
 *
 */
void
SerialIO::sendExtension()
{
  char curr_char;

  /* first, collect our file extension */
  while (true) {
    while (Serial.available()) {
      curr_char = (char) (Serial.read());
      if (sent_transmit_bytes < EXTENSION_BYTES) {
        *p_file_extension = curr_char;
        p_file_extension++;
      } 
      
      if (sent_transmit_bytes == EXTENSION_BYTES-1) {
        printConfig(); // for debugging
        break;
      }

      sent_transmit_bytes++;
    }
  }
}

/*
 * 
 */
char *
SerialIO::getExtension()
{
  return file_extension;
}


/*
 * Function sendFile() sends hex from the Arduino to the transmitter in chunks of 16 bytes.
 * Afterwards, we call handshake() so that the computer knows to send the next 16 bytes.  
 * This transmission is done in chunks because we may not be able to fit our entire file 
 * to send in memory at once.
 */
void 
SerialIO::sendFile() 
{
}

/*
 * Function printExtension() Prints the extension of the to-be-transmitted file.
 * More for debugging purposes.
 */
void 
SerialIO::printConfig() 
{
  Serial.print(input_channel);
  Serial.print(CONFIRMATION_CHAR);
  Serial.print(input_address.num);
  Serial.print(CONFIRMATION_CHAR);
  for (auto i {0}; i < EXTENSION_BYTES; ++i) {
    Serial.print(file_extension[i]);
  }
  Serial.print(CONFIRMATION_CHAR);
}
