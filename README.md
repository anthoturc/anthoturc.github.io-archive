# rfsling
ES project for communication via rf signals

## Arduino Default Configuration Info
### CONFIG
EN_CRC = 1 to enable CRC
CRCO = 0 so CRC follows a 1-byte encoding scheme
MASK_MAX_RT = 1 to mask interrupt caused by max amount of repeaed transmits

### SETUP_AW
AW_1:0 = 01 sets the address widths to 3 for the data pipes 

### SETUP_RETR
ARD_3:0 = 1111 so autoretransmissions occur every 4000 microseconds
ARC_3:0 = 1111 so up to 15 retransmits allowed

### RF_CH
Conficured with the help of our shell script

### RF_SETUP
RF_DR_1:0 = 00 so air data rate = 1Mbps
RF_PWR_1:0 = 11 for max TX RF output power (0dBM)

