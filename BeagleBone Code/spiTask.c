/************************************************************************************************
* File name   : spitask.c                                                                       *
* Authors     : Puneet Bansal and  Nachiket Kelkar                                              *
* Description : Functions to configure spi, send and receive values to the remote node via SPI  *
* Tools used  : GNU make, gcc, arm-linux-gnueabihf-gcc        									*
* References  : https://raw.githubusercontent.com/torvalds/linux/master/tools/spi/spidev_test.c *
************************************************************************************************/

#include "genericIncludes.h"
#include "spiTask.h"
#include "mq.h"
#include "myTimer.h"

static const char *device = "/dev/spidev1.0";
static uint32_t mode;
static uint8_t bits = 16;
static uint32_t speed = 250000;
static uint16_t delay;
uint16_t tx1;
uint16_t rx1;
uint16_t tx;
uint16_t rx;

void signal_handler(int , siginfo_t * , void* );

void *spiTaskRoutine(void *dataObj)
{
printf("Entered SPI Task\n");

int spi_fd;
uint8_t present_trid,present_source;

spiStruct dataReceived;
decisionStruct dataToSend;
logStruct dataToSendToLog;

spi_fd=spi_init();

/***************Configuring the timer to poll remote node every 2 second****************/
struct sigevent spiEvent;
struct sigaction spiAction;
struct itimerspec spiSpec;
timer_t spiTimer;


spiAction.sa_flags     = SA_SIGINFO;
spiAction.sa_sigaction = signal_handler;

if((sigaction(SIGRTMIN, &spiAction, NULL))<0)
{
    perror("Failed setting timer handler for SPI");
}

//Assigning signal to timer
spiEvent.sigev_notify             = SIGEV_SIGNAL;
spiEvent.sigev_signo              = SIGRTMIN;
spiEvent.sigev_value.sival_ptr    = &spiTimer;

if((timer_create(CLOCK_REALTIME, &spiEvent, &spiTimer)) < 0)
{
    perror("Timer creation failed for spi task");
}

//Setting the time and starting the timer
spiSpec.it_interval.tv_nsec = 0;
spiSpec.it_interval.tv_sec  = 2;
spiSpec.it_value.tv_nsec    = 0;
spiSpec.it_value.tv_sec     = 2;

if((timer_settime(spiTimer, 0, &spiSpec, NULL)) < 0)
{
    perror("Starting timer in SPI task failed");
}


/*********************Configuring a 5 second timer to check remote node connection*********************/

struct sigevent connectionEvent;
struct sigaction connectionAction;
struct itimerspec connectionSpec;
timer_t connectionTimer;


connectionAction.sa_flags     = SA_SIGINFO;
connectionAction.sa_sigaction = signal_handler;

if((sigaction(SIGRTMIN+1, &connectionAction, NULL))<0)
{
    perror("Failed setting timer handler for SPI");
}

//Assigning signal to timer
connectionEvent.sigev_notify             = SIGEV_SIGNAL;
connectionEvent.sigev_signo              = SIGRTMIN + 1;
connectionEvent.sigev_value.sival_ptr    = &connectionTimer;

if((timer_create(CLOCK_REALTIME, &connectionEvent, &connectionTimer)) < 0)
{
    perror("Timer creation failed for checking connection");
}

//Setting the time and starting the timer
connectionSpec.it_interval.tv_nsec = 0;
connectionSpec.it_interval.tv_sec  = 1;
connectionSpec.it_value.tv_nsec    = 0;
connectionSpec.it_value.tv_sec     = 8;

if((timer_settime(connectionTimer, 0, &connectionSpec, NULL)) < 0)
{
    perror("Starting timer for checking connection failed");
}

 /*************************************************************************************************/
mqd_t decisionQueue = mqueue_init(DECISIONQUEUENAME, DECISION_QUEUE_SIZE, sizeof(decisionStruct));
mqd_t spiQueue = mqueue_init(SPIQUEUENAME, SPI_QUEUE_SIZE, sizeof(spiStruct));
mqd_t logQueue = mqueue_init(LOGQUEUENAME, LOG_QUEUE_SIZE, sizeof(logStruct));

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
    perror("Failed to create spi queue");
}

dataToSendToLog.type=sensing;
dataToSendToLog.logLevel=info;

