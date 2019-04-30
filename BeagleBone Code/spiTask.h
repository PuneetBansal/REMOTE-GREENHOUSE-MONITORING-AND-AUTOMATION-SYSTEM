
/************************************************************************************************
* File name   : spiTask.h                                                                       *
* Authors     : Nachiket Kelkar and Puneet Bansal                                               *
* Description : Contains header files and function definitions for spiTask.c                    *
* Tools used  : GNU make, gcc, arm-linux-gnueabihf-gcc                                          *
************************************************************************************************/

#include "myTimer.h"

#define POLL_REQ 0x0001
#define DATA_REQ 0X0002
#define DEFAULT_TRID 0x00
#define SOURCE_BITMASK 0x00ff
#define TRID_BITMASK 0xff00
uint8_t prev_trid;
uint8_t degradedState;

/**
 * @brief: Spi task call back function. Creates timers to take poll slave after every 2 second and to check whether slave is active or not.
 * Takes the message from slave via SPI, sends it to the decision task to get command for the data and to logger task. It also receives the command 
 * messages from decisionTask and sends it via SPI to the slave.
 * 
 * */
void *spiTaskRoutine(void *);

/**
 * @brief: Configures SPI
 * @returns : file descriptor for SPI
 * */
int spi_init();

/**
 * @brief: Uses IOCTL command to send and receive specific bytes of data from the slave. 
 * @param1 : file descriptor for SPI
 * @param2 : buffer pointing to the data to send
 * @param3 : buffer pointing to the data received.
 * @param4 : number of bytes of data to transmit/receive.
 * 
 * */
void spi_transfer(int , uint16_t const *, uint16_t const *, size_t);

/**
 * @brief : function to check if the remote node is operating in degraded state or not.
 * @param 1: sensor source to which the received message pertains.
 *  * */
void checkDegradedState(uint8_t);

/*************GLOBAL VARIABLES**************************/
uint8_t temp,soilMoisture;

