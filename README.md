# RFSling
File transfers between computers via Arduinos and RF signals.

## Table of Contents

<!--ts-->
- [Intro](#Intro)
- [Schedule](#Schedule)
- [Methods](#Methods)
  * [Setup/BOM](#Setup-and-BOM)
  * [Communication Chain](#Communication-Chain)
  * [Interface](#Interface)
  * [Computer and Feather](#Computer-and-Feather)
  * [Feather and Transceiver](#Feather-and-Transceiver)
  * [Transceiver and Transceiver](#Transceiver-and-Transceiver)
- [Results](#Results)
- [Issues](#Issues)
- [Future Work](#Future-Work)
- [References](#References)
<!--te-->

## Intro
During this project we decided to create two systems that would communicate with each other remotely, without making use of pre-existing mediums such as Wifi. Our reasoning for this is based on privacy concerns. It is no secret that ISPs and websites can record and maintain our meta data ruthlessly. We believed that our project would attempt to solve that problem. 

The type of data that we decided to communicate was plain-text files; however, when we designed the system we left ourselves enough flexibility to extend this to other data types. For example, we could theoretically send over images but we would need to worry more about how we read the bytes from the image.

We felt that this project was appropriate because our class placed a huge emphasis on the communication between different devices.

## Schedule
We decided to perfer an Agile-like schedule for development.  We used Github's "Projects" feature to store our user stories, implemented as "Issues."  Because of the time constraint, we only completed a single sprint and all user stories were included in the To-Do pile.  Our schedule was as follows:

### Week 1: Project Definition
* Discussed requirements
* Explored hardware and their associated data sheets needed to satisfy requirements
* Populated backlog on Github with user stories

### Week 2 - 3: Sprint 1/1
* Grabbed items from the backlog and marked them as being currently worked on (so no two people accidently did the same work)
* Branched, commited often, and requested to pull
* Reviewed code
* Performed quasi-daily stand-ups

We completed all user stories except for 3: "Checksum" (to check for data loss in transmission), "As Transceiver" (Feathers and switch between RX and TX without reuploading code), and "Encryption" (for added security in transmission)

Below is a more detailed outline of the milestones that we hit over the last few weeks.
![RFSling Development Schedule](final_scheudle.png)

## Methods
An important topic in embedded systems is communicating between devices and peripherals.  In our project, we were faced with the daunting need to communicating not only between devices and peripherals, but also between devices.

### Setup and BOM
ANTHO

### Communication Chain
As per our architecture, we needed to communicate between 6 devices peripherals at once: two computers, two feathers, and two transceivers.  We also needed a way for users to interface with our entire systems.  Thus, our project could be can be broken down into 4 distinct parts of the communication chain (from one computer to the next) as follows:

1)	User and Computer (Interface)
2)	Computer and Feather
3)	Feather and Transceiver
4)	Transceiver and Transceiver

Since we are communicating between devices, we had separate code bases uploaded on computers and feathers (and between feathers as is described below).  Thus, there was a need to synchronize states between devices for successful communication.  Specifically, we needed to ensure that state was a predictable variable, that devices only transferred information between each other when both were ready.  We will go into detail on how we accomplished this for most of our four parts of the communication chain (parts 1-3).

Since our peripherals were transceivers, ideally, each Feather would be able to both send and receive files, with an identical build uploaded to both devices.  As will be described in the issues section, mainly due to time constraints, this was not feasible.  Therefore, we needed to create separate builds, one for RX one for TX, to be uploaded separately to each board.  Unfortunately, this made it so that each board could only function as a receiver or a transmitter at a given time.  Thus, in the following subsections we will describe at a high level how each communication part was implemented both from an RX and a TX standpoint. 

### Interface
On both the RX and TX end, users must run a shell script (rx_interface.sh and tx_interface.sh) in their terminal to pull up the communication interface.  Users on the RX end must configure their Feathers first so that they can be listening before users on the TX end send any data.  Failure to do so will result in data loss because TX users will transmit data without knowing that there is no one on the other side receiving.  When both scripts are run, users will be prompted to input their Feather device into a free USB slot so that the script can recognize the device port to communicate with over serial.  This is accomplished using a diff on the /dev/ directory before and after devices are inserted.  After which point, the device path will be passed to a python script to handle future configuration.  Before this script is run on the TX end, however, users will be prompted to input the full path to the file they wish to transmit.  The shell script then checks that the file exists before passing the file path, in addition to the device path, to the python script. 

In the python script, users are first prompted to input their desired channel (operating frequency) and address (identifier at specific channel).  These channels and addresses must be mirrored between RX and TX users for successful communication.  Once this information is input, the python script will send over the configuration to the Feather (described in the next section) and transmission or receiving/listening will begin.  Once transmission is complete, on both the RX and TX ends, users will be notified and given the option to either quit or send/receive another file.  This is accomplished via a while loop within each of the scripts that calls our python script until some stop sequence of characters is input by the user.


### Computer and Feather
ELIJAH

### Feather and Transceiver
Communicaiton between the feather and the transceiver is done over SPI. When looking over the data sheet (see at end of document) we identified the relevant pieces of this communication and encapsulated them via the nRF24L01 library. 

We focused on abstracting the common SPI commands (pg 48 of the data sheet) and manipulation of the register map (pg 54 of the data sheet). This meant that writing data to registers or writing a payload to the TX FIFO, for example, was done throught a simple call to an nRF24 method. 

The data sheet also provided a state diagram (see section 6) that helped us solidify our sense of timings and what methods we should include to ensure that the module was in a state that we expected.

Testing of this funcitonality consisted of writing to registers then reading back the values in those registers. We were able to implement the majority of the functionality offered by the module in transmission mode. However, we could not get the module to actually transmit data. This will be discussed in the issues section.

### Transceiver and Transceiver
ANTHO

## Results
ANTHO

## Issues

Intro to issues... ANTHO?

### Syncronizing Communication ELIJAH
See [Computer and Feather](#Computer-and-Feather).

### Interfacing with the nRF24L01+ chip ANTHO
ANTHO

### As Transeiver
ELIJAH

### Data Loss
ANTHO


GET RID OF RAW HEXXXXXXXX

## Future Work
To continue to improve upon the project we would get the Feathers to successfully function as transceivers without the need to re-upload code; complete our own, already-initiated version of the nRF24L01+ driver; and complete the remaining checksum and encryption user stores.  

Once these loose ends of the project are finished, we would generalize the code to work on boards other than the Feather (since how the CE and CSN pins are hard wired only for the feather), and we would like to make hardware improvements to increase the range of communication and hopefully lessen data loss.  Specifically, we would be interested in attaching an amplifier to further signal output.

## References
### Data Sheets:
* https://www.sparkfun.com/datasheets/Components/SMD/nRF24L01Pluss_Preliminary_Product_Specification_v1_0.pdf
* https://usermanual.wiki/Document/esp32technicalreferencemanualen.1071254929/help

### Libraries:
* https://github.com/nRF24/RF24

