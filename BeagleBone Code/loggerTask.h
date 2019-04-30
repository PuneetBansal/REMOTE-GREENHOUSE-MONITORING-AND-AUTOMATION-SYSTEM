
/************************************************************************************************
* File name   : decisionTask.h                                                                  *
* Authors     : Puneet Bansal and Nachiket Kelkar                                               *
* Description : Contains header files and function definitions for decisionTask.c               *
* Tools used  : GNU make, gcc, arm-linux-gnueabihf-gcc                                          *
************************************************************************************************/


#include "mq.h"
#include <time.h>

/**
 * @brief: Logger task call back function. Logs the following to a file. 
 * 1)Timestamp
 * 2)Source of the sensor/actuation value
 * 3)Sensor/Actuation data values
 * 4)Log level : ALERT, DEBUG, INFO
 * 5)Reporting if remote node is in active, degraded or not active state. 
 * 
 * Receives data from spitask and decision task and logs it to the file.
 * 
 * @param1: name of the log file I
 *  * */
void *loggerTaskRoutine(void *);

/**
 * @brief : returns the time stamp value
 * @preturns: timestamp
 * */
char* printTimeStamp();

/**
 * @brief: opens the file in appropriate mode, and logs messages depending upon several conditions.
 *  * */
void logToFile(char *, logStruct);