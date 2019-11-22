#include <SPI.h>
#include <stdint.h>
#include <Arduino.h>

#include "nRF24L01.h"


using namespace nRF24Module;

nRF24::nRF24(uint8_t cePin, uint8_t csnPin) 
    : cePin_(cePin), csnPin_(csnPin) 
{
    SPI.begin();
    pinMode(cePin_, OUTPUT);
    pinMode(csnPin_, OUTPUT);

    /* radio should be reciever by default */
    setToReceiver();
}

void
nRF24::setToReceiver()
{
    pinMode(cePin_, HIGH);
    SPI.beginTransaction(SPISettings(SPI_FRQ, LSBFIRST, SPI_MODE0));
    digitalWrite(csnPin_, LOW);
    
    /* per the data sheet put the chip in power up mode */
    byte configuration = 0b00001011;
    data_frame_u df = makeFrame(W_REGISTER, configuration);
    
    SPI.transfer16(df.data_frame);
    digitalWrite(csnPin_, HIGH);
}

void 
nRF24::setToTransmitter()
{
    pinMode(cePin_, HIGH);
    SPI.beginTransaction(SPISettings(SPI_FRQ, LSBFIRST, SPI_MODE0));
    digitalWrite(csnPin_, LOW);
    
    /* per the data sheet put the chip in power up mode */
    byte configuration = 0b00001010; 
    data_frame_u df = makeFrame(W_REGISTER, configuration);
    
    SPI.transfer16(df.data_frame);

    SPI.endTransaction();
    digitalWrite(csnPin_, HIGH);
}


/* basic read and write operations for the radio */
void
nRF24::readSPI(byte * arr, uint32_t size)
{
    SPI.beginTransaction(SPISettings(SPI_FRQ, LSBFIRST, SPI_MODE0));
    digitalWrite(csnPin_, LOW);

    data_frame_u df = makeFrame(R_RX_PAYLOAD, NO_DATA);
    
    // create the frame to send over 
    for (int i = 0; i < size/2; i+=2) {
        uint16_t recv = SPI.transfer16(df.data_frame);
        arr[i] = (recv << 8) >> 8;
        arr[i+1] = recv >> 8;
    }

    // get the response back
    SPI.endTransaction();
    digitalWrite(csnPin_, HIGH);
}

void
nRF24::writeSPI(byte * arr, uint32_t size)
{
    SPI.beginTransaction(SPISettings(SPI_FRQ, LSBFIRST, SPI_MODE0));
    digitalWrite(csnPin_, LOW);
    
    for (int i = 0; i < size; ++i) {
        data_frame_u frame = makeFrame(W_TX_PAYLOAD, arr[i]); 
        SPI.transfer16(frame.data_frame);
    }

    SPI.endTransaction();
    digitalWrite(csnPin_, HIGH);
}

data_frame_u
nRF24::makeFrame(commands cmd, byte data)
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