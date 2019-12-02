#!/bin/python3
"""
When run in main, the script will record and send over the user's
desired communication configurations to a connected Arduino for RF
file transfers.

Params:
    sys.argv[1]:
        Absolute path of Serial port

    sys.argv[2]:
        Baudrate of Serial port

Sends:
    start_signal:
        signal to the Arduino that config params are about to be sent
        this step is important for flushing the Arduino's Serial before
        configurations are sent

    channel:
        0-125

    address:
        0-10000
"""

import sys
import serial
from arduino_serial_io import (MIN_CHANNEL, MAX_CHANNEL, MIN_ADDRESS, enableTX,
                               MAX_ADDRESS, ENDIANESS, getIntInput, flushSerial)

if __name__ == "__main__":

    # init pipe and channel to out of bounds
    channel = -1
    address = -1

    print("Enter channel ({0}-{1}): ".format(MIN_CHANNEL, MAX_CHANNEL))
    while not MIN_CHANNEL <= channel <= MAX_CHANNEL:
        channel = getIntInput()
        if not MIN_CHANNEL <= channel <= MAX_CHANNEL:
            print("Invalid input. Re-enter channel ({0}-{1}): ".format(MIN_CHANNEL, MAX_CHANNEL))

    print("Enter address ({0}-{1}): ".format(MIN_ADDRESS, MAX_ADDRESS))
    while not MIN_ADDRESS <= address <= MAX_ADDRESS:
        address = getIntInput()
        if not MIN_ADDRESS <= address <= MAX_ADDRESS:
            print("Invalid input. Re-enter address ({0}-{1}): ".format(MIN_ADDRESS, MAX_ADDRESS))

    # configuring our serial
    ser = serial.Serial()
    ser.port = sys.argv[1]
    ser.baudrate = int(sys.argv[2])
    ser.open()
    flushSerial(ser)

    # sending over our configurations
    channel = channel.to_bytes(1, byteorder=ENDIANESS)
    address = address.to_bytes(4, byteorder=ENDIANESS)
    ser.write(channel)
    ser.write(address)
    # enableTX(ser)

    ser.close()
