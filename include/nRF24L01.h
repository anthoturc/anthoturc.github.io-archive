/*
 *  This header file is based on the specificaitons made in section
 *  8 of the nRF24L01+ data sheet.
 *  
 *  SPI has a max data rate of 10Mbps (cannot be configured by program)
 *  
 *  Each new command must start with a high to low transition on CSN (i.e. CSN is active low).
 *  
 *  This library is meant to be run on the Adafruit ESP 32 Feather.
 */

#pragma once

#ifndef _NRF_24_
#define _NRF_24_

#include <stdint.h>

/*
 *  These macros correspond to the min and max frequency that our 
 *  module is able to transmit/receive at.
 */
#define MIN_FREQ 2400e6ul
#define MAX_FREQ 2525e6ul

/*
 *  A channel is a specific frequency at which the nRF24L01
 *  can receive and transmit data. This module can
 *  do both between 2,400 - 2,525 MHz. Each channel can 
 *  occupy bandwidth of less than 1 MHz. So the number of channels
 *  for this module is ~ 125
 * 
 *  Note that each channel will occupy bandwidth based on the air data
 *  rate.
 */
#define NUM_CHANNELS 125

/*
 *  This macro will determine the frequency at a specified channel.
 *  Since there are 125 channels, we can add the channel to the min
 *  frequency to get the frequency of the channel.
 */
#define FREQ(channel) (MIN_FREQ + (channel))

/*
 *  Macros for the pins that are MCU will
 *  have access to. Not all of them will be used.
 *  They are mainly here to document the purpose 
 *  of each pin on the nRF24L01
 */
#define GND_PIN     1
#define VCC         2 

/* 
 *  The CE (Chip Enable) pin is an active-High pin. 
 *  When selected the nRF24L01 will either transmit 
 *  or receive, depending on the mode it is in.
 */        
#define CE_PIN      3

/*
 *  The CSN (Chip Select Not) pin is an active-Low pin and will
 *  usually be HIGH. When the pin goes low, the nRF24L01 begins 
 *  listening on its SPI port for data and processes it.
 */
#define CSN_PIN     4

/*
 *  The SCK (Serial Clock) pin will take the clock from the 
 *  SPI bus master.
 */
#define SCK_PIN     5

/*
 *  The MOSI (Master Out Slave In) is SPI input to the nRF24L01.
 */ 
#define MOSI_PIN    6

/*
 *  The MISO (Master In Slave Out) is the SPI output from the nRF24L01.
 */
#define MISO_PIN    7

/*
 *  The IRQ pin is an interrupt pin that will alert the master when new
 *  data is available. 
 */
#define IRQ_PIN     8

/*
 *  The module uses a packet structure called Enhanced ShockBurst.
 *  This structure is broken down into 5 fields. 
 * 
 *  These fields can be modeled using bit fields.
 * 
 *  TODO: Check the endianess of the Feather to make sure
 *  the order of the struct fields are correct
 */

#define MISO_PIN    12
#define MOSI_PIN    13
#define SCK_PIN     14


typedef union
{
    /* This is the frame that will be send over SPI */
    uint64_t data_frame;
    typedef struct
    {
        /* preable is 1 byte */
        uint8_t preamble;
        /* addresses can be 3-5 bytes wide, we can use 3 bytes */
        uint32_t addr : 24;
        /* payload length is 6 bits, packet id is 2 bits, no ack is 1 bit */
        uint16_t packet_ctrl : 9;
        /* we will use 2 bytes */
        uint8_t byte1;
        uint8_t byte2;
        /* this byte is just padding */
        uint8_t byte3 : 7;
    } bits_t;
} packet_u;

/*
 *  The following contains the registers of 
 *  the devices. This includes the register names
 *  and their addresses.
 */
enum
{
    CONFIG          = 0x00,
    EN_AA           = 0x01,
    EN_RXADDR       = 0x02,
    SETUP_AW        = 0x03,
    SETUP_RETR      = 0x04,
    RF_CH           = 0x05,
    RF_SETUP        = 0x06,
    STATUS          = 0x07,
    OBSERVE_TX      = 0x08,
    CD              = 0x09,
    RX_ADDR_P0      = 0x0A,
    RX_ADDR_P1      = 0x0B,
    RX_ADDR_P2      = 0x0C,
    RX_ADDR_P3      = 0x0D,
    RX_ADDR_P4      = 0x0E,
    RX_ADDR_P5      = 0x0F,
    TX_ADDR         = 0x10,
    RX_PW_P0        = 0x11,
    RX_PW_P1        = 0x12,
    RX_PW_P2        = 0x13,
    RX_PW_P3        = 0x14,
    RX_PW_P4        = 0x15,
    RX_PW_P5        = 0x16,
    FIFO_STATUS     = 0x17,
    /* Registers in this gap are used for testing */
    DYNPD           = 0x1C,
    FEATURE         = 0x1D,
};

#endif /* _NRF_24_ */