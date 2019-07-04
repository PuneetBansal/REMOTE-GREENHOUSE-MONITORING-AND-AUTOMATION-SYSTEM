# REMOTE GREENHOUSE MONITORING & AUTOMATION
                               Contributors : Puneet Bansal & Nachiket Kelkar
                               Professor    : Richard Heidebrecht


## HARDWARE COMPONENTS: 

- BegaleBone Black running Debian Linux (Control Node)
- TIVA C Series TM4C1294XL development board running FreeRTOS (Remote Node)
- Soil Moisture Sensor (Analog)
- Temperature Sensor (SPI)
- SPI, UART

## SOFTWARE COMPONENTS:
- Multithreading
- Built In Self Test
- Inter Process Communication : Message Queues, Task Notification, Mutex
- State Machines, Timers
- Hardware Drivers for sensors.

## ARCHITECTURE DIAGRAM :
![Architecture Diagram](https://github.com/PuneetBansal/REMOTE-GREENHOUSE-MONITORING-AUTOMATION-SYSTEM/blob/master/Images/SystemArchitecture.jpg)

## FEATURES :

- **Remote Node (Tiva C-Series LaunchPad running FreeRTOS):**
  - SPI based Temperature sensor and Analog Soil Moisture Sensor Interface.
  - Displays the sensing and actuation data on LCD.
  - LED’s to indicate the mode of operation : Normal, Degraded, Fail.
  - PWM control of the water pump/fan to change the speed according to control signal received. 
  - Takes reading from the sensors in the sensor task and sends the data to the control node via the SPI task.
  - Receives control data from the control node to perform necessary actuation i.e. turn on/off the fan/pump.


- **Control Node (BeagleBone Black running Debian Linux) :**
  - Takes the data from remote node in the SPI task, compares the value to set threshold in the decision task and sends control signals     back to the remote node via SPI task. 
  - Has pre set thresholds to provide control signals to the remote node based on sensing data received. 
  - Logging feature with different log levels (debug, error, info) to support performance and failure analysis. 
- The system provides appropriate error reporting, fault detection, fault tolerance, and failsafe behavior.
- In case the remote node loses communication with the control node or in the absence/failure of a single sensor, the system runs in a     degraded state.
- In case of sensor pulled out/ not working properly
- Inter Board SPI to communicate between the remote node and control node.
- Signal Handler for safe exits.


## INTERBOARD SPI COMMUNICATION: 

![Architecture Diagram](https://github.com/PuneetBansal/REMOTE-GREENHOUSE-MONITORING-AUTOMATION-SYSTEM/blob/master/Images/InterBoardSPI.jpg)

- The major part of our project is enabling inter-board communication between Beagle Bone and TIVA using SPI communication protocol.
- Beagle Bone Black (Control Node) is configured as the master and TIVA (Remote Node) is configured as slave
- The Beagle  Bone initiates communication by sending a poll command (0x01) to TIVA. TIVA receives this command and sends a 16-bit          packet. The lower 8 bits of the packet represents source of the data and upper 8 bits represent the Transaction ID.
- Beagle Bone receives this packet and parses the source and transaction id. If the transaction id is the same as the previous transaction id , this indicates that the data is old and should be discarded. In such a case , the master again sends a poll request (0x01 ) to the slave.
- I f the transaction id  is different than the previous one, this indicates the presence of valid data and so, the master sends a Get Data Command to the slave.
- The slave receives this command and sends valid sensor data to the master.
- After receiving valid sensor data, the master again sends the poll request and repeats the above steps.

## HOW TO RUN THE PROJECT :

###### Beaglebone: 
- Go the beaglebone folder, perform make mainTask. 
- Before running the prgoram on beaglebone, the following configurations needs to be done on command line. 
  - config-pin P9.17 spi_cs
  - config-pin P9.18 spi
  - config-pin P9.21 spi
  - config-pin P9.22 spi_sclk
- Now, run the program with command sudo ./mainTask.
###### TIVA Board :
- Import the project to CCS workspace and build.



