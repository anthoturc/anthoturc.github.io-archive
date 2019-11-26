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


# We are going to store the configured integers in our Arduino in a Union, 
# by sending over our individal bytes and storing them in memory.  
# Thus, we need to consider the endianess of our Arduino
ENDIANESS = 'little'

# byte to send to Arduino to signal that we are about to send a file
HANDSHAKE_CHAR_BYTE = 9

# How many consecutive signals we need to send over 
HANDSHAKE_REPS = 5

# RF frequency channel bounds
MAX_CHANNEL = 125
MIN_CHANNEL = 0

# Data pipe address bounds
MAX_ADDRESS = 10000
MIN_ADDRESS = 0

def sendFlushSerialSignal(ser):
    """
    Sends to the Arduino HANDSHAKE_BYTE HANDSHAKE_REPS times.  On the Arduino side, this
    function relies on the flushSerial() function in the serial_io class to flush the
    serial and prepare the Arsuino for communication.

    Params:
        ser:
            Our initiallized pyserial serial port

    Outputs:
        None
    """
    # signal to the Arduino that we are about to send over configuration data
    flush_signal = bytearray()
    flush_signal.extend([HANDSHAKE_CHAR_BYTE for _ in range(HANDSHAKE_REPS)])
    ser.write(flush_signal)

if __name__ == "__main__":

    # init pipe and channel to out of bounds
    channel = -1
    address = -1

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

    # configuring our serial
    ser = serial.Serial()
    ser.port = sys.argv[1]
    ser.baudrate = int(sys.argv[2])
    ser.open()
    sendFlushSerialSignal(ser)
    
    # sending over our configurations
    channel = channel.to_bytes(1, byteorder=ENDIANESS)
    address = address.to_bytes(4, byteorder=ENDIANESS)    
    ser.write(channel)
    ser.write(address)
    
    ser.close()
