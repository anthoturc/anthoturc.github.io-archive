#!/bin/bash

DEVICE_PATH="/dev/"

# RF frequency channel bounds
MAX_CHANNEL=125
MIN_CHANNEL=0

# Data pipe address bounds
MAX_ADDRESS=16777215
MIN_ADDRESS=0

# init pipe and channel to out of bounds
CHANNEL=-1
ADDRESS=-1

# BAUD for serial communication
BAUD_RATE=115200

# store curr connected devices:
ls $DEVICE_PATH > tmp_prev_usb.txt
cat tmp_prev_usb.txt > tmp_curr_usb.txt  # init device lists to be the same

printf "Connect your Arduino file transfer device..."

# check to see if a device has been added
while [ "$( cat tmp_prev_usb.txt )" == "$( cat tmp_curr_usb.txt )" ]; do
	ls $DEVICE_PATH > tmp_curr_usb.txt
	sleep 1
done

# find the difference between the two device printouts and search for relevant devices
DEV_PORT=$( comm -23 tmp_curr_usb.txt tmp_prev_usb.txt | grep -E 'tty|cu' ) # tty for linux, cu for mac
printf "\nConnected! $DEVICE_PATH$DEV_PORT"
rm tmp_curr_usb.txt tmp_prev_usb.txt

# finding channel
printf "\nLet\'s get started.\n\n\nWhat channel are we on? (0-125): "
while [ $CHANNEL -lt $MIN_CHANNEL -o $CHANNEL -gt $MAX_CHANNEL ]; do
	read CHANNEL
	if  [ $CHANNEL -lt $MIN_CHANNEL -o $CHANNEL -gt $MAX_CHANNEL ]
		then printf "Invalid input. What channel are we on? (0-125): "
	fi
done

# finding address
printf "What is your address? (0-16,777,215) "
while [ $ADDRESS -lt $MIN_ADDRESS -o $ADDRESS -gt $MAX_ADDRESS ]; do
	read ADDRESS
	if  [ $ADDRESS -lt $MIN_ADDRESS -o $ADDRESS -gt $MAX_ADDRESS ]
		then printf "Invalid input. What is your address? (0-16,777,215): "
	fi
done

# Sending data to Arduino
# TODO: I have no clue why we need to send then reset, but we do...
printf "0\n" > $DEVICE_PATH$DEV_PORT
printf "0\n" > $DEVICE_PATH$DEV_PORT

printf "Please reset your board in the next 10 seconds:\n"
sleep 1
echo 10
sleep 1
echo 9
sleep 1
echo 8
sleep 1
echo 7
sleep 1
echo 6
sleep 1
echo 5
sleep 1
echo 4
sleep 1
echo 3
sleep 1
echo 2
sleep 1
echo 1

# we have to reverse the address and channel for readInput()
# to understand why, see comment in readInput()
printf "$CHANNEL\n" | rev > $DEVICE_PATH$DEV_PORT
printf "$ADDRESS\n" | rev > $DEVICE_PATH$DEV_PORT
