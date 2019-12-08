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
    // give module some time to settle 
    digitalWrite(cePin_, LOW);
    digitalWrite(cePin_, HIGH);
    delay(5);
    /* address width is 5 bytes by default */
    setAddressWidth(MAX_ADDRESS_WIDTH);
    
    setRegister(DYNPD, 0);
    setRegister(FEATURE, 0);
    
    /* when intialized the module will be in standby so that the user can configure */
    flushRXPayload();
    flushTXPayload();

    setToStandBy();

    setRegister(CONFIG, (getRegister(CONFIG) & 0b11111110));
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
    delayMicroseconds(140);

    txMode_ = false;
}

void 
nRF24::setToTransmitter()
{
    /* send to standby mode */
    digitalWrite(cePin_, LOW);
    delayMicroseconds(400); // TODO: make this a constant
    /* flush the fifo! */
    flushTXPayload();
    /* set the PRIM_RX field to 0 */
    byte data = 0b11111110;
    setRegister(CONFIG, (getRegister(CONFIG) & data));

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

/*
 *  Unsure of the delays here. Transmission depends on 
 *  the type of board you have as well.
 */
void
nRF24::writeSPI(char * arr, uint32_t size)
{
    // write to the TX buffer when the CE pin is low
    digitalWrite(cePin_, LOW);

    beginTransaction();
    /* write a payload with no ack packet */
    data_frame_u df = makeFrame(W_TX_PAYLOAD_NO_ACK, NO_DATA);
    uint8_t status_data = SPI.transfer(df.atomic_frame.preamble);

#if DEBUG
    Serial.print("Status is: ");
    Serial.println(status_data);
#endif

    for (int i = 0; i < size; ++i) {
        SPI.transfer(arr[i]);
    }

    // a full 32 bytes need to be sent
    for (int i = size; i < 32; ++i) {
        SPI.transfer(NO_DATA);
    }

    endTransaction();

    digitalWrite(cePin_, HIGH);
    uint8_t data = 0b00110000;
    while (!(status() & data)) {}

    digitalWrite(cePin_, LOW);

    flushTXPayload();
    
    setRegister(STATUS, data);
    
    //flushTXPayload();
}

void 
nRF24::setChannel(uint8_t channel)
{
    if (channel > NUM_CHANNELS || channel < 0) channel = 0;
    auto CHANNEL_MASK = 0x7F;
    setRegister(RF_CH, (CHANNEL_MASK & channel));
}

uint8_t 
nRF24::getChannel()
{
    return getRegister(RF_CH);
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
nRF24::setWritingAddress(uint8_t * address)
{
    /* This method should not work when module is not in transmitting mode */
    if (!txMode_) return;
    beginTransaction();

    data_frame_u df = makeFrame((W_REGISTER | (REGISTER_MASK & TX_ADDR)), NO_DATA);
    SPI.transfer(df.atomic_frame.preamble);
    int i = 0;
    
    for (; i < addressWidth_; ++i) {
        SPI.transfer(address[i]);
    }

    for (i = addressWidth_; i < MAX_ADDRESS_WIDTH; ++i) {
        SPI.transfer(NO_DATA);
    }

    endTransaction();

    // per data sheet need to also write to RX_ADDR_P0 with address
    beginTransaction();

    df = makeFrame((W_REGISTER | (REGISTER_MASK & RX_ADDR_P0)), NO_DATA);
    SPI.transfer(df.atomic_frame.preamble);

    i = 0;
    for (; i < addressWidth_; ++i) {
        SPI.transfer(address[i]);
    }

    for (i = addressWidth_; i < MAX_ADDRESS_WIDTH; ++i) {
        SPI.transfer(NO_DATA);
    }

    endTransaction();

    uint8_t buff_sz = 32;
    setRegister(RX_PW_P0, buff_sz);
    
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
    // making sure that aw is formatted correctly
    // to match the data sheet definition
    aw = 0b11111100 & (0b11111100 | (aw-2));  
    setRegister(SETUP_AW, (getRegister(SETUP_AW) & aw));
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
    SPI.transfer(df.atomic_frame.preamble);
    uint8_t result = SPI.transfer(NO_DATA);
    endTransaction();

#if DEBUG
    GET_VAL_16BIT(result)
#endif
    
    return result;
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

void 
nRF24::getWritingAddress(uint8_t * buff)
{    
    beginTransaction();

    data_frame_u df = makeFrame(R_REGISTER | (REGISTER_MASK & TX_ADDR), NO_DATA);
    SPI.transfer(df.atomic_frame.preamble);
    for (int i = 0; i < addressWidth_; ++i) {
        buff[i] = SPI.transfer(NO_DATA);
    }
    endTransaction();
}

bool
nRF24::txFIFOEmpty()
{
    uint8_t tx_empty_bit = 0b00010000; // per the data sheet
    uint8_t empty_val = 0;
    uint8_t status = getFIFOStatus(FIFO_STATUS);
    Serial.print("Empty status is: ");
    Serial.println(status & tx_empty_bit);
    return (status & tx_empty_bit) == empty_val; 
}

bool
nRF24::txFIFOFull()
{
    uint8_t tx_full_bit = 0b00100000;
    uint8_t full_val = 1;
    uint8_t status = getFIFOStatus(FIFO_STATUS);
    Serial.print("Full status is: ");
    Serial.println(status & tx_full_bit);
    return (status & tx_full_bit) == full_val;
}

uint8_t 
nRF24::status() 
{
    beginTransaction();
    uint8_t status = SPI.transfer(NOP);
    endTransaction();

    return status;
}

uint8_t 
nRF24::getFIFOStatus(uint8_t r)
{
    beginTransaction();

    data_frame_u df = makeFrame((R_REGISTER | (REGISTER_MASK & r)), NO_DATA);
    SPI.transfer(df.atomic_frame.preamble);
    uint8_t status = SPI.transfer(NO_DATA);

    endTransaction();

    return status;
}