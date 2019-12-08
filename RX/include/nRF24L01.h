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
 * This is the maxiumum speed that we can communicate with the 
 * chip, per the data sheet
 */
#define SPI_FRQ 10e6 

/*
 *  These macros correspond to the min and max frequency that our 
 *  module is able to transmit/receive at.
 */
#define MIN_FREQ 2400e6ul
#define MAX_FREQ 2525e6ul
#define MHz 1e6ul

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
 *  A pipe is a logical channel in the physical RF channel. Each one has
 *  its own unique address. There are 6 piptes total, numbered 0-5.
 */
#define N_PIPES 5

/*
 *  This macro will determine the frequency at a specified channel.
 *  Since there are 125 channels, we can add the channel to the min
 *  frequency to get the frequency of the channel.
 */
#define FREQ(channel) (MIN_FREQ + (channel * MHz))

/*
 * Max SCK_FREQ for SPI communication.  Depends on board config
 * so this number may be actually as high as 10MHz.
 */
#define SCK_FREQ 4e6ul

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
 * PIN definitions of CONFIG (0x00) register
 */

/*
 * RX/TX control
 * 
 * 1: PRX
 * 0: PTX
 */
#define PRIM_RX 0

/*
 * Enable or disable power up mode.  In power up mode, chip is disabled
 * but register values are maintained and accessable via SPI
 * 
 * 1: POWER UP
 * 0: POWER DOWN
 */
#define PWR_UP 1

/*
 * CRC encoding scheme.  Used by Enhanced ShockBurst for error
 * detection.  CRC bytes are the last 1-2 bytes of the Enhanced
 * ShockBurst packet.
 * 
 * 0: 1-byte
 * 1: 2-byte
 */
#define CRCO 2

/*
 * Enable CRC
 * 
 * 0: Disable
 * 1: Enable
 */
#define EN_CRC 3

/*
 * Mask interrupt caused by MAX_RT (max amount of repeated transmits)
 * 
 * 0: Reflect MAX_RT as active low interrupt on IRQ pin
 * 1: Interrupt not reflected on IRQ pin
 */
#define MASK_MAX_RT 4

/*
 * Mask interrupt caused by TX_DS (data sent)
 * 
 * 0: Reflect TX_DS as active low interrupt on IRQ pin
 * 1: Interrupt not reflected on IRQ pin
 */
#define MASK_TX_DS 5

/*
 * Mask interrupt caused by RX_DR (data recieved)
 * 
 * 0: Reflect RX_DR as active low interrupt on IRQ pin
 * 1: Interrupt not reflected on IRQ pin
 */
#define MASK_RX_DR 6

/*
 * The size of each FIFO (for TX and RX) in bytes
 */
#define FIFO_SZ 32

/*
 * Macro for no bytes to be sent. This serves as "padding"
 * for making the data frame
 */
#define NO_DATA 0x00

/*
 * PIN definitions of SETUP_AW (0x03) register 
 * (setup address widths for data pipes)
 */

/*
 * RX/TX Address field width
 * 
 * AW_1=0 and AW_0=0: Illegal
 * AW_1=0 and AW_0=1: 3 bytes
 * AW_1=1 and AW_0=0: 4 bytes
 * AW_1=1 and AW_0=1: 5 bytes
 */
#define AW_0 0
#define AW_1 1


/*
 * PIN definitions of SETUP_RETR (0x04) register 
 * (setup of automatic retransmission)
 */

/*
 * Auto Retransmission delay: every additional bit of
 * ARD, given by ARD_3:0 corresponds to an additional
 * 250 microseconds with '0000' = wait 250 microseconds
 */
#define ARD_0 4
#define ARD_1 5
#define ARD_2 6
#define ARD_3 7

/*
 * Auto Retransmission count: every additional bit of
 * ARC, given by ARC_3:0 corresponds to an additional
 * retransmission with 
 * 
 * '0000' = retransmit disabled
 * '0001' = up to 1 retransmit
 * '1111' = up to 15 retransmit
 */
#define ARC_0 0
#define ARC_1 1
#define ARC_2 2
#define ARC_3 3


/*
 * PIN definitions of RF_CH (0x05) register 
 * (setup RF channel i.e. operating frequency)
 */

/*
 * RF_CH_6:0 represents the channel for our communication 
 * 
 * Frequency is given by 2.4GHz + (channel * MHz)
 * Max channel = 125 = '1111101'
 */
#define RF_CH_0 0
#define RF_CH_1 1
#define RF_CH_2 2
#define RF_CH_3 3
#define RF_CH_4 4
#define RF_CH_5 5
#define RF_CH_6 6


/*
 * PIN definitions of RF_SETUP (0x06) register 
 * (RF setup register)
 */

/*
 * RF_DR_1:0 sets the air data rate
 * 
 * '00' = 1Mbps
 * '01' = 2Mbps
 * '10' = 250kbps
 * '11' = Illegal
 */
#define RF_DR_0 3
#define RF_DR_1 5

