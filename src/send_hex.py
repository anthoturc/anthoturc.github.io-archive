#!/bin/python3
"""
When run in main, the script will record and send over the raw hex and file extension
of a previously specified file to a connected Arduino for RF file transfers.

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
    transmit_byte:
        Signals to the Arduino to change to TX state
    
    file_extension_bytes:
        File extension of our sent file
    
    raw_hex_bytes:
        Raw-hex of our file
    
"""
import sys
import serial

 
if __name__ == "__main__":

    # Bit to send to Arduino to signal that we are about to send a file
    TRANSMISSION_BIT = 9

    # Size of char array in union representing extension on Arduino
    EXTENSION_LEN = 10

    # used to communicate state changes between the Arduino and Computer
    CONFIRMATION_CHAR = '\t'

    # TODO: send over size of hex as checksum if have time
    raw_hex_bytes = bytearray()  # our file to be sent
    transmit_byte = bytearray()  # tell Arduino to switch to transmit state
    file_extension_bytes = bytearray()  # file extension being sent over, used for decoding
    file_extension = []

    # configuring our serial
    ser = serial.Serial()
    ser.port = sys.argv[1]
    ser.baudrate = sys.argv[2]
    ser.open()
    
    print(sys.argv) # for debugging communication
    
    # Translation:
    #   1) .split('.')[1] means take everything after the . of our file path
    #   2) ord converts characters to bytes
    file_extension.extend(map(ord, sys.argv[3].split('.')[1]))

    # Fill in unused chars with spaces (as bytes), ensuring that our file extension to
    # be sent over has EXTENSION_LEN bytes total
    space_pad = [ord(" ") for _ in range(EXTENSION_LEN - len(file_extension))]
    file_extension.extend(space_pad)

    # populating our byte arrays
    file_extension_bytes.extend(file_extension)
    raw_hex_bytes.extend(map(ord, sys.argv[4]))
    transmit_byte.extend([TRANSMISSION_BIT])

    # first we tell the Arduino to switch to transmit state
    ser.write(transmit_byte)

    # then we wait until we get confirmation from the Arduino that it is transmitting
    while True:
        config_string = ""
        while ser.in_waiting:
            config_string += ser.read(1).decode("utf-8")
            if config_string == CONFIRMATION_CHAR:
                break

        if config_string == CONFIRMATION_CHAR:
            break

    # now we are ready to send over the extension and its contents one byte at a time
    ser.write(file_extension_bytes)
    # ser.write(raw_hex_bytes)
    
    ser.close()
