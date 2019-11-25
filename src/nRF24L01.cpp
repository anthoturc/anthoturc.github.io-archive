#include <SPI.h>
#include <stdint.h>
#include <Arduino.h>

#include "nRF24L01.h"

/* Used to debug the results of the SPI.transfer() call */
#define DEBUG 0
#define GET_VAL_16BIT(result)  Serial.print("Status is: ");         \
    Serial.println((result) >> sizeof(uint8_t));                    \
    Serial.println("Data is: ");                                    \
    Serial.println((result) << sizeof(uint8_t)) >> sizeof(uint8_t)); 

using namespace nRF24Module;

nRF24::nRF24(uint8_t cePin, uint8_t csnPin) 
    : cePin_(cePin), csnPin_(csnPin), txMode_(false)
{
    SPI.begin();
    pinMode(cePin_, OUTPUT);
    pinMode(csnPin_, OUTPUT);

    /* address width is 5 bytes by default */
    setAddressWidth(MAX_ADDRESS_WIDTH);
    /* when intialized the module will be in standby so that the user can configure */
    setToStandBy();
}

void
nRF24::setToStandBy()
{
    /* standby is acheived by setting the PWR_UP bit in the CONFIG register to HIGH */
    powerOn();
    /* there is a 1.5 ms delay while in standby mode */
    delay(5);
}

void
nRF24::setToReceiver()
{
    /* go to standby mode */
    digitalWrite(cePin_, LOW);

    /* set the PRIM_RX field to 1 and the CE pin to HIGH */
    byte data = 0b0000001;
    setRegister(CONFIG, (getRegister(CONFIG | data)));
    digitalWrite(cePin_, HIGH);
    /* delay for RX settings is 130 micro sec. so delay 1 ms */    
    delay(1);

    txMode_ = false;
}

void 
nRF24::setToTransmitter()
{
    /* send to standby mode */
    digitalWrite(cePin_, LOW);
    /* flush the fifo! */

    /* set the PRIM_RX field to 0 */
    byte data = 0b11111110;
    setRegister(CONFIG, (getRegister(CONFIG) & data));
    digitalWrite(cePin_, HIGH);
    /* delay for TX settings is 130 micro sec. so delay 1 ms */    
    delay(1);

    txMode_ = true;
}

/* basic read and write operations for the radio */
void
nRF24::readSPI(byte * arr, uint32_t size)
{
    beginTransaction();

    data_frame_u df = makeFrame(R_RX_PAYLOAD, NO_DATA);
    uint8_t recv = SPI.transfer(df.atomic_frame.preamble);
#if DEBUG
    Serial.println("Status is: ")
    Serial.println(recv);
#endif
    // create the frame to send over 
    for (int i = 0; i < size; ++i) {
        recv = SPI.transfer(NO_DATA);
        arr[i] = recv;
#if DEBUG
        Serial.print((char)recv);
#endif
    }

#if DEBUG
    Serial.println()
#endif

    endTransaction();
}

void
nRF24::writeSPI(byte * arr, uint32_t size)
{
    beginTransaction();
    
    data_frame_u df = makeFrame(W_TX_PAYLOAD, NO_DATA);
    uint8_t status = SPI.transfer(df.atomic_frame.preamble);

#if DEBUG
    Serial.print("Status is: ");
    Serial.println(status);
#endif

    for (int i = 0; i < size; ++i) {
        SPI.transfer(arr[i]);
    }
    
    endTransaction();
}

void 
nRF24::setChannel(uint8_t channel)
{
    if (channel > NUM_CHANNELS || channel < 0) channel = 0;

    setRegister(RF_CH, (getRegister(RF_CH) | channel));
}

uint8_t 
nRF24::getChannel()
{
    return getRegister(RF_CH);
}

uint8_t 
nRF24::getChannel()
{
    SPI.beginTransaction(SPI_SETTINGS);
    digitalWrite(csnPin_, LOW);

    data_frame_u df = makeFrame(R_REGISTER | RF_CH, NO_DATA);
    uint16_t data =SPI.transfer16(df.data_frame);
    

    SPI.endTransaction();
    digitalWrite(csnPin_, HIGH);

    return ((data << 8) >> 8);
}

void 
nRF24::setReadingPipeAddr(uint8_t pipe, uint8_t * address)
{
    /* pipe must be in range [0, N_PIPES] */
    if (pipe > N_PIPES || pipe < 0) return;

    beginTransaction();
    
    byte data;
    byte payloadWidth;
    switch (pipe) {
        case 1:
            data = RX_ADDR_P1;
            payloadWidth = RX_PW_P1;
            break;
        case 2:
            data = RX_ADDR_P2;
            payloadWidth = RX_PW_P2;
            break;
        case 3:
            data = RX_ADDR_P3;
            payloadWidth = RX_PW_P3;
            break;
        case 4:
            data = RX_ADDR_P4;
            payloadWidth = RX_ADDR_P4;
            break;
        case 5:
            data = RX_ADDR_P5;
            payloadWidth = RX_ADDR_P5;
            break;
        default:
            data = RX_ADDR_P0;
            payloadWidth = RX_PW_P0;
            break;
    }

    data_frame_u df = makeFrame((W_REGISTER | (REGISTER_MASK & data)), NO_DATA);
    
    SPI.transfer(df.atomic_frame.preamble);
    for (int i = 0; i < addressWidth_; ++i) {
        SPI.transfer(address[i]);
    }

    endTransaction();

    /* the FIFOs will use 32 bytes */
    uint8_t numBytes = 32;
    setRegister(payloadWidth, numBytes);
}

data_frame_u
nRF24::makeFrame(uint8_t cmd, byte data)
{
    data_frame_u df;
    df.atomic_frame.preamble = cmd;
    df.atomic_frame.data = data;
    return df;
}

void
nRF24::flushPayload(uint8_t cmd)
{
    beginTransaction();
    
    data_frame_u df = makeFrame(cmd, NO_DATA);
    uint16_t result = SPI.transfer16(df.data_frame);

#if DEBUG
    GET_VAL_16BIT(result)
#endif

    endTransaction();
}

void 
nRF24::flushTXPayload()
{
    flushPayload(FLUSH_TX);
}
        
void 
nRF24::flushRXPayload()
{
    flushPayload(FLUSH_RX);
}

void 
nRF24::setAddressWidth(uint8_t aw)
{
    /* only allow ADDRESS_WIDTH bytes to be sent over */
    if (aw > MAX_ADDRESS_WIDTH || aw < MIN_ADDRESS_WIDTH) aw = MIN_ADDRESS_WIDTH;
    addressWidth_ = aw;
    setRegister(SETUP_AW, (getRegister(SETUP_AW) | aw));
}

void 
nRF24::setDataRate(data_rate rate)
{
    setRegister(RF_SETUP, (getRegister(RF_SETUP) | rate));
}

void
nRF24::powerOn()
{
    /* per the data sheet the 2nd bit should be HIGH */
    byte data = 0b00000010;
    /* to avoid overriding other settings get the data in the CONFIG resgister */
    setRegister(CONFIG, (getRegister(CONFIG) | data));
}

void 
nRF24::powerOff()
{
    /* per the data sheet the 2nd bit should be LOW */
    byte data = 0b11111101;
    /* to avoid overriding other settings get the data in the CONFIG register */
    setRegister(CONFIG , (getRegister(CONFIG) & data));
}

/*
 *  Set the data in register 'r' to 'data'.
 */
void
nRF24::setRegister(uint8_t r, uint8_t data)
{
    beginTransaction();

    data_frame_u df = makeFrame(W_REGISTER | (REGISTER_MASK & r), data);
    uint16_t result = SPI.transfer16(df.data_frame);

    endTransaction();

#if DEBUG
    GET_VAL_16BIT(result);
#endif
}

/*
 *  Get the data stored in register 'r'.
 */
uint8_t 
nRF24::getRegister(uint8_t r)
{
    beginTransaction();

    data_frame_u df = makeFrame(R_REGISTER | (REGISTER_MASK & r), NO_DATA);
    uint16_t result = SPI.transfer16(df.data_frame);

    endTransaction();

#if DEBUG
    GET_VAL_16BIT(result)
#endif
    
    return (result << sizeof(uint8_t)) >> sizeof(uint8_t);
}

void 
nRF24::beginTransaction()
{
    digitalWrite(csnPin_, LOW);
    SPI.beginTransaction(SPI_SETTINGS);
}

void 
nRF24::endTransaction()
{
    SPI.endTransaction();
    digitalWrite(csnPin_, HIGH);
}