#!/bin/python3

import sys
import serial

# BAUD for serial communication
BAUD_RATE = 115200

# Bit to send to Arduino to signal that we are about to send a file
TRANSMISSION_BIT = 9

EXTENSION_LEN = 10


if __name__ == "__main__":
    ser = serial.Serial()
    ser.baudrate = BAUD_RATE
    ser.port = sys.argv[1]
    ser.open()
    
    print(sys.argv)
    # send over file extension for decoding
    file_extension_bytes = bytearray()

    # take everything after the .
    file_extension = []
    file_extension.extend(map(ord, sys.argv[3].split('.')[1]))

    # fill in unised chars with spaces
    space_pad = [ord(" ") for _ in range(EXTENSION_LEN - len(file_extension))]
    file_extension.extend(space_pad)
    
    
    raw_hex_bytes = bytearray()
    transmit_bytes = bytearray()

    # ::-1 because little endian
    file_extension_bytes.extend(file_extension)
    raw_hex_bytes.extend(map(ord, sys.argv[2]))  # TODO: send over size of hex as checksum
    transmit_bytes.extend([TRANSMISSION_BIT])

    ser.write(transmit_bytes) # switch to transmit state
    while True:
        config_string = ""
        while ser.in_waiting:
            config_string += ser.read(1).decode("utf-8") # check that arduino confirms
            print(config_string)
            if config_string == "\t":
                break

        if config_string == "\t":
            break

    ser.write(file_extension_bytes)
    # ser.write(raw_hex_bytes)
    
    ser.close()
