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

/*
 * user input data pipe address
 */
typedef union {
  uint32_t num;
  uint8_t bytes[4];
} address;

/*
 * States signify whether the board is currently being configured or 
 * has been configures and is now running.
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

  board_state_e getBoardState();
  void readConfig(void);
  void sendFile(void);
  void receive(void);
  void handShake(void);
  void transmit(void);
  void printConfig(void);
  bool isConfigPrinted(void);
  void printExtension(void);

  bool is_config_printed {false};

private:
  board_state_e board_state {CONFIG};
  serial_state_e serial_state {FLUSHING};

  /* Communication configuration variables */
  uint8_t sent_config_bytes {0}; 

  uint8_t input_channel {0};
  address input_address;
  uint8_t * p_input_address {input_address.bytes};

  uint8_t serial_flush_count {0};

  char file_extension[EXTENSION_BYTES];
  char * p_file_extension {file_extension};

  uint8_t sent_transmit_bytes {0};
};

#endif /* _SERIAL_IO_H_ */