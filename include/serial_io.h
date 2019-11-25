#pragma once

#ifndef _SERIAL_IO_H_
#define _SERIAL_IO_H_

#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>

#define FLUSH_CONST 9  // byte value we expect  when flushing Serial buffer
#define FLUSH_COUNT 5  // number of sequential FLUSH_CONST needed to switch serial_state_e to READING
#define CHANNEL_BYTES 1  // only 126 possible channels so use 1 byte
#define ADDRESS_BYTES 4  // address width

/* 
 * bytes needed to represent all possible mini chunk sizes.
 * Serial buffer is 128 bytes, so can represent #bytes send with a single byte
 */
#define CUNK_SIZE_BYTES 1  
#define MAX_CHUNK_CHARS 255 // given by the size of our Serial buffer
#define EXTENSION_BYTES 10 // expected bytes in our file extension
#define BAUD_RATE 115200
#define CONFIRMATION_CHAR '\t'  // used to communicate state changes between the Arduino and Computer


/*
 * 32 bit value sent over serial
 */
typedef union {
  uint32_t num;
  uint8_t bytes[4];
} uint32_serial_u;


/*
 * States signify whether the board is currently being configured or 
 * has been configured and is now ready to send and receive files.
 */
typedef enum 
{
  CONFIG,
  READY
} board_state_e;


/*
 * States signify whether the serial should be read or not (being flushed)
 */
typedef enum 
{
  FLUSHING,
  READING
} serial_state_e;


class SerialIO
{
public:
  SerialIO();

  /* Getters */
  board_state_e getBoardState();
  char * getExtension(void);
  uint8_t * getAddress(void);
  uint8_t getChannel(void);
  char * getFileChunk(void);
  uint8_t getFileChunkSize(void);

  /* Setters */
  void setFromSerial(char *, uint32_t);
  void setFromSerial(uint8_t *, uint32_t);
  void setConfig(void);
  void setExtension(void);
  void setFileChunkSize(void);
  void setFileChunk(void);
  void emptyFileChunk(void);
  void emptyFileExtension(void);
  void softReset(void);

  /* Auxillary Functions */
  void handShake(void);
  void flushSerial(void);

  /* Arduino -> Computer */
  void send(char *);
  void send(uint8_t);
  void send(uint32_t);


private:

  /* -----communication configuration variables----- */
  board_state_e board_state {CONFIG};
  serial_state_e serial_state {FLUSHING};
  uint8_t input_channel {0};
  uint32_serial_u input_address;


  /* -----file configuration variables----- */
  char file_extension[EXTENSION_BYTES];
  uint8_t next_chunk_size {0};
  char file_chunk[MAX_CHUNK_CHARS];
};

#endif /* _SERIAL_IO_H_ */