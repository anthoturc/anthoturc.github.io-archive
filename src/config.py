#!/bin/python3

import sys
import serial


# RF frequency channel bounds
MAX_CHANNEL=125
MIN_CHANNEL=0

# Data pipe address bounds
MAX_ADDRESS=16777215
MIN_ADDRESS=0

# BAUD for serial communication
BAUD_RATE=115200


if __name__ == "__main__":

    # init pipe and channel to out of bounds
    channel=-1
    address=-1

    print("Enter channel ({0}-{1}): ".format(MIN_CHANNEL, MAX_CHANNEL))
    while not (MIN_CHANNEL <= channel <= MAX_CHANNEL):
        channel = int(input())
        if not (MIN_CHANNEL <= channel <= MAX_CHANNEL):
            print("Invalid input. Re-enter channel ({0}-{1}): ".format(MIN_CHANNEL, MAX_CHANNEL))
    
    print("Enter address ({0}-{1}): ".format(MIN_ADDRESS, MAX_ADDRESS))
    while not (MIN_ADDRESS <= address <= MAX_ADDRESS):
        address = int(input())
        if not (MIN_ADDRESS <= channel <= MAX_ADDRESS):
            print("Invalid input. Re-enter address ({0}-{1}): ".format(MIN_ADDRESS, MAX_ADDRESS))

    ser = serial.Serial()
    ser.baudrate = BAUD_RATE
    ser.port = sys.argv[1]
    ser.open()
    
    start_signal = bytearray()
    start_signal.extend([9,9,9,9,9])
    channel = channel.to_bytes(1, byteorder='little')
    address = address.to_bytes(4, byteorder='little')

    ser.write(start_signal)
    ser.write(channel)
    ser.write(address)
    
    ser.close()
