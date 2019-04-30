# Repository for Project2 of AESD.
## How to Run the project : 

Beaglebone: Go the beaglebone folder, perform make mainTask. 
Before running the prgoram on beaglebone, the following configurations needs to be done on command line. 

config-pin P9.17 spi_cs
config-pin P9.18 spi
config-pin P9.21 spi
config-pin P9.22 spi_sclk

Now, run the program with command sudo ./mainTask.
TIVA Board. Import the project to CCS workspace and build.
