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
    file_extension_bytes:
        File extension of our sent file

    raw_hex_bytes:
        Raw-hex of our file

"""
import sys
import serial
from arduino_serial_io import EXTENSION_LEN, ENDIANESS, handshake, chunkGenerator, printData

# used to debug communication
DEBUG = 1


if __name__ == "__main__":
    raw_hex_bytes = bytearray()  # our file to be sent
    file_extension_bytes = bytearray()  # file extension being sent over, used for decoding
    file_extension = []

    # configuring our serial
    ser = serial.Serial()
    ser.port = sys.argv[1]
    ser.baudrate = int(sys.argv[2])
    ser.open()

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

    # initialte communication with the Arduino
    handshake(ser)

    # send over the extension and its contents one byte at a time
    ser.write(file_extension_bytes)

    # precaution to make sure we are only sending file data over Serial
    chunks = chunkGenerator(raw_hex_bytes)
    for c in chunks:

        # Shake between every transaction to make sure that the Arduino
        # is ready for our next chunk of data
        handshake(ser)
        total = len(c)
        total = total.to_bytes(1, byteorder=ENDIANESS)
        ser.write(total)
        ser.write(c)

        if DEBUG:
            printData(ser, '') # output our received file

    ser.close()
