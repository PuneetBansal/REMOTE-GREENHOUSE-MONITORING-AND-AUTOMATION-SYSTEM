
/************************************************************************************************
* File name   : decisionTask.h                                                                  *
* Authors     : Puneet Bansal and Nachiket Kelkar                                               *
* Description : Contains header files and function definitions for decisionTask.c               *
* Tools used  : GNU make, gcc, arm-linux-gnueabihf-gcc                                          *
************************************************************************************************/

#include <stdint.h>
#define TEMP_THRESHOLD 30
#define SOILMOISTURE_THRESHOLD 100

/**
 * @brief : function that returns a command message based on the defined thresholds
 * @param1 : 16 bit data for which the commmand is to be generated.
 * @returns : 8 bit command 
 * */
uint8_t getCommand(uint16_t);

/**
 * @brief : Decision Task callback function. Receives sensor data from SPI task over message queue, determines the control message
 * and sends it to the SPI taks and the logger task.
 * */

void *decisionTaskRoutine(void *);