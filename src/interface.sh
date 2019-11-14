#!/bin/bash

DAVICE_PATH="/dev/"

# RF frequency channel bounds
MAX_CHANNEL=125
MIN_CHANNEL=0

# Data pipe address bounds
MAX_ADDRESS=16777215
MIN_ADDRESS=0

# init pipe and channel to out of bounds
CHANNEL=-1
ADDRESS=-1

# store curr connected devices:
ls $DAVICE_PATH > tmp_prev_usb.txt
cat tmp_prev_usb.txt > tmp_curr_usb.txt  # init device lists to be the same

printf "Connect your Arduino file transfer device..."

# check to see if a device has been added
while [ "$( cat tmp_prev_usb.txt )" == "$( cat tmp_curr_usb.txt )" ]; do
	ls $DAVICE_PATH > tmp_curr_usb.txt
	sleep 1
done


DEV_PORT=$( comm -23 tmp_curr_usb.txt tmp_prev_usb.txt | grep -E 'tty|cu' ) # tty for linux, cu for mac
printf "\nConnected! $DAVICE_PATH$DEV_PORT"
rm tmp_curr_usb.txt tmp_prev_usb.txt

printf "\n\nLet\'s get started.\n\n\nWhat channel are we on? (0-125): "
while [ $CHANNEL -lt $MIN_CHANNEL -o $CHANNEL -gt $MAX_CHANNEL ]; do
	read CHANNEL
	if  [ $CHANNEL -lt $MIN_CHANNEL -o $CHANNEL -gt $MAX_CHANNEL ]
		then printf "Invalid input. What channel are we on? (0-125): "
	fi
done

printf "\nWhat is your address? (0-16,777,215) "
while [ $ADDRESS -lt $MIN_ADDRESS -o $ADDRESS -gt $MAX_ADDRESS ]; do
	read ADDRESS
	if  [ $ADDRESS -lt $MIN_ADDRESS -o $ADDRESS -gt $MAX_ADDRESS ]
		then printf "Invalid input. What is your address? (0-16,777,215): "
	fi
done

echo $CHANNEL
echo $ADDRESS
