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
#define CUNK_SIZE_BYTES 4  // address width
#define EXTENSION_BYTES 10 // expected bytes in our file extension
#define BAUD_RATE 115200
#define CONFIRMATION_CHAR '\t'  // used to communicate state changes between the Arduino and Computer
#define MAX_FILE_CHUNK_BYTES 80000 // max amount of bytes to be sent by the computer at once


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

  /* Setters */
  void setFromSerial(char *, uint32_t, bool);
  void setFromSerial(uint8_t *, uint32_t);
  void setConfig(void);
  void setExtension(void);
  void setFileHexSize(void);
  void setFileHexChunk(void);

  /* Auxillary Functions */
  void handShake(void);
  void flushSerial(void);

  /* Arduino -> Computer */
  template <typename T>
  void send(T);


private:

  /* -----communication configuration variables----- */
  board_state_e board_state {CONFIG};
  serial_state_e serial_state {FLUSHING};
  uint8_t input_channel {0};
  uint32_serial_u input_address;


  /* -----file configuration variables----- */
  char file_extension[EXTENSION_BYTES];
  uint32_serial_u next_chunk_size;
  char file_chunk[MAX_FILE_CHUNK_BYTES];
  
  /*
   * Keep track of the file size so that we can perform a checksum
   * after comunication (feature not implemented yet)
   */
  uint32_t file_size {0};
};

#endif /* _SERIAL_IO_H_ */