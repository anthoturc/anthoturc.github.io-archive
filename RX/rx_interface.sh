#!/bin/bash

DEVICE_PATH="/dev/"
BAUD_RATE=115200
LOG_PATH="./logs/"
RX_FILE_PATH="./rx-files/"
# DEVICE_PATH="/dev/cu.SLAB_USBtoUART"

if [[ ! -e "$LOG_PATH" ]]; then
	mkdir $LOG_PATH
fi

if [[ ! -e "$RX_FILE_PATH" ]]; then
	mkdir $RX_FILE_PATH
fi

# store curr connected devices:
ls $DEVICE_PATH > /tmp/prev_usb.txt
cat /tmp/prev_usb.txt > /tmp/curr_usb.txt  # init device lists to be the same

printf "\n-----------\nConnect your Arduino file transfer device..."

# check to see if a device has been added
while [ "$( cat /tmp/prev_usb.txt )" == "$( cat /tmp/curr_usb.txt )" ]; do
	ls $DEVICE_PATH > /tmp/curr_usb.txt
	sleep 1
done

# find the difference between the two device printouts and search for relevant devices
DEV_PORT=$( comm -23 /tmp/curr_usb.txt /tmp/prev_usb.txt | grep -E 'ttyU|cu.S' ) # ttyU for linux, cu.S for mac
printf "\nConnected! $DEVICE_PATH$DEV_PORT\n-----------\n"
rm /tmp/curr_usb.txt /tmp/prev_usb.txt

# we use the while loop to allow users to transmit more than once
stop=""
while [ "$stop" != "stop" ]; do
	printf "\nCurrently Receiving..."
	python3 ./scripts/receive_hex.py $DEVICE_PATH$DEV_PORT $BAUD_RATE $file_path "$raw_hex" -W 2> ./logs/error-log-rx.txt
	printf "File Recieved!  Type 'stop' to stop receiving, else hit enter?\n"
	read stop
done
printf "\nGoodbye!\n"
