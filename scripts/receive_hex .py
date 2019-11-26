#!/bin/python3
"""
When run in main, the script will record sent over raw hex and file extension
from our Arduino.

Params: 
    sys.argv[1]:
        Absolute path of Serial port

    sys.argv[2]: 
        Baudrate of Serial port   
"""
import sys
import serial
import time

# Max amount of time to wait duing our handshake before we throw and exception
MAX_HANDSHAKE_SEC = 10

# used to communicate state changes between the Arduino and Computer
HANDSHAKE_CHAR = '\t'

# byte to send to Arduino to signal that we are about to send a file
HANDSHAKE_CHAR_BYTE = 9

# tell Arduino to switch to transmit state
HANDSHAKE_BYTE = bytearray()
HANDSHAKE_BYTE.extend([HANDSHAKE_CHAR_BYTE])

# used to debug communication
DEBUG = 0


def handshakeRX(ser):
    """
    
    """
    

def printData(ser, end_char='\n'):
    """
    Prints data sent to the computer from the Arduino over seral.
    Transmissions are seperated by the HANDSHAKE_CHAR.

    Params:
        ser:
            Our initiallized pyserial serial port
        end_char:
            String: End line character after print statement


    Outputs:
        None
    """
    data = ""
    curr = ""
    # time.sleep(1) # wait for the Arduino
    while curr !=  HANDSHAKE_CHAR:
        while curr !=  HANDSHAKE_CHAR and ser.in_waiting:
            data += curr
            curr = ser.read(1).decode("utf-8")
        
    print(data, end=end_char)
 

if __name__ == "__main__":

    # configuring our serial
    ser = serial.Serial()
    ser.port = sys.argv[1]
    ser.baudrate = int(sys.argv[2])
    ser.open()
    

    # printData(ser, '') #output our received file
    
    ser.close()
