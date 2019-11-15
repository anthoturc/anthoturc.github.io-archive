#!/bin/bash

DEVICE_PATH="/dev/"

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
printf "\nConnected! $DEVICE_PATH$DEV_PORT\n-----------\n"
rm tmp_curr_usb.txt tmp_prev_usb.txt

python3 interface.py $DEVICE_PATH$DEV_PORT
