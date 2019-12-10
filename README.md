# RFSling
File transfers between computers via Arduinos and RF signals.

Demo: 
[![Alt text](https://img.youtube.com/vi/QZxoFUmi7cs/0.jpg)](https://www.youtube.com/watch?v=QZxoFUmi7cs)

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
During this project we decided to create two systems that would communicate with each other remotely, without making use of pre-existing mediums such as Wi-Fi. Our reasoning for this is based on privacy concerns. It is no secret that ISPs and websites can record and maintain our meta data ruthlessly. We believed that our project would attempt to solve that problem. 

The type of data that we decided to communicate was plain-text files; however, when we designed the system, we left ourselves enough flexibility to extend this to other data types. For example, we could theoretically send over images, but we would need to worry more about how we read the bytes from the image.

We felt that this project was appropriate because our class placed a huge emphasis on the communication between different devices.

## Schedule
We decided to prefer an Agile-like schedule for development.  We used Github's "Projects" feature to store our user stories, implemented as "Issues."  Because of the time constraint, we only completed a single sprint and all user stories were included in the To-Do pile.  Our schedule was as follows:

### Week 1: Project Definition
* Discussed requirements
* Explored hardware and their associated data sheets needed to satisfy requirements
* Populated backlog on Github with user stories

### Week 2 - 3: Sprint 1/1
* Grabbed items from the backlog and marked them as being currently worked on (so no two people accidentally did the same work)
* Branched, committed often, and requested to pull
* Reviewed code
* Performed quasi-daily stand-ups

We completed all user stories except for 3: "Checksum" (to check for data loss in transmission), "As Transceiver" (Feathers and switch between RX and TX without re-uploading code), and "Encryption" (for added security in transmission)

Below is a more detailed outline of the milestones that we hit over the last few weeks.
![RFSling Development Schedule](https://github.com/anthoturc/anthoturc.github.io/blob/master/final_schedule.png)

## Methods
An important topic in embedded systems is communicating between devices and peripherals.  In our project, we were faced with the daunting need to communicating not only between devices and peripherals, but also between devices.

### Setup and BOM
Our setup (per system) consisted of the following:
* 10pcs Arduino nRF24L01+ 2.4 GHz Wireless RF Transceiver Module (11.98 USD) 
* Adafruit HUZZAH32 - ESP32 Feather Board (19.95 USD)
* Premium Female/Male 'Extension' Jumper Wires (1.95 USD)
* USB cable - USB A to Micro USB-B (2.95 USD)
* PC (prices vary)

Total: >= 24.85 USD

These are the required parts for just one system. Another system would be twice this amount. 

The setup is shown below:
![](https://github.com/anthoturc/anthoturc.github.io/blob/master/setup.png)

### Communication Chain
As per our architecture, we needed to communicate between 6 devices peripherals at once: two computers, two feathers, and two transceivers.  We also needed a way for users to interface with our entire systems.  Thus, our project could be can be broken down into 4 distinct parts of the communication chain (from one computer to the next) as follows:

1)  User and Computer (Interface)
2)  Computer and Feather
3)  Feather and Transceiver
4)  Transceiver and Transceiver

Since we are communicating between devices, we had separate code bases uploaded on computers and feathers (and between feathers as is described below).  Thus, there was a need to synchronize states between devices for successful communication.  Specifically, we needed to ensure that state was a predictable variable, that devices only transferred information between each other when both were ready.  We will go into detail on how we accomplished this for most of our four parts of the communication chain (parts 1-3).

Since our peripherals were transceivers, ideally, each Feather would be able to both send and receive files, with an identical build uploaded to both devices.  As will be described in the issues section, mainly due to time constraints, this was not feasible.  Therefore, we needed to create separate builds, one for RX one for TX, to be uploaded separately to each board.  Unfortunately, this made it so that each board could only function as a receiver or a transmitter at a given time.  Thus, in the following subsections we will describe at a high level how each communication part was implemented both from an RX and a TX standpoint. 

### Interface
On both the RX and TX end, users must run a shell script (rx_interface.sh and tx_interface.sh) in their terminal to pull up the communication interface.  Users on the RX end must configure their Feathers first so that they can be listening before users on the TX end send any data.  Failure to do so will result in data loss because TX users will transmit data without knowing that there is no one on the other side receiving.  When both scripts are run, users will be prompted to input their Feather device into a free USB slot so that the script can recognize the device port to communicate with over serial.  This is accomplished using a diff on the /dev/ directory before and after devices are inserted.  After which point, the device path will be passed to a python script to handle future configuration.  Before this script is run on the TX end, however, users will be prompted to input the full path to the file they wish to transmit.  The shell script then checks that the file exists before passing the file path, in addition to the device path, to the python script. 

In the python script, users are first prompted to input their desired channel (operating frequency) and address (identifier at specific channel).  These channels and addresses must be mirrored between RX and TX users for successful communication.  Once this information is input, the python script will send over the configuration to the Feather (described in the next section) and transmission or receiving/listening will begin.  Once transmission is complete, on both the RX and TX ends, users will be notified and given the option to either quit or send/receive another file.  This is accomplished via a while loop within each of the scripts that calls our python script until some stop sequence of characters is input by the user.

### Computer and Feather
Communication between the computer and feather was done over Serial (UART under the hood).  As explained in the [interface](#Interface) section, once the user has fully configured their communication setup, they will be performing computation from one of two Python scripts, receive_hex.py and send_hex.py for RX and TX communication respectively.  Most important to note, since the computer and the board are running separate code bases at the same time, which rely on the output of one another, we must synchronize their communication.  That is, we must make sure that each device can predict the state of the other device at any given time.  This ensures that we only send data between the two devices when each is fully ready.  Thus, to synchronize communication, we introduce the idea of a 'handshake.'  Basically, the idea of a handshake is that every time device A sends data to device B, which must be processed by device B before it can continue communication, device A will remain in its prior state, stuck in a while loop, until device B indicates to device A that it is ready to continue.  The way this works is device A will send a predefined byte value over serial, our HANDSHAKE_CHAR, to indicate to device B that it is waiting-- in our code this HANDSHAKE_CHAR is configured to '\t'.  Device A will then enter a while loop that is only broken when device B sends a HANDSHAKE_CHAR.  At which point the two devices and perform their next communication.

We will now explain how the communication works on a high level on both the TX and RX ends of file transferring.  First, we will go over how the channel and address are sent, because this is the same for both TX and RX:

Once the user has input channel and address from the interface, the computer will first send a series of HANDSHAKE_CHAR's, followed by the channel and address.  We will then send over a HANDSHAKE_CHAR to indicate the computer is waiting for the Feather to configure itself before continuing communication.  The reason we must first send over a series of HANDSHAKE_CHAR's is because at startup, once the board is plugged into the computer, there will be unwanted data sent over serial, which we do not wish to interoperate as configuration data for our Feather.  Thus, our Feather will enter a serial flushing state at startup until it receives a series of HANDSHAKE_CHAR's, at which point it breaks and enters a reading state, ready to store the following sent over channel and address variables.  

Since the channel and address are of fixed length (channel cannot be more than one byte and address cannot be more than 4 bytes), we program the Feather to store the first byte sent over as the channel, and the next four as the address.  The Feather will then enter its own handshake state, where is will read the HANDSHAKE_CHAR sent by the computer, and respond by sending its own HANDSHAKE_CHAR to the computer.  The Feather and computer then both break their handshake states and both continue with their code.

#### TX
After configuration, the computer will send over the file extension of our file, which we fix as a 32-byte value-- which is convenient because our transceiver's FIFO can store 32 bytes-- and we pad unused bytes with space characters.  The Feather is programmed to store these next 32 bytes, our extension, as the file extension, to be sent over radio.

Once the extension has been sent, we send the file over serial in chunks of 224 bytes at a time, the multiple of 32 nearest to and less than size of serial's buffer (this again is useful for FIFO loading), and handshake with the Feather between transmissions.  Note that the final chunk may be of length less than 224 for our final chunk, thus we add an additional byte in each transmission, the first byte of each chunk, to help the Feather send data into our transceiver's FIFOs.

On the Feather's side, with each data chunk we load data into the FIFOs, 32 bytes at a time, and once we have sent all current chunk data over radio, we shake hands with the computer to indicate we are ready for the next chunk.

#### RX
After configuration, the Arduino will enter a while loop, continuously reading from the transceiver's FIFO, handshaking, then sending data over to the computer until it reads a special END_CHAR sent over FIFO at which point it will send the END_CHAR to the computer to signify the end of transmission. The computer has mirrored functionality, first storing the file extension, then storing the sent over chunks of the file while intermittently handshaking.  Once the computer reads the END_CHAR, it saves the compiled file with sent over file extension.

### Feather and Transceiver
Communication between the feather and the transceiver is done over SPI. When looking over the data sheet (see at end of document) we identified the relevant pieces of this communication and encapsulated them via the nRF24L01 library. 

We focused on abstracting the common SPI commands (pg 48 of the data sheet) and manipulation of the register map (pg. 54 of the data sheet). This meant that writing data to registers or writing a payload to the TX FIFO, for example, was done through a simple call to an nRF24 method. 

The data sheet also provided a state diagram (see section 6) that helped us solidify our sense of timings and what methods we should include to ensure that the module was in a state that we expected.

Testing of this functionality consisted of writing to registers, then reading back the values in those registers. We were able to implement the majority of the functionality offered by the module in transmission mode. However, we could not get the module to actually transmit data. This will be discussed in the issues section.

### Transceiver and Transceiver
This portion of the communication is out of the scope of this course. While it is interesting to know how the modules are able to communicate over RF, our project and research is more closely related to what we discussed over the course of the semester.

## Results
We believe we were successful in creating systems that communicated data remotely. The demo of our project is below.
[Demo](https://www.youtube.com/watch?v=QZxoFUmi7cs)

## Issues

During the project, we faced 4 principal issues, included below:

### Synchronizing Communication
See [Computer and Feather](#Computer-and-Feather).

### Interfacing with the nRF24L01+ chip
ANTHO

### As Transceiver
Ideally, we would have the same code uploaded to both boards involved in communication and ideally both devices would be able to receive and send files.  When we were initially planning our project, we envisioned the devices functioning as receivers by default and only transmitting when users indicated the wished to do so, similar to a walkie talkie.  To accomplish this, we started by exploring at_cmd UART interrupts, where an interrupt would be triggered on the Feather when a pre-configured at_cmd char is observed a certain number of times in a row without too much time in between at_cmd char transmissions and with enough time in between prior and post communication after the at_cmd sequence is sent.  Reading the data sheet, we were able to identify the registers responsible for configuring, enabling, and clearing the interrupt, which we included in esp32AtCmdUART.h.  We looked into the source code to try to find their implementation of esp_intr_alloc, used to attach ISR's to specific interrupt events, and we initially could not find its implementation, only its header file.  We cynically thought that esp was forcing us to use their framework (esp-idf) over the Arduino framework to do so.  Looking back over their codebase, however, we realized that we were just looking in the wrong place and implementations are, indeed, included.  Regardless, in comical fashion, we realized that interrupts were not the best tool to use here because we would not want to grant the user the ability to stop functioning as a receiver while currently receiving a file.  Thus, ideally, we decided to poll the at_cmd UART interrupt status in our main loop to determine whether the device would function as a receiver or transmitter.  On a high level, the loop within main looked something like this:

```
loop:
    if flag not raised:
        RX;
    if raised:
        TX; 
        clear flag;

```

Since serial communication is actually UART under the hood, we were able to successfully trigger and clear at_cmd interrupts from the computer over serial using python the command line.  Thus, the Feather-side PoC is complete.  Due to time constraints, however, we were unable to restructure our Python scripts and shell script interfaces to handle dual TX RX functionality.  We do believe, however, that it would be feasible, given enough time.


### Data Loss
This is not entirely obvious from the demo, but we do experience data loss during transmission. We believe that this has to do with our configuration of the module itself. The library we used does offer access to more advanced capabilities, such as retransmission of packets, but we did not explore those options very much.

## Future Work
To continue to improve upon the project we would get the Feathers to successfully function as transceivers without the need to re-upload code; complete our own, already-initiated version of the nRF24L01+ driver; and complete the remaining checksum and encryption user stores.  

Once these loose ends of the project are finished, we would generalize the code to work on boards other than the Feather (since how the CE and CSN pins are hard wired only for the feather), and we would like to make hardware improvements to increase the range of communication and hopefully lessen data loss.  Specifically, we would be interested in attaching an amplifier to further signal output.

## References
### Data Sheets:
* https://www.sparkfun.com/datasheets/Components/SMD/nRF24L01Pluss_Preliminary_Product_Specification_v1_0.pdf
* https://usermanual.wiki/Document/esp32technicalreferencemanualen.1071254929/help

### Libraries:
* https://github.com/nRF24/RF24
