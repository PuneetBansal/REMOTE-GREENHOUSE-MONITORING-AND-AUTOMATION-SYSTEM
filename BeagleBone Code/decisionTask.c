#include "genericIncludes.h"
#include "decisionTask.h"
#include "mq.h"

void *decisionTaskRoutine(void *dataObj)
{
spiStruct dataToSend;
decisionStruct dataReceived;
logStruct dataToSendToLog;

printf("Entered Decision Routine\n");
mqd_t decisionQueue = mqueue_init(DECISIONQUEUENAME, DECISION_QUEUE_SIZE, sizeof(decisionStruct));
mqd_t spiQueue = mqueue_init(SPIQUEUENAME, SPI_QUEUE_SIZE, sizeof(spiStruct));
mqd_t logQueue = mqueue_init(LOGQUEUENAME, LOG_QUEUE_SIZE, sizeof(logStruct));

dataToSendToLog.logLevel = info;
dataToSendToLog.type = actuation;

// mqd_t decisionQueue;
//     struct mq_attr queue_attr;
//     //printf("queue name in %s is %s\n",__func__,queue_name);
//     //printf("queue size in %s is %d\n",__func__,queue_size);
//     //printf("queue name in %s is %s",__func__,queue_name);
//     queue_attr.mq_maxmsg  = DECISION_QUEUE_SIZE;
//     queue_attr.mq_msgsize = sizeof(decisionStruct);
//     //queue_attr.mq_flags   = O_NONBLOCK;
//     decisionQueue = mq_open(DECISIONQUEUENAME, O_CREAT | O_RDWR , 0666, &queue_attr);

// mqd_t spiQueue;
//     struct mq_attr queue_attr;
//     queue_attr.mq_maxmsg  = SPI_QUEUE_SIZE;
//     queue_attr.mq_msgsize = sizeof(spiStruct);
//     spiQueue = mq_open(SPIQUEUENAME, O_CREAT | O_RDWR , 0666, &queue_attr);

if(decisionQueue < 0)
{
    perror("Failed to create decision queue");
}
if(spiQueue < 0)
{
    perror("Failed to create spi queue");
}
if(logQueue < 0)
{
    perror("Failed to create log queue");
}

while(1)
{
//printf("BEFORE MQ REC in decision task\n");
int ret = mq_receive(decisionQueue,(char*)&dataReceived,sizeof(decisionStruct),0);
if(ret<0)
{
//printf("mq receive failed in decision task\n");
}
else
{
    printf("Data received from spi task is %x from source %x\n",dataReceived.data,dataReceived.source);
    if(dataReceived.source == 0x55)
    {
        uint16_t temp;
        temp = dataReceived.data;
        temp =temp >> 2 ;
        dataToSendToLog.source = 0x55;
        printf("source is temperature");
        if(temp > TEMP_THRESHOLD)
        {
         dataToSend.sourceAndCommand = 0x01;
         dataToSend.sourceAndCommand <<= 8;
         dataToSend.sourceAndCommand |= dataReceived.source;
         
         dataToSendToLog.data =0x01;
        }
        else
        {
         dataToSend.sourceAndCommand = 0x00;
         dataToSend.sourceAndCommand <<= 8;
         dataToSend.sourceAndCommand |= dataReceived.source;
         dataToSendToLog.data =0x00;
        }
        int ret=mq_send(spiQueue, (char*)&dataToSend,sizeof(spiStruct),0);
        if(ret<0)
        {
            printf("Sending to spi queue failed\n");
        }    
        ret=mq_send(logQueue, (char*)&dataToSendToLog,sizeof(logStruct),0);
        if(ret<0)
        {
            printf("Sending to log queue failed\n");
        }       
    }
    if(dataReceived.source == 0xaa)
    {
        printf("Source is soil moisture");
        uint8_t command;
        dataToSendToLog.source = 0xaa;
        command = getCommand(dataReceived.data);
        dataToSend.sourceAndCommand = command;
        dataToSend.sourceAndCommand <<= 8;
        dataToSend.sourceAndCommand |= dataReceived.source;
        dataToSendToLog.data =command;

        printf("Command message is %x\n",dataToSend.sourceAndCommand);
        int ret=mq_send(spiQueue, (char*)&dataToSend,sizeof(spiStruct),0);
        if(ret<0)
        {
            printf("Sending to spi queue failed\n");
        }
        ret=mq_send(logQueue, (char*)&dataToSendToLog,sizeof(logStruct),0);
        if(ret<0)
        {
            printf("Sending to log queue failed\n");
        }          

    }
    
     
}

}
}

uint8_t getCommand(uint16_t data)
{
    uint8_t commandToSend;
    if(data>0 && data <500)
    {
        commandToSend = 10;
    }
    else if(data>=500 && data <1500)
    {
        commandToSend = 6;   
    }
    else if(data>=1500 && data <3000)
    {
        commandToSend = 3;   
    }
    else if(data>=3000 && data <4095)
    {
        commandToSend = 0;   
    }
return commandToSend;
}