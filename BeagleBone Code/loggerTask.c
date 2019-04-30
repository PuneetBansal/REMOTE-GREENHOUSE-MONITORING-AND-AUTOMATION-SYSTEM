/************************************************************************************************
* File name   : loggerTask.c                                                                    *
* Authors     : Puneet Bansal and  Nachiket Kelkar                                              *
* Description : Functions to configure logging to a file                                        *
* Tools used  : GNU make, gcc, arm-linux-gnueabihf-gcc        									*
************************************************************************************************/

#include "genericIncludes.h"
#include "loggerTask.h"

extern uint8_t count;
extern void signal_handler(int , siginfo_t * , void* );

void *loggerTaskRoutine(void *fileName)
{
logStruct dataReceived;

printf("Entered Logger Routine\n");
mqd_t decisionQueue = mqueue_init(DECISIONQUEUENAME, DECISION_QUEUE_SIZE, sizeof(decisionStruct));
mqd_t spiQueue = mqueue_init(SPIQUEUENAME, SPI_QUEUE_SIZE, sizeof(spiStruct));
mqd_t logQueue = mqueue_init(LOGQUEUENAME, LOG_QUEUE_SIZE, sizeof(logStruct));

while(1)
{
    int ret = mq_receive(logQueue,(char*)&dataReceived,sizeof(logStruct),0);
    if(ret<0)
    {
    //printf("mq receive failed in decision task\n");
    }
    else    
    {
        logToFile(fileName,dataReceived);

    }   
    if(exitThread)
	{
			break;
	}
}
mq_close(decisionQueue);
mq_close(spiQueue);
mq_close(logQueue);
mq_unlink(DECISIONQUEUENAME);
mq_unlink(SPIQUEUENAME);
mq_unlink(LOGQUEUENAME);
pthread_exit(NULL);
}

char* printTimeStamp()
{
    char* time_stamp=malloc(40);
    struct timespec thTimeSpec;
    clock_gettime(CLOCK_REALTIME, &thTimeSpec);
    sprintf(time_stamp,"[s: %ld, ns: %ld]",thTimeSpec.tv_sec,thTimeSpec.tv_nsec);
    return time_stamp;
}


void logToFile(char *fileName, logStruct dataToReceive)
{
   
    FILE *logging;
    char level[20];
    char source[20];
    char type[20];
    char remoteNodeStatus[30];

    if(dataToReceive.logLevel==alert)
    {
        strcpy(level,"[ALERT]");
    }
    else if(dataToReceive.logLevel==info)
    {
        strcpy(level,"[INFO]");   
    }
    else
    {
        strcpy(level,"[DEBUG]");
    }

    
    if(dataToReceive.source == 0x55)
    {
        strcpy(source,"TEMPERATURE");
    }
    else if(dataToReceive.source == 0xaa)
    {
        strcpy(source,"SOIL MOISTURE");   
    }
    else
    {
        strcpy(source,"");
    }
    

    printf("printing the value of source %s",source);

    if(dataToReceive.type == actuation)
    {
        strcpy(type,"[ACTUATION DATA]");
    }
    else if(dataToReceive.type == sensing)
    {
        strcpy(type,"[SENSING DATA]");   
    }

    if(dataToReceive.remoteStatus == degraded)
    {
        strcpy(remoteNodeStatus,"[DEGRADED STATE]");
    }
    else if(dataToReceive.remoteStatus == notActive)
    {
        strcpy(remoteNodeStatus,"[REMOTE NODE INACTIVE]");
    }
    else if(dataToReceive.remoteStatus == active)
    {
        strcpy(remoteNodeStatus,"[ACTIVE STATE]");
    }

  
    if(dataToReceive.remoteStatus==none_state)
    {
    if(count ==1)
    {
        logging = fopen(fileName,"w");
        count=0;
    }
    else
    {
        logging = fopen(fileName,"a");
    }
       
    fprintf(logging,"%s %s [%s] %s %d \n",printTimeStamp(),level,source,type,dataToReceive.data); 
    fclose(logging);
    }
    else if(dataToReceive.remoteStatus== degraded || dataToReceive.remoteStatus ==notActive || dataToReceive.remoteStatus ==active)    
    {

    if(count ==1)
    {
        logging = fopen(fileName,"w");
        count=0;
    }
    else
    {
        logging = fopen(fileName,"a");
    }
    fprintf(logging,"%s %s %s\n",printTimeStamp(),level,remoteNodeStatus); 
    fclose(logging);
    
    }   
    
}

    