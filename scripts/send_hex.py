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


def handshake(ser):
    """
    Sends to the Arduino HANDSHAKE_BYTE, telling it we are about to send over data.
    Then, we wait for a confimation from Arduino that it is ready via HANDSHAKE_CHAR
    at which point we break, signifying a successful handshake.

    We throw an error if handshake takes more than MAX_HANDSHAKE_SEC

    Params:
        ser:
            Our initiallized pyserial serial port

    Outputs:
        None
    """
    ser.write(HANDSHAKE_BYTE)
    config_string = ""

    start_time = time.time()
    time.sleep(1)
    # wait until we get confirmation from the Arduino
    while True:
        while ser.in_waiting:
            config_string = ser.read(1).decode("utf-8")            
            if (time.time() - start_time) > MAX_HANDSHAKE_SEC:
                raise Exception('Handshake time limit of {0} sec exceeded'.format(MAX_HANDSHAKE_SEC))
            
            if config_string == HANDSHAKE_CHAR:
                break

        if config_string and config_string == HANDSHAKE_CHAR:
            break
        
        
 

if __name__ == "__main__":
    
    # size of char array in union representing extension on Arduino
    EXTENSION_LEN = 10

    # TODO: send over size of hex as checksum if have time
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

    # for debugging communication
    ext = ""
    time.sleep(1)
    while True:
        while ser.in_waiting:
            curr = ser.read(1).decode("utf-8")
            
            if curr ==  HANDSHAKE_CHAR:
                print(ext)
                if ext[0] not in set([str(x) for x in range(10)]):
                    break            
                ext = ""
            else:
                ext += curr
        
        if ext[0] not in set([str(x) for x in range(10)]):
            break            

    time.sleep(1)
    handshake(ser)
    
    # send 80000 bytes at a time = 80000 transmissions
    
    print(len(raw_hex_bytes))
    # ser.write(raw_hex_bytes)
    
    ser.close()