/*
 * RF_PWR_1:0 sets TX RF output power
 * 
 * '00' = -18dBM
 * '01' = -12dBM
 * '10' = -6dBM
 * '11' =  0dBM
 */
#define RF_PWR_0 1
#define RF_PWR_1 2

#define SPI_SETTINGS SPISettings(SPI_FRQ, MSBFIRST, SPI_MODE0)


/*
 *  These macros correspond to the minimum and 
 *  maximum number of bytes that are allowed to 
 *   be taken up by the address.
 */
#define MIN_ADDRESS_WIDTH 3
#define MAX_ADDRESS_WIDTH 5

/*
 * When writing to a register we take the lower 5 bits
 */
#define REGISTER_MASK 0x1F

/*
 *  The module uses a packet structure called Enhanced ShockBurst.
 *  This structure is broken down into 5 fields. 
 * 
 *  These fields can be modeled using bit fields.
 * 
 *  The endianess of the feather and the module are the same!
 */
namespace nRF24Module {
    typedef struct
    {
        /* data to be sent */
        uint8_t data;
        /* preable is 1 byte */
        uint8_t preamble;
    } bits_t;


    typedef union
    {
        /* This is the frame that will be send over SPI */
        uint16_t data_frame;
        /* This is the frame that will be send over SPI */
        bits_t atomic_frame;    
    } data_frame_u;

    /*
    *  The following contains the registers of 
    *  the devices. This includes the register names
    *  and their addresses.
    */

    enum registers
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

    enum commands
    {
        R_RX_PAYLOAD            = 0b01100001,
        W_TX_PAYLOAD            = 0b10100000,
        FLUSH_TX                = 0b11100001,
        FLUSH_RX                = 0b11100010,
        REUSE_TX_PL             = 0b11100011,
        R_RX_PL_WID             = 0b01100000,
        R_REGISTER              = 0b00000000,
        W_REGISTER              = 0b00100000,
        W_ACK_PAYLOAD           = 0b10101000,
        W_TX_PAYLOAD_NO_ACK     = 0b10110000,
        NOP                     = 0b11111111,    
    };

    enum data_rate
    {
        DATA_RATE_1MBPS = 0b00000000,
        DATA_RATE_2MBPS = 0b00001000,
        DATA_RATE_250KBPS = 0b00100000,
    };
    
    class nRF24 
    {
    public:
        nRF24(uint8_t cePin, uint8_t csnPin);

        /*
         *  readSPI
         *  
         *  args: 
         *      arr (byte *)
         *      size (uint32_t)
         * 
         *  Description:
         *      The readSPI method works by reading
         *      over `size' bytes from the RX FIFO into
         *      the buffer `arr'
         *      
         *      This is done by reading one byte at a time 
         */
        void readSPI(byte * arr, uint32_t size);

        /*
         *  writeSPI
         *  
         *  args: 
         *      arr (char *)
         *      size (uint32_t)
         * 
         *  Description:
         *      The readSPI method works by sending
         *      over `size' bytes from the buffer `arr'
         *      
         *      This is done by sending one byte at a time
         *      until the FIFO is full 
         */
        void writeSPI(char * arr, uint32_t size);   

        /*
         *  flushTXPayload
         * 
         *  args:
         *      none.
         *  
         *  Description:
         *      The flushTXPayload method will flush the TX FIFO.
         */
        void flushTXPayload();
        
        /*
         *  flushRXPayload
         * 
         *  args:
         *      none.
         *  
         *  Description:
         *      The flushTXPayload method will flush the RX FIFO.
         */
        void flushRXPayload();

        /*
         *  setToReceiver
         *  
         *  args:
         *      none.
         * 
         *  Description:
         *      The setToReciever method transitions 
         *      the nRF module into the standby mode 
         *      and adjusts registers to make the device
         *      a primary reciever.
         * 
         *      This is accomplished by following transitions in the 
         *      state diagram (see data sheet).
         */
        void setToReceiver();

        /*
         *  setToTransmitter
         *  
         *  args:
         *      none.
         * 
         *  Description:
         *      The setToTransmitter method transitions 
         *      the nRF module into the standby mode 
         *      and adjusts registers to make the device
         *      a primary transmitter.
         * 
         *      This is accomplished by following transitions in the 
         *      state diagram (see data sheet).
         */
        void setToTransmitter();

        /*
         *  setChannel
         *  
         *  args:
         *      channel (uint8_t)
         * 
         *  Description:
         *      The setChannel method will validate the input
         *      and write the channel to the appropriate register.
         *      
         *      Note that the system works "better" when the channel is near 
         *      the middle of the range of available channels (76). This will
         *      lead to less interference from nearby bands
         */
        void setChannel(uint8_t channel);

        /*
         *  getChannel()
         * 
         *  args:
         *      none.
         *      
         *  Description:
         *      Returns the value of the channel stored in the corresponding register
         */
        uint8_t getChannel();

        /*
         *  setReadingPipeAddr
         * 
         *  args:
         *      pipe    (uint8_t)
         *      address (uint8_t *)
         * 
         *  Description:
         *      Given a pipe and address, the module will be configured to read
         *      from the pipe (which is a logical address for the RF channel)
         *      
         *      Note that the address of the transmitter and the reciever should be
         *      be the same.
         */
        void setReadingPipeAddr(uint8_t pipe, uint8_t * address);

        /*
         *  setWritingAddress
         *  
         *  args:
         *      address - (uint8_t)
         * 
         *  Description:
         *      Set's the address of the transmitter.
         * 
         *      Note that the address of the transmitter and the receiver should
         *      be the same.
         */
        void setWritingAddress(uint8_t * address);

        /*
         *  setAddressWidth
         *  
         *  args: 
         *      aw - (uint8_t)
         *  
         *  Description:
         *      Sets the address width. This width determines the number of bytes that will be
         *      used in the address itself. If the address is greater than MAX_ADDRESS_WIDTH then
         *      the aw is set to MAX_ADDRESS_WIDTH. The 
         */
        void setAddressWidth(uint8_t aw);

        /*
         *  getWritingAddress
         *  
         *  args:
         *      buff (uint8_t)
         *  
         *  Description:
         *      Retrieves the address from the module and 
         *      and fils the buffer with the address.
         * 
         *      Note that the address/address width must be set
         *      before using this method. Otherwise the behavior
         *      is undefined. 
         */
        void getWritingAddress(uint8_t * buff);


        /*
         *  setDataRate
         *      
         *  args:
         *      rate (data_rate)
         *      
         *  Description:
         *      Given a data rate from the following list
         *      { DATA_RATE_1MBPS, DATA_RATE_2MBPS, DATA_RATE_250KBPS }
         *      the module is set to that rate. 
         */
        void setDataRate(data_rate rate);

        /*
         *  txFIFOEmpty
         *  
         *  args:
         *      none. 
         *  
         *  Description:
         *      Returns whether the txFIFO is full
         *      
         *      Note that this is typically used to ensure
         *      that data is in the fifo. 
         */
        bool txFIFOEmpty();

        /*
         *  txFIFOFull
         * 
         *  args:
         *      none.
         * 
         *  Description: 
         *      Returns whethere the txFIFO is empty
         *      
         *      Note that this is typically used to ensure that 
         *      TX FIFO is empty when flushed.
         */
        bool txFIFOFull();

        uint8_t status();

    private:
        uint8_t cePin_;
        uint8_t csnPin_;
        uint8_t addressWidth_;        
        bool txMode_;

        data_frame_u makeFrame(uint8_t cmd, byte data);
        
        /*
         *  setRegister
         *  
         *  args:
         *      r       (uint8_t)
         *      data    (uint8_t)
         *  
         *  Description: 
         *      This private method is used to configure registers
         *      on the module. The majority of them only require
         *      a single byte of data to be configured.
         */
        void setRegister(uint8_t r, uint8_t data);

        /*
         *  getRegister
         * 
         *  args: 
         *      r (uint8_t)
         *  
         *  Description:
         *      This private method is used to get the value stored in
         *      a register. This method is typically used to ensure that
         *      entire registers are not overwritten.
         */
        uint8_t getRegister(uint8_t r);

        /*
         *  beginTransaction
         *  
         *  args:
         *      none.
         *  
         *  Description:
         *      This method is a wrapper for the SPI method.
         *      It was made to reduce the amount of duplicated 
         *      code. 
         */
        void beginTransaction();

        /*
         *  endTransaction
         *  
         *  args:
         *      none.
         * 
         *  Description:
         *      Similar to the beginTransaction method, this
         *      method is a wrapper for the SPI end transaction method.
         *      It was made to reduce the amount of duplicated code.
         */
        void endTransaction();

        /*
         *  powerOn
         *  
         *  args:
         *      none. 
         * 
         *  Description:
         *      Puts the module into standby mode. 
         *      This occurs when the module is going to be made
         *      a transmitter or receiver. See the state diagram
         *      in the data sheet for more detail.
         */
        void powerOn();

        /*
         *  powerOff
         *  
         *  args: 
         *      none.
         *  
         *  Description:
         *      Places the module into the power down mode.
         *      This method is not really used during operation
         *      but is here for completeness.
         */
        void powerOff();
        
        /*
         *  setToStandBy
         *  
         *  args:
         *      none.
         * 
         *  Description:
         *      Puts the module into standby mode
         *      by setting the appropriate registers.
         */
        void setToStandBy();

        /*
         *  flushPayload
         *  
         *  args:
         *      cmd (uint8_t)
         *  
         *  Description:
         *      Will flush the payload corresponding to command. The 
         *      command will either be the TX payload or the RX payload.
         */
        void flushPayload(uint8_t cmd);

        /*
         *  getFIFOStatus
         *  
         *  args: 
         *      r (uint8_t)
         * 
         *  Description:
         *      Returns the value of the status register. This method
         *      was primarily used for debugging the writeSPI method. 
         */
        uint8_t getFIFOStatus(uint8_t r);
    };
}; // nRF24Module
#endif /* _NRF_24_ */