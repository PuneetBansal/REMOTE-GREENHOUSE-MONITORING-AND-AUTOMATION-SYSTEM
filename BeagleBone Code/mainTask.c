/************************************************************************************************
* File name   : maintask.c                                                                      *
* Authors     : Puneet Bansal and  Nachiket Kelkar                                              *
* Description : The main logic of the code                                                      *
* Tools used  : GNU make, gcc, arm-linux-gnueabihf-gcc                                          *
************************************************************************************************/

#include "genericIncludes.h"
#include "mainTask.h"

pthread_t spiTask,decisionTask,loggerTask;
extern void signal_handler(int , siginfo_t * , void* );
char *logFileName;
uint8_t count;

int main(int argc, char *argv[])
{
    spi_handler=0;
    count=1;
    temp=0;
    soilMoisture=0;
    recoveryIndiacation=0;
    printOnlyOnce=0;
    connection_handler =0;
    exitThread=false;
    notDegraded=0;
    revived=0;
    gpio_init(55,1);
    gpio_init(56,1);
    gpio_write_value(55,0);
    gpio_write_value(56,0);
    degradedState=0;
    logFileName = malloc(20);
    if(argc ==2)
    {
        strcpy(logFileName, argv[1]);
    }
    else
    {
        strcpy(logFileName, "logFile.txt");
    }    
    
    if(pthread_create(&spiTask,NULL,&spiTaskRoutine,NULL)!=0)
    {
        perror("SPI Task create failed");
    }
    if(pthread_create(&decisionTask,NULL,&decisionTaskRoutine,NULL)!=0)
    {
        perror("Decision Task create failed");
    }
    if(pthread_create(&loggerTask,NULL,&loggerTaskRoutine,logFileName)!=0)
    {
        perror("Logger Task create failed");
    }
    while(1)
    {
        if(exitThread)
	    {
		    break;
    	}
    }
    
    pthread_join(spiTask,NULL);
    pthread_join(decisionTask,NULL);
    pthread_join(loggerTask,NULL);
    return 0;
}