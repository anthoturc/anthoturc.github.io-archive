#pragma once

#ifndef _SERIAL_IO_H_
#define _SERIAL_IO_H_

#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>

#define FLUSH_CONST 9        // byte value we expect  when flushing Serial buffer
#define FLUSH_COUNT 5        // number of sequential FLUSH_CONST needed to switch serial_state_e to READING
#define CHANNEL_BYTES 1      // only 126 possible channels so use 1 byte
#define ADDRESS_BYTES 4      // address width

/* 
 * bytes needed to represent all possible file chunk sizes.
 * Serial buffer is 128 bytes, so can represent #bytes send with a single byte
 */
#define CUNK_SIZE_BYTES 1

#define MAX_CHUNK_CHARS 224   // given by the size of our Serial buffer
#define EXTENSION_BYTES 10    // expected bytes in our file extension
#define BAUD_RATE 115200      // for serial communication
#define FIFO_SIZE_BYTES 32    // size of FIFO in bytes, used to configure buffers for serial data
#define HANDSHAKE_CHAR '\t'   // used to communicate state changes between the Arduino and Computer
#define END_CHAR '}'          // signify the end of transmission
#define TX_CHAR '~'           // at_cmd UART char with matching counterpart in Python receive_hex script 
#define TX_CHAR_REPS 1        // necessary reps of at_cmd char over UART to trigger at_cmd UART interrupt

/* To clarify return values of getExpectedRadioState() */
#define RX_MODE 0
#define TX_MODE 1


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
  /*
   * Getter for the board_state (configuring or not)
   */
  board_state_e getBoardState();

  /*
   * Getter for file_extension, set after setConfig() is run
   */
  char * getExtension(void);

  /*
   * Getter for input_address.bytes, set after setConfig() is run
   */
  uint8_t * getAddressBytes(void);

  /*
   * Getter for input_address.num, set after setConfig() is run
   */
  uint32_t getAddressNum(void);

  /*
   * Getter for input_channel, set after setConfig() is run
   */
  uint8_t getChannel(void);

  /*
   * Getter for file_chunk, set after setFileHexChunk() is run
   */
  char * getFileChunk(void);

  /*
   * Getter for next_chunk_size, set after setFileChunkSize() is run
   */
  uint8_t getFileChunkSize(void);

  /*
   * Gets the expected state of the radio (either tx or rx) as determined by the raw
   * at_cmd interrupt flag
   * 
   * Outputs:
   *  TX_MODE == 1 if Arduino expects the radio to be in rx mode
   *  RX_MODE == 0 if Arduino expects the radio to be in tx mode
   */
  uint8_t getExpectedRadioState(void); 


  /* Setters */

  /*
   * Reads from Serial to set a variable of a predetermined size, by setting
   * one byte at a time.  
   * 
   * Params:
   *  toSet: 
   *    Our char * to set over serial
   *  size:
   *    size in bytes of toSet
   */
  void setFromSerial(char * toSet, uint32_t size);

  /*
   * Reads from Serial to set a variable of a predetermined size, by setting
   * one byte at a time.
   * 
   * Params:
   *  toSet: 
   *    Our uint8_t * to set over serial
   *  size:
   *    size in bytes of toSet
   */
  void setFromSerial(uint8_t * toSet, uint32_t size);

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
  void setConfig(void);

  /*
   * Function setExtension() reads data sent over serial to set the desired file
   * extension so we can decode our sent hex file on the RX side.
   */
  void setExtension(void);

  /*
   * Function setFileChunk() reads raw hex of our to-be-sent file over serial
   * to set our next file chunk, which will be sent over from our TX board to our 
   * RX board.  Note that our file must be sent in chunks because we are unable to 
   * fit it entirely on the board, so we must determine the size of each chunk via 
   * setFileMiniChunkSize(), sent to us over serial via the send_hex.py script
   */
  void setFileChunk(void);

  /*
   * Function setFileChunkSize() reads data sent over serial to set the size of our
   * next hex chunk of our to-be-sent file.
   */
  void setFileChunkSize(void);

  /*
   * Function emptyFileChunk() resets file_chunk array by setting all byte values
   * to 0
   */
  void emptyFileChunk(void);

  /*
   * Function emptyFileChunk() resets file_chunk array by setting all byte values
   * to 0
   */
  void emptyFileExtension(void);

  /*
   * Function softReset() resets io object's file parameters while maintaining its 
   * configuration
   */
  void softReset(void);

  /* Auxillary Functions */

  /*
   * Function handshake() establishes a line of communication between the computer and Arduino.
   * First, we (from the Arduino) wait for the computer to say it is ready, then we say we are 
   * ready back by sending and receiving HANDSHAKE_CHAR over serial
   */
  void handshake(void);

  /*
   * flushSerial makes sure that any noise sent over serial at startup is disregarded
   * by keeping track of how many consecutive FLUSH_CONST we see.  We have successfully
   * flushed the serial after seeing FLUSH_CONST FLUSH_COUNT in a row.
   */
  void flushSerial(void);

  /*
   * Clears the UART interrupt flag of an interrupt specified by bit number of UART_INT_CLR_REG
   * 
   * Params:
   *  bit:
   *    bin of UART_INT_CLR_REG to clear
   */
  void clearInterruptUART(uint8_t bit);

  /*
   * Configures the at_cmd interrupt, which, on a high level, is flagged when a predefined character is
   * sent over UART a predefined number of times with sufficient space in between each each transmission.
   * 
   * Params:
   *  c: 
   *    our at_cmd character
   *  reps:
   *    number of reps needed to determine if we should flag the interrupt
   */
  void configAtCmdCharInterrupt(char c, uint8_t reps);


  /* Arduino -> Computer */

  /*
   * Function send() Prints over serial our data, then sends a HANDSHAKE_CHAR
   * to indicate that the transmission is over
   */
  void send(char * data);
  void send(char data);
  void send(uint8_t data);
  void send(uint32_t data);

  char END_TX_CHUNK[32] {'}'};

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