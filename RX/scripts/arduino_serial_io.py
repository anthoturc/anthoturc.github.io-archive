#!/bin/python3
"""
Module contains all the functions and constants neccessary for serial communication
with an Arduino to perform nRF24L01+ RF communication.
"""

# We are going to store the configured integers in our Arduino in a Union,
# by sending over our individal bytes and storing them in memory.
# Thus, we need to consider the endianess of our Arduino
ENDIANESS = 'little'

# RF frequency channel bounds
MAX_CHANNEL = 125
MIN_CHANNEL = 0

# Data pipe address bounds
MAX_ADDRESS = 10000
MIN_ADDRESS = 0

# used to communicate state changes between the Arduino and Computer
HANDSHAKE_CHAR = '\t'

END_CHAR = '}'

TX_CHAR = '~'

# Max amount of time to wait duing our handshake before we throw and exception
MAX_HANDSHAKE_SEC = 10

# tell Arduino we are ready to send data
HANDSHAKE_BYTE = bytearray()
HANDSHAKE_BYTE.extend([ord(HANDSHAKE_CHAR)])

# tell Arduino to switch to transmit state
TX_BYTE = bytearray()
TX_BYTE.extend([ord(TX_CHAR)])

# How many consecutive signals we need to send over
HANDSHAKE_REPS = 5

FLUSH_SIGNAL = bytearray()
FLUSH_SIGNAL.extend([ord(HANDSHAKE_CHAR) for _ in range(HANDSHAKE_REPS)])

# size of char array in union representing extension on Arduino
EXTENSION_LEN = 32

# Max bytes to send over to the Arduino at one time. We send 224 bytes because it is
# a multiple of 32, the size of our our FIFO buffer on the nRF24L01+ chip, and it still
# fits within our serial's buffer, which has a capacity of 224 hex chars.
MAX_HEX_CHUNK_BYTES = 224

# All numbers 0-9 as strings, used for input error handling
NUMBERS = {str(x) for x in range(10)}


def flushSerial(ser):
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
    ser.write(FLUSH_SIGNAL)


def getIntInput():
    """
    Returns -1 if our input is not an integer, and the input if it is an integer.

    Params:
        None

    Outputs:
        int
    """
    int_input = input()
    if not int_input or not all([i in NUMBERS for i in int_input]):
        int_input = -1

    return int(int_input)


def handshake(ser):
    """
    Sends to the Arduino HANDSHAKE_BYTE, telling it we are about to send over data.
    Then, we wait for a confimation from Arduino that it is ready via HANDSHAKE_CHAR
    at which point we break, signifying a successful handshake.  This funciton relies
    on handshake function included in the serial_io class on the Arduino's side of
    communication.

    We throw an error if handshake takes more than MAX_HANDSHAKE_SEC

    Params:
        ser:
            Our initiallized pyserial serial port

    Outputs:
        None
    """
    ser.write(HANDSHAKE_BYTE)
    curr = ""

   # We need two while loops because we need to remain in our
   # hanshake state until the Arduino sends over data, but that
   # may not happen instantly.  We also want to break out of
   # the innermost loop the second that we have the confirmation
   # of our handshake from the Arduino's side
    while curr != HANDSHAKE_CHAR:
        while curr != HANDSHAKE_CHAR and ser.in_waiting:
            curr = ser.read(1).decode("utf-8")


def chunkGenerator(data):
    """
    Generator to split data into chunks to be sent over serial.  We cannot fit
    all the data on the Arduino at once, which is why we have to split it up
    like this.  Data is read over serial on the Arduino's side and fills the
    file_chunk buffer via the setFromSerial function (both within serial_io).

    Params:
        data:
            string: hex data to be sent to Arduino

    Yields:
        string: next chunk of data with size <= MAX_HEX_CHUNK_BYTES
    """
    i = -MAX_HEX_CHUNK_BYTES
    j = 0
    for _ in range(len(data) // MAX_HEX_CHUNK_BYTES):
        i += MAX_HEX_CHUNK_BYTES
        j += MAX_HEX_CHUNK_BYTES
        yield data[i:j]

    yield data[i + MAX_HEX_CHUNK_BYTES:len(data)]


def enableTX(ser):
    """
    Signals to the Arduino to enable TX mode of the nRF chip.

    Params:
        ser:
            Our initiallized pyserial serial port

    Outputs:
        None
    """
    ser.write(TX_BYTE)


def getData(ser):
    """
    Gets data sent to the computer from the Arduino over seral.
    Transmissions are seperated by the HANDSHAKE_CHAR.

    Params:
        ser:
            Our initiallized pyserial serial port

    Outputs:
        string: our data
    """
    data = ""
    curr = ""
    while curr != HANDSHAKE_CHAR:
        while curr != HANDSHAKE_CHAR and ser.in_waiting:
            data += curr
            curr = ser.read(1).decode("utf-8")

    return data


def setConfig():
    """
    Uses user input to configure the channel and address parameters for 
    communicaiton with Arduino

    Outputs:
        tuple: channel, address
            channel:
                int between 0 and 125
            
            address:
                int between 0 and 10000
    """
    # init pipe and channel to out of bounds
    channel = -1
    address = -1

    print("\nEnter channel ({0}-{1}): ".format(MIN_CHANNEL, MAX_CHANNEL))
    while not MIN_CHANNEL <= channel <= MAX_CHANNEL:
        channel = getIntInput()
        if not MIN_CHANNEL <= channel <= MAX_CHANNEL:
            print("Invalid input. Re-enter channel ({0}-{1}): ".format(MIN_CHANNEL, MAX_CHANNEL))

    print("\nEnter address ({0}-{1}): ".format(MIN_ADDRESS, MAX_ADDRESS))
    while not MIN_ADDRESS <= address <= MAX_ADDRESS:
        address = getIntInput()
        if not MIN_ADDRESS <= address <= MAX_ADDRESS:
            print("Invalid input. Re-enter address ({0}-{1}): ".format(MIN_ADDRESS, MAX_ADDRESS))

    channel = channel.to_bytes(1, byteorder=ENDIANESS)
    address = address.to_bytes(4, byteorder=ENDIANESS)

    return channel, address
