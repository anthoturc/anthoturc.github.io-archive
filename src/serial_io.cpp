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
 * Function setConfig reads data sent over serial to configure the desired channel
 * and address settings for transmission.
 * 
 * Note that channel must be sent first over transmission, then address.  This is taken
 * into account in config.py script.
 */
void 
SerialIO::setConfig() 
{
  uint8_t curr_byte;
  
  switch (serial_state) {
  case FLUSHING:

    /* Flush until read FLUSH_CONST FLUSH_COUNT in a row */
    while (serial_flush_count != FLUSH_COUNT) {
      curr_byte = (uint8_t) (Serial.read());
      if (curr_byte == FLUSH_CONST) {
        serial_flush_count++;
      } else {
        serial_flush_count = 0;
      }

      if (serial_flush_count == FLUSH_COUNT) {
        serial_state = READING;
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
  char curr_char {'a'}; // arbirary initiallization != CONFIRMATION_CHAR

  /* stay in while loop until computer says it is ready */
  while (curr_char != CONFIRMATION_CHAR) {
    
    /*
     * If the computer tells us it is ready, we break to respond 
     * that we are now ready to transmit.
     */
    while (curr_char != CONFIRMATION_CHAR && Serial.available()) {
      curr_char = (char) (Serial.read());
    }
  }

  /* sleep first so that both computer and Arduino not listening at same time */
  sleep(1); 
  Serial.print(CONFIRMATION_CHAR);
}


/*
 * Function sendFile() sends received file to serial
 */
void
SerialIO::sendFile()
{
  // TODO: everything
}


/*
 * Function setExtension() reads data sent over serial to configure the desired file
 * extension so we can decode our sent hex file.
 */
void
SerialIO::setExtension()
{
  char curr_char;
  handShake();

  /* first, collect our file extension */
  while (n_setter_bytes != EXTENSION_BYTES-1) {
    while ((n_setter_bytes != EXTENSION_BYTES-1) && Serial.available()) {
      curr_char = (char) (Serial.read());
      *p_file_extension = curr_char;
      p_file_extension++;
      
      n_setter_bytes++;
      // for debugging
      if (n_setter_bytes == EXTENSION_BYTES-1) {
          print(input_channel);
          print(input_address.num);
          print((char *)file_extension);
      }

      
    }
  }
}

/*
 * Getter for file extension, set after setTransmission() is run
 */
char *
SerialIO::getExtension()
{
  return file_extension;
}

/*
 * Getter for address, set after setTransmission() is run
 */
uint8_t * 
SerialIO::getAddress(void) 
{
  return input_address.bytes;
}

/*
 * Getter for channel, set after setTransmission() is run
 */
uint8_t 
SerialIO::getChannel(void) 
{
  return input_channel;
}


/*
 */
void 
SerialIO::setFileHexSize() 
{
  char curr_char;
  uint8_t sent_bytes {0};
  
  handShake();

  while (sent_bytes != CUNK_SIZE_BYTES) {
    while ((sent_bytes != CUNK_SIZE_BYTES) && Serial.available()) {
      curr_char = (char) (Serial.read());
      *p_next_chunk_size = curr_char;
      p_next_chunk_size++;
      sent_bytes++;
      if (sent_bytes == CUNK_SIZE_BYTES) {
          print(next_chunk_size.num);
      }

      
    }
  }
}


/*
 * Function setFileHexChunk() gets the raw hex of our file from the computer in chunks of 80k bytes.
 * Afterwards, we call handshake() so that the computer knows to send the next 80k bytes.  
 * This transmission is done in chunks because we may not be able to fit our entire file 
 * to send in memory at once.
 */
void 
SerialIO::setFileHexChunk() 
{
  char curr_char;
  uint8_t sent_bytes {0};
  
  setFileHexSize();
  handShake();
  sent_bytes = 0;
  while (sent_bytes != next_chunk_size.num) {
    while ((sent_bytes != next_chunk_size.num) && Serial.available()) {
      curr_char = (char) (Serial.read());
      *p_file_chunk = curr_char;
      p_file_chunk++;
      sent_bytes++;
      if (sent_bytes == next_chunk_size.num) {
          print(file_chunk);
      }

      
      n_setter_bytes++;
    }
  }
}


/*
 * Function printExtension() Prints the extension of the to-be-transmitted file.
 * More for debugging purposes.
 */
template <typename T>
void 
SerialIO::print(T data) 
{
  Serial.print(data);
  Serial.print(CONFIRMATION_CHAR);
}
