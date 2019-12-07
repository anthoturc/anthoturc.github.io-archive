#include <Arduino.h>
#include <stdint.h>
#include "serial_io.h"
#include "esp32AtCmdUART.h"

SerialIO::SerialIO() {}

/* -----Getters----- */

board_state_e 
SerialIO::getBoardState() 
{
    return board_state;
}


char *
SerialIO::getExtension()
{
  return file_extension;
}


uint8_t * 
SerialIO::getAddressBytes(void) 
{
  return input_address.bytes;
}

uint32_t
SerialIO::getAddressNum(void) 
{
  return input_address.num;
}


uint8_t 
SerialIO::getChannel(void) 
{
  return input_channel;
}


char *
SerialIO::getFileChunk(void)
{
  return file_chunk;
}


uint8_t
SerialIO::getFileChunkSize(void)
{
  return next_chunk_size;
}


uint8_t
SerialIO::getExpectedRadioState() 
{
  return (UART_INT_RAW_REG & (1 << UART_AT_CMD_CHAR_DET_INT_CLR_BIT)) >> UART_AT_CMD_CHAR_DET_INT_CLR_BIT;
}


/* -----Setters----- */

void
SerialIO::setFromSerial(char * toSet, uint32_t size) 
{
  char curr_char;
  uint32_t sent_bytes {0};

  /*
   * We need two while loops because we need to remain in our
   * setting state until the computer sends over data, but that
   * may not happen instantly.  We also want to break out of
   * the innermost loop the second that we have all the data 
   * we need.
   */
  while (sent_bytes < size) {
    while (sent_bytes < size && Serial.available()) {
      curr_char = (char) (Serial.read());
      *toSet = curr_char;
      toSet++;
      sent_bytes++;
    }
  }
}


void
SerialIO::setFromSerial(uint8_t * toSet, uint32_t size) 
{
  uint8_t curr_byte;
  uint32_t sent_bytes {0};

  /* For 2 loop reasoning, see setFromSerial(char *, uint32_t)  */
  while (sent_bytes < size) {
    while (sent_bytes < size && Serial.available()) {
      curr_byte = (uint8_t) (Serial.read());
      *toSet = curr_byte;
      toSet++;
      sent_bytes++;
    }
  }
}


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


void
SerialIO::setExtension()
{
  setFromSerial(file_extension, (uint32_t) EXTENSION_BYTES); 
}


void 
SerialIO::setFileChunk() 
{
  setFromSerial(file_chunk, next_chunk_size);
}


void 
SerialIO::setFileChunkSize() 
{
  setFromSerial(&next_chunk_size, (uint32_t) CUNK_SIZE_BYTES);
}


void 
SerialIO::emptyFileChunk() 
{
  /* Reset all values of our file chunk */
  for (uint8_t i = 0; i < MAX_CHUNK_CHARS; ++i) {
    file_chunk[i] = 0;
  }
}


void 
SerialIO::emptyFileExtension() 
{
  /* Reset all values of our file chunk */
  for (uint8_t i = 0; i < EXTENSION_BYTES; ++i) {
    file_extension[i] = 0;
  }
}


void 
SerialIO::softReset() 
{
  emptyFileChunk();
  emptyFileExtension();
  next_chunk_size = 0;
}


/* -----Auxillary Functions----- */

void
SerialIO::handshake()
{
  char curr_char {'a'}; // arbirary initiallization != HANDSHAKE_CHAR

  /* For 2 loop reasoning, see setFromSerial(char *, uint32_t)  */
  while (curr_char != HANDSHAKE_CHAR) {
    while (curr_char != HANDSHAKE_CHAR && Serial.available()) {
      curr_char = (char) (Serial.read());
    }
  }

  /* now tell the computer that we are ready too */
  Serial.print(HANDSHAKE_CHAR);
}


void
SerialIO::flushSerial() 
{
  uint8_t curr_byte;
  uint8_t serial_flush_count {0};
  
  /* For 2 loop reasoning, see setFromSerial(char *, uint32_t)  */
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


void 
SerialIO::clearInterruptUART(uint8_t bit) {
  UART_INT_CLR_REG |= (1 << bit);
}


void
SerialIO::configAtCmdCharInterrupt(char c, uint8_t reps) 
{
  UART_AT_CMD_CHAR_REG = (UART_AT_CMD_CHAR_REG & ~UART_CHAR_NUM_MASK) | (reps << UART_CHAR_NUM_BIT);
  UART_AT_CMD_CHAR_REG = (UART_AT_CMD_CHAR_REG & ~UART_AT_CMD_CHAR_MASK) | c;
}


/* ------Arduino -> Computer----- */

void
SerialIO::send(uint8_t data) 
{
  Serial.print(data);
  Serial.print(HANDSHAKE_CHAR);
}

void
SerialIO::send(uint32_t data) 
{
  Serial.print(data);
  Serial.print(HANDSHAKE_CHAR);
}

void
SerialIO::send(char * data) 
{
  Serial.print(data);
  Serial.print(HANDSHAKE_CHAR);
}

void
SerialIO::send(char data) 
{
  Serial.print(data);
  Serial.print(HANDSHAKE_CHAR);
}
