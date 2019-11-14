#!/bin/bash

MAX_CHANNEL=125
MIN_CHANNEL=0
CHANNEL=127

printf "Let\'s get started.\n\n\nWhat channel are we on? (0-125): "
while [ $CHANNEL -lt $MIN_CHANNEL -o $CHANNEL -gt $MAX_CHANNEL ]; do
	read CHANNEL
	if  [ $CHANNEL -lt $MIN_CHANNEL -o $CHANNEL -gt $MAX_CHANNEL ]
		then printf "Invalid input. What channel are we on? (0-125): "
	fi
done
echo $CHANNEL
