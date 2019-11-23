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
#define EXTENSION_BYTES 10 // expected bytes in our file extension
#define BAUD_RATE 115200
#define CONFIRMATION_CHAR '\t'  // used to communicate state changes between the Arduino and Computer
#define MAX_FILE_CHUNK_BYTES 80000 // max amount of bytes to be sent by the computer at once


/*
 * user input data pipe address
 */
typedef union {
  uint32_t num;
  uint8_t bytes[4];
} address;


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
 * States signify whether the serial is active or not (being flushed)
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

  void handShake(void);
  void sendFile(void);

  /* Setters */
  void setConfig(void);
  void setExtension(void);
  void setFileHexChunk(void);

  /* Getters */
  board_state_e getBoardState();
  char * getExtension(void);
  uint8_t * getAddress(void);
  uint8_t getChannel(void);

private:
  void printConfig(void);  // for debugging

  board_state_e board_state {CONFIG};
  serial_state_e serial_state {FLUSHING};


  /* -----communication configuration variables----- */
  /* 
   * keeps track of how many config bytes sent so we know
   * if the curr byte should be included in the address var
   * or the channel var
   */
  uint8_t sent_config_bytes {0};
  uint8_t input_channel {0};
  address input_address;
  uint8_t * p_input_address {input_address.bytes};

  /* 
   * keeps track of how many consecutive FLUSH_CONST we
   * receive in a row, so we know when to start reading from
   * the serial.
   */
  uint8_t serial_flush_count {0};


  /* -----file configuration variables----- */
  char file_extension[EXTENSION_BYTES];
  char * p_file_extension {file_extension};
  byte file_chunk[MAX_FILE_CHUNK_BYTES];
  
  /*
   * Keep track of the file size, including extension
   * so that we can perform a checksum after comunication
   * (feature not implemented yet)
   */
  uint8_t n_setter_bytes {0};
};

#endif /* _SERIAL_IO_H_ */