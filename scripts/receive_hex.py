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
from arduino_serial_io import *
 
# used to debug communication
DEBUG = 1

if __name__ == "__main__":

    # configuring our serial
    ser = serial.Serial()
    ser.port = sys.argv[1]
    ser.baudrate = int(sys.argv[2])
    ser.open()
    

    # printData(ser, '') #output our received file
    
    ser.close()
