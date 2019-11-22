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
}

/* other instructions that wrap the reading and writing */
byte * 
nRF24::readRegister(uint8_t reg)
{

}


/* basic read and write operations for the radio */
byte *
nRF24::readSPI(uint8_t addr)
{
    SPI.beginTransaction(SPISettings(SPI_FRQ, LSBFIRST, SPI_MODE0));
    digitalWrite(csnPin_, LOW);
    
    // create the frame to send over 

    // get the response back
    SPI.endTransaction();
    digitalWrite(csnPin_, HIGH);

    return { 0 };
}

void
nRF24::writeSPI(byte * arr, uint32_t size)
{
    SPI.beginTransaction(SPISettings(SPI_FRQ, LSBFIRST, SPI_MODE0));
    digitalWrite(csnPin_, LOW);
    
    
    // create the frame to send over
    for (int i = 0; i < size; ++i) {
        data_frame_u frame = makeFrame(arr[i]); 
        SPI.transfer(frame.data_frame);
    }

    SPI.transferBytes(makeDataFrame(), );    
    // get the response back 

    SPI.endTransaction();
    digitalWrite(csnPin_, HIGH);
}