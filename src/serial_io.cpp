#include <Arduino.h>
#include <stdint.h>
#include "serial_io.h"

SerialIO::SerialIO() {}


/* -----Getters----- */

/*
 * Getter for the board_state (configuring or not)
 */
board_state_e 
SerialIO::getBoardState() 
{
    return board_state;
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


/* -----Setters----- */

/*
 * Reads from Serial to set a variable of a predetermined size, by setting
 * one byte at a time.  If the value to be set is a file chunk, we can flag
 * with is_file_bytes to indicate that we want to count the total number of bytes
 * sent over to perform checksum functionality on the RX end (future functionality).
 * 
 * Params:
 *  toSet: 
 *    Our char * to set over serial
 *  size:
 *    size in bytes of toSet
 *  is_file_bytes:
 *    boolean flag to indicate if we are setting a file chunk
 */
void
SerialIO::setFromSerial(char * toSet, uint32_t size, bool is_file_bytes) 
{
  char curr_char;
  uint32_t sent_bytes {0};

  while (sent_bytes < size) {
    while (sent_bytes < size && Serial.available()) {
      curr_char = (char) (Serial.read());
      *toSet = curr_char;
      toSet++;
      sent_bytes++;
      
      if (is_file_bytes) {
        file_size++;
      }
    }
  }
}

/*
 * Reads from Serial to set a variable of a predetermined size, by setting
 * one byte at a time.  If the value to be set is a file chunk, we can flag
 * with is_file_bytes to indicate that we want to count the total number of bytes
 * sent over to perform checksum functionality on the RX end (future functionality).
 * 
 * Params:
 *  toSet: 
 *    Our uint8_t * to set over serial
 *  size:
 *    size in bytes of toSet

 */
void
SerialIO::setFromSerial(uint8_t * toSet, uint32_t size) 
{
  uint8_t curr_byte;
  uint32_t sent_bytes {0};

  while (sent_bytes < size) {
    while (sent_bytes < size && Serial.available()) {
      curr_byte = (uint8_t) (Serial.read());
      *toSet = curr_byte;
      toSet++;
      sent_bytes++;
    }
  }
}

/*
 * SetConfig takes the user input channel and address, sent via the config.py script, and stores
 * the variables locally on the Arduino.
 * 
 * First we flush the Serial to prepare for communication, then we set our channel and address
 * over serial, at which point we are ready for communication.
 * 
 * Note that channel must be sent first over transmission, then address.  This is taken
 * into account in config.py script.
 */
void 
SerialIO::setConfig() 
{
  while (board_state == CONFIG) {
    switch (serial_state) {
    case FLUSHING:
      flushSerial();  
      serial_state = READING;
      break;
    
    case READING:
      setFromSerial(&input_channel, (uint32_t) CHANNEL_BYTES);
      setFromSerial(input_address.bytes, (uint32_t) ADDRESS_BYTES);
      board_state = READY;
      break;
    
    default:
      break;
    }
  }   
}

/*
 * Function setExtension() reads data sent over serial to set the desired file
 * extension so we can decode our sent hex file on the RX side.
 */
void
SerialIO::setExtension()
{
  handShake();
  setFromSerial(file_extension, (uint32_t) EXTENSION_BYTES, false); 

  // for debugging
  send(input_channel);
  send(input_address.num);
  send((char *)file_extension);
}


/*
 * Function setFileHexChunk() reads raw hex of our to-be-sent file over serial
 * to set our next file chunk, which will be sent over from our TX board to our 
 * RX board.  Note that our file must be sent in chunks because we are unable to 
 * fit it entirely on the board, so we must determine the size of each chunk via 
 * setFileHexSize(), sent to us over serial via the sed_hex.py script
 */
void 
SerialIO::setFileHexChunk() 
{

  setFileHexSize();
  handShake();
  setFromSerial(file_chunk, next_chunk_size.num, true);

  // for debugging
  send(file_chunk);
}

/*
 * Function setFileHexSize() reads data sent over serial to set the size of our
 * next hex chunk of our to-be-sent file.
 */
void 
SerialIO::setFileHexSize() 
{
  handShake();
  setFromSerial(next_chunk_size.bytes, (uint32_t) CUNK_SIZE_BYTES);
  
  // for debugging
  send(next_chunk_size.num);
}


/* -----Auxillary Functions----- */

/*
 * Function handShake() establishes a line of communication between the computer and Arduino.
 * First, we (from the Arduino) wait for the computer to say it is ready, then we say we are 
 * ready back by sending and receiving CONFIRMATION_CHAR over serial
 */
void
SerialIO::handShake()
{
  char curr_char {'a'}; // arbirary initiallization != CONFIRMATION_CHAR

  /* stay in while loop until computer says it is ready */
  while (curr_char != CONFIRMATION_CHAR) {
    while (curr_char != CONFIRMATION_CHAR && Serial.available()) {
      curr_char = (char) (Serial.read());
    }
  }

  /* sleep first so that both computer and Arduino not listening at same time */
  sleep(1); 

  /* now tell the computer that we are ready too */
  Serial.print(CONFIRMATION_CHAR);
}

/*
 * flushSerial makes sure that any noise sent over serial at startup is disregarded
 * by keeping track of how many consecutive FLUSH_CONST we see.  We have successfully
 * flushed the serial after seeing FLUSH_CONST FLUSH_COUNT in a row.
 */
void
SerialIO::flushSerial() 
{
  uint8_t curr_byte;
  uint8_t serial_flush_count {0};
  
  while (serial_flush_count < FLUSH_COUNT) {
    while (serial_flush_count < FLUSH_COUNT && Serial.available()) {
      curr_byte = (uint8_t) (Serial.read());
      if (curr_byte == FLUSH_CONST) {
        serial_flush_count++;
      } else {
        serial_flush_count = 0;
      }
    }
  }
}


/* ------Arduino -> Computer----- */
/*
 * Function send() Prints over serial our data, then sends a CONFIRMATION_CHAR
 * to indicate that the transmission is over
 * More for debugging purposes.
 */
template <typename T>
void 
SerialIO::send(T data) 
{
  Serial.print(data);
  Serial.print(CONFIRMATION_CHAR);
}
