#!/bin/python3

import sys
import serial

# BAUD for serial communication
BAUD_RATE=115200


if __name__ == "__main__":
    ser = serial.Serial()
    ser.baudrate = BAUD_RATE
    ser.port = sys.argv[1]
    ser.open()
    
    print(sys.argv)
    # send over file extension for decoding
    file_extension = bytearray()
    raw_hex = bytearray()
    transmit = bytearray()

    # ::-1 because little endian
    file_extension.extend(map(ord, sys.argv[3].split('.')[1][::-1]))   # take everything after the .
    raw_hex.extend(map(ord, sys.argv[2][::-1]))  # TODO: send over size of hex as checksum
    transmit.extend([9])

    ser.write(transmit) # switch to transmit state
    while True:
        config_string = ""
        while ser.in_waiting:
            config_string += ser.read(1).decode("utf-8") # check that arduino confirms
            print(config_string)
        if config_string == "ready":
            break

    ser.write(file_extension)
    ser.write(raw_hex)
    
    ser.close()
