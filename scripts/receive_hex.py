#!/bin/python3
"""
When run in main, the script will record and send over the user's
desired communication configurations and a file in the form of raw hex 
and its file extension to a connected Arduino for RF file transfers.

Params:
    sys.argv[1]:
        Absolute path of Serial port

    sys.argv[2]:
        Baudrate of Serial port

    sys.argv[3]:
        Filepath of file to send

    sys.argv[4]:
        Plain raw-hex string of file to send

Sends:
    channel:
        0-125
        
    address:
        0-10000

    file_extension_bytes:
        File extension of our sent file

    raw_hex_bytes:
        Raw-hex of our file

"""


import sys
import serial
import time
from subprocess import call
from arduino_serial_io import *

RX_FILE_PATH="./rx-files/"

# used to debug communication
DEBUG = 1


if __name__ == "__main__":

    channel, address = setConfig()

    # configuring our serial
    ser = serial.Serial()
    ser.port = sys.argv[1]
    ser.baudrate = int(sys.argv[2])
    ser.open()

    flushSerial(ser)

    # sending over our configurations
    ser.write(channel)
    ser.write(address)

    print("\nListening for file please wait...")

    # get rid of residual END_CHAR
    file_extension = END_CHAR
    while file_extension == END_CHAR:  
        handshake(ser)
        file_extension = getData(ser).strip()  # remove extra whitespace

    file = ""
    data = ""
    while data != END_CHAR:
        file += data
        handshake(ser)
        data = getData(ser)

    ser.close()

    # convert the file back to its orriginal form
    call( 'printf "' + file + '"' + " | xxd -r -p >" + RX_FILE_PATH + str(int(time.time())) + "." + file_extension, shell=True)
