#include "nRF24L01.h"
#include <stdint.h>
#include <Arduino.h>

using namespace nRF24Module;

nRF24::nRF24(uint8_t cePin, uint8_t csnPin) : cePin_(cePin), csnPin_(csnPin) {}

void
nRF24::readSPI(uint8_t numBytes)
{
    // drive the CSN pin low 

}