#!/bin/bash

DEVICE_PATH="/dev/"
BAUD_RATE=115200
LOG_PATH="./logs/"
DEVICE_PATH="/dev/cu.SLAB_USBtoUART"

if [[ ! -e "$LOG_PATH" ]]; then
    mkdir $LOG_PATH
fi

# # store curr connected devices:
# ls $DEVICE_PATH > /tmp/prev_usb.txt
# cat /tmp/prev_usb.txt > /tmp/curr_usb.txt  # init device lists to be the same

# printf "\n-----------\nConnect your Arduino file transfer device..."

# # check to see if a device has been added
# while [ "$( cat /tmp/prev_usb.txt )" == "$( cat /tmp/curr_usb.txt )" ]; do
# 	ls $DEVICE_PATH > /tmp/curr_usb.txt
# 	sleep 1
# done

# # find the difference between the two device printouts and search for relevant devices
# DEV_PORT=$( comm -23 /tmp/curr_usb.txt /tmp/prev_usb.txt | grep -E 'ttyU|cu.S' ) # ttyU for linux, cu.S for mac
# printf "\nConnected! $DEVICE_PATH$DEV_PORT\n-----------\n"
# rm /tmp/curr_usb.txt /tmp/prev_usb.txt

# we use the while loop to allow users to transmit more than once
file_path=""
while [ "$file_path" != "q" ]; do
	printf "\nCurrently Transmitting...\nEnter absolute file path to send file or q to quit: \n"
	read file_path

	if [[ -e "$file_path" ]] && [[ ! -d "$file_path" ]]; then

		# if our file exits, we convert it to raw hex and send
		raw_hex=$(xxd -p $file_path)
		if [ "$file_path" != "q" ]; then
			python3 ./scripts/send_hex.py $DEVICE_PATH$DEV_PORT $BAUD_RATE $file_path "$raw_hex" -W 2> ./logs/error-log-tx.txt
			printf "\nSent!\n"
		fi
	elif [ "$file_path" != "q" ]; then
		printf "File does not exist. Try again.\n"
	fi	
done
printf "\nGoodbye!\n"