int c=0;
prev_trid=DEFAULT_TRID;
while(1)
{
	dataToSendToLog.logLevel=none_state;
    int k;
	if(connection_handler)
	{
		connection_handler=0;
		printf("Remote Node not active\n");

		gpio_write_value(55,1); //turning on the LED if remote node is not active.

		dataToSendToLog.logLevel=alert;
		dataToSendToLog.remoteStatus=notActive;
		if(mq_send(logQueue,(char*)&dataToSendToLog,sizeof(logStruct),0)!=0)
		{
				printf("Data sending from spitask to logTask failed 1");
		}
	}
	else if(degradedState==1)
	{
						
		degradedState=0;
		dataToSendToLog.logLevel=alert;
		dataToSendToLog.remoteStatus=degraded;

		gpio_write_value(56,1); //turning on LED on pin 56 if remote node working in degraded state
		//led blink
		if(mq_send(logQueue,(char*)&dataToSendToLog,sizeof(logStruct),0)!=0)
		{
			printf("Data sending from spitask to logTask failed 2");
		}
	}
	else if(revived==1)
	{
		revived=0;
		dataToSendToLog.logLevel=alert;
		dataToSendToLog.remoteStatus=active;
		if(mq_send(logQueue,(char*)&dataToSendToLog,sizeof(logStruct),0)!=0)
		{
			printf("Data sending from spitask to logTask failed 2");
		}
	}
	// else
	// {			
	// 	dataToSendToLog.logLevel=none_state;
	// }
	dataToSendToLog.logLevel=none_state;
	
	/*Polling the remote node*/
    if(spi_handler==1)
    {
        spi_handler=0;
        tx=POLL_REQ;
        rx=0;
        spi_transfer(spi_fd,&tx,&rx,2);
		printf("rx is %x\n",rx);
		present_source= (rx & SOURCE_BITMASK);
		present_trid= (rx & TRID_BITMASK) >> 8;
		printf("source is %x and trid is %x\n",present_source,present_trid);

    if(present_trid != prev_trid )
		{
		gpio_write_value(55,0); //turning off the led if remote node is active	
        checkDegradedState(present_source); 
		if((timer_settime(connectionTimer, 0, &connectionSpec, NULL)) < 0)
        {
            perror("Starting timer for checking connection failed");
     	}
			
			for(k=0;k<100000;k++); //Giving inline waits to synchronise the communication
			for(k=0;k<100000;k++);
			for(k=0;k<100000;k++);
			for(k=0;k<100000;k++);

			tx1=DATA_REQ;
			rx1=0;

			spi_transfer(spi_fd, &tx1, &rx1, 2); 
	
            dataToSend.source=present_source;
			dataToSend.data=(rx1*0.25);		
          
			dataToSendToLog.source = present_source;
			dataToSendToLog.data = rx1;

			printf("Data  is %x\n",dataToSendToLog.data);
			printf("Source is %x\n",dataToSendToLog.source);

            if(mq_send(decisionQueue,(char*)&dataToSend,sizeof(decisionStruct),0)!=0)
			{
				printf("Data sending from spitask to decisionTak failed");
			}
			else
			{
				//printf("Before mq receive in spitask\n");
				if(mq_receive(spiQueue,(char*)&dataReceived,sizeof(spiStruct),0)>-1)
            	{
                //printf("data received from decision queue in spitask %x\n",dataReceived.sourceAndCommand);
				spi_transfer(spi_fd, &dataReceived.sourceAndCommand, &rx1, 2);
            	}
				
			}
			dataToSendToLog.remoteStatus=none_state;
			if(mq_send(logQueue,(char*)&dataToSendToLog,sizeof(logStruct),0)!=0)
			{
				printf("Data sending from spitask to logTask failed");
			}
			           
			//printf("Came out of mq_receive in spiTask\n");
		prev_trid= present_trid;

		}
	for(k=0;k<100000;k++);
    for(k=0;k<100000;k++);
    for(k=0;k<100000;k++);
    for(k=0;k<100000;k++);	        
    
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

timer_delete(spiTimer);
timer_delete(connectionTimer);

pthread_exit(NULL);
}

int spi_init()
{ 
    int fd;
    int ret=0;
    
    fd = open(device, O_RDWR);
	if (fd < 0)
	{
        perror("can't open device");
        abort();
    }
    

	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
	{
        perror("can't set spi mode");
        abort();
    }
	
    ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1)
    {
		perror("can't get spi mode");
        abort();
    }

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
	{
        perror("can't set bits per word");
        abort();
    }

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
	{
        perror("can't get bits per word");
        abort();
    }

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
	{
        perror("can't set max speed hz");
        abort();
    }

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
    {
        perror("can't get max speed hz");
        abort();
    }   

	printf("spi mode: 0x%x\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
    
    return fd;
}

void spi_transfer(int fd, uint16_t const *tx, uint16_t const *rx, size_t len)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	if (mode & SPI_TX_QUAD)
		tr.tx_nbits = 4;
	else if (mode & SPI_TX_DUAL)
		tr.tx_nbits = 2;
	if (mode & SPI_RX_QUAD)
		tr.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
		tr.rx_nbits = 2;
	if (!(mode & SPI_LOOP)) {
		if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
			tr.rx_buf = 0;
		else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
			tr.tx_buf = 0;
	}

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
	{
        perror("can't send spi message");
        abort();
    }
	
}

void checkDegradedState(uint8_t source)
{
	if(source==0x55)
	{
		temp++;
	}
	else if(source ==0xaa)
	{
		soilMoisture++;
	}
	if(abs(temp-soilMoisture)>10)
	{
		printf("------------------------------------>DEGRADED STATE<-------------------------------------- \n");
		degradedState=1;
		temp=0;
		soilMoisture=0;
		notDegraded=0;
	}
	else if(abs(temp-soilMoisture)<10)
	{
		notDegraded++;
		if(notDegraded>20)
		{
		printf("------------------------------------>ACTIVE<-------------------------------------- \n");
		revived=1;
		gpio_write_value(56,0);
		notDegraded=0;
		}
		
	}
	

	
}

void signal_handler(int sig, siginfo_t * var1, void* var2)
{
switch(sig)
{
    case 2:
		printf("SIGINT signal is received\n ----------> Exiting thread <---------\n");
		exitThread = true;		
		break;
	case 34: 
		spi_handler=1;
    	break;

    case 35: 
		connection_handler=1;
    	break;
}
}

