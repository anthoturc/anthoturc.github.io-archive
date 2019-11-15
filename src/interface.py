#!/bin/python3

import sys
import serial


# RF frequency channel bounds
MAX_CHANNEL=125
MIN_CHANNEL=0

# Data pipe address bounds
MAX_ADDRESS=16777215
MIN_ADDRESS=0

# BAUD for serial communication
BAUD_RATE=115200


if __name__ == "__main__":
    PORT = sys.argv[1]

    # init pipe and channel to out of bounds
    channel=-1
    address=-1

    print("Enter channel ({0}-{1}): ".format(MIN_CHANNEL, MAX_CHANNEL))
    while not (MIN_CHANNEL <= channel <= MAX_CHANNEL):
        channel = int(input())
        if not (MIN_CHANNEL <= channel <= MAX_CHANNEL):
            print("Invalid input. Re-enter channel ({0}-{1}): ".format(MIN_CHANNEL, MAX_CHANNEL))
    
    print("Enter channel ({0}-{1}): ".format(MIN_ADDRESS, MAX_ADDRESS))
    while not (MIN_ADDRESS <= address <= MAX_ADDRESS):
        address = int(input())
        if not (MIN_ADDRESS <= channel <= MAX_ADDRESS):
            print("Invalid input. Re-enter channel ({0}-{1}): ".format(MIN_ADDRESS, MAX_ADDRESS))
    
    
    print(channel)
    print(address)

    


# # Sending data to Arduino
# # TODO: I have no clue why we need to send then reset, but we do...
# printf "0\n" > $DEVICE_PATH$DEV_PORT
# printf "0\n" > $DEVICE_PATH$DEV_PORT

# printf "Please reset your board in the next 10 seconds:\n"
# sleep 1
# echo 10
# sleep 1
# echo 9
# sleep 1
# echo 8
# sleep 1
# echo 7
# sleep 1
# echo 6
# sleep 1
# echo 5
# sleep 1
# echo 4
# sleep 1
# echo 3
# sleep 1
# echo 2
# sleep 1
# echo 1

# # we have to reverse the address and channel for readInput()
# # to understand why, see comment in readInput()
# printf "$CHANNEL\n" | rev > $DEVICE_PATH$DEV_PORT
# printf "$ADDRESS\n" | rev > $DEVICE_PATH$DEV_PORT
