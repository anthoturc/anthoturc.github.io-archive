#include <SPI.h>
#include <stdint.h>
#include <Arduino.h>

#include "nRF24L01.h"


using namespace nRF24Module;

nRF24::nRF24(uint8_t cePin, uint8_t csnPin) 
    : cePin_(cePin), csnPin_(csnPin), txMode_(false)
{
    SPI.begin();
    pinMode(cePin_, OUTPUT);
    pinMode(csnPin_, OUTPUT);

    setAddressWidth();
    /* radio should be reciever by default */
    setToReceiver();
}

void
nRF24::setToReceiver()
{
    pinMode(cePin_, LOW);
    /* per the data sheet put the chip in power up mode */
    byte data = 0b00001011;
    writeConfiguration(W_REGISTER | CONFIG, data);
}

void 
nRF24::setToTransmitter()
{
    pinMode(cePin_, HIGH);
    /* per the data sheet put the chip in power up mode */
    byte data = 0b00001010; 
    writeConfiguration(W_REGISTER | CONFIG, data);
}


/* basic read and write operations for the radio */
void
nRF24::readSPI(byte * arr, uint32_t size)
{
    SPI.beginTransaction(SPI_SETTINGS);
    digitalWrite(csnPin_, LOW);

    data_frame_u df = makeFrame(R_RX_PAYLOAD, NO_DATA);
    SPI.transfer(df.atomic_frame.preamble);
    // create the frame to send over 
    for (int i = 0; i < size; ++i) {
        uint8_t recv = SPI.transfer(NO_DATA);
        arr[i] = recv;
    }

    // get the response back
    SPI.endTransaction();
    digitalWrite(csnPin_, HIGH);
}

void
nRF24::writeSPI(byte * arr, uint32_t size)
{
    SPI.beginTransaction(SPI_SETTINGS);
    digitalWrite(csnPin_, LOW);
    
    data_frame_u df = makeFrame(W_TX_PAYLOAD, NO_DATA);
    SPI.transfer(df.atomic_frame.preamble);
    for (int i = 0; i < size; ++i) {
        SPI.transfer(arr[i]);
    }
    SPI.endTransaction();
    digitalWrite(csnPin_, HIGH);
}

void 
nRF24::setChannel(uint8_t channel)
{
    if (channel > NUM_CHANNELS || channel < 0) return;

    writeConfiguration(W_REGISTER | RF_CH, channel);
}

void 
nRF24::setReadingPipeAddr(uint8_t pipe, uint8_t * address)
{
    /* pipe must be in range [0, N_PIPES] */
    if (pipe > N_PIPES || pipe < 0) return;

    SPI.beginTransaction(SPI_SETTINGS);
    digitalWrite(csnPin_, LOW);
    
    byte data = W_REGISTER;
    byte payloadWidth = W_REGISTER;
    switch (pipe) {
        case 0:
            data |= RX_ADDR_P0;
            payloadWidth |= RX_PW_P0;
            break;
        case 1:
            data |= RX_ADDR_P1;
            payloadWidth |= RX_PW_P1;
            break;
        case 2:
            data |= RX_ADDR_P2;
            payloadWidth |= RX_PW_P2;
            break;
        case 3:
            data |= RX_ADDR_P3;
            payloadWidth |= RX_PW_P3;
            break;
        case 4:
            data |= RX_ADDR_P4;
            payloadWidth |= RX_ADDR_P4;
            break;
        case 5:
            data |= RX_ADDR_P5;
            payloadWidth |= RX_ADDR_P5;
            break;
        default:
            break;
    }
    data_frame_u df = makeFrame(data, NO_DATA);
    SPI.transfer(df.atomic_frame.preamble);
    for (int i = 0; i < ADDRESS_WIDTH; ++i) {
        SPI.transfer(address[i]);
    }

    SPI.endTransaction();
    digitalWrite(csnPin_, HIGH);
    
    delay(100);    

    /* set the payload width to be 32 bits */
    data = 0b00010000;    
    writeConfiguration(payloadWidth, data);
}

void 
nRF24::setListeningAddr(uint8_t * address)
{
    SPI.beginTransaction(SPI_SETTINGS);
    digitalWrite(csnPin_, LOW);

    data_frame_u df = makeFrame(W_REGISTER | TX_ADDR, NO_DATA);
    SPI.transfer(df.atomic_frame.preamble);
    for (int i = 0; i < ADDRESS_WIDTH; ++i) {
        SPI.transfer(address[i]);
    }

    SPI.endTransaction();
    digitalWrite(csnPin_, HIGH);
    
    setReadingPipeAddr(0, address);
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
nRF24::flushTXPayload()
{
    byte data[1] = { FLUSH_TX };
    writeSPI((byte *)data, 1);
}
        
void 
nRF24::flushRXPayload()
{
    byte data[1] = { FLUSH_RX };
    writeSPI((byte *)data, 1);
}

void 
nRF24::setAddressWidth()
{
    /* only allow ADDRESS_WIDTH bytes to be sent over */
    writeConfiguration(W_REGISTER | SETUP_AW, ADDRESS_WIDTH_CONFIG);
}

void 
nRF24::setDataRate()
{
    /* set RF to 250kbps and -18dBm */
    byte data = 0b00100000;
    writeConfiguration(W_REGISTER | RF_SETUP, data);
}

void 
nRF24::writeConfiguration(uint8_t cmd, uint8_t data)
{
    SPI.beginTransaction(SPI_SETTINGS);
    digitalWrite(csnPin_, LOW);

    data_frame_u df = makeFrame(cmd, data);

    SPI.transfer16(df.data_frame);
    SPI.endTransaction();

    digitalWrite(csnPin_, LOW);
}