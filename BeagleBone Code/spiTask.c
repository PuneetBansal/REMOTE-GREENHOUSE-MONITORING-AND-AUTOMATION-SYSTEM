#include "genericIncludes.h"
#include "spiTask.h"
#include "mq.h"

static const char *device = "/dev/spidev1.0";
static uint32_t mode;
static uint8_t bits = 16;
static uint32_t speed = 250000;
static uint16_t delay;
uint16_t tx1;
uint16_t rx1;
uint16_t tx;
uint16_t rx;


void *spiTaskRoutine(void *dataObj)
{
printf("Entered SPI Task\n");

int spi_fd;
uint8_t present_trid,present_source;

spiStruct dataReceived;
decisionStruct dataToSend;

spi_fd=spi_init();

int retval=myTimerCreate(0,2);
if(retval!=0)
{
     printf("Timer creation for SPI failed\n");
}

mqd_t decisionQueue = mqueue_init(DECISIONQUEUENAME, DECISION_QUEUE_SIZE, sizeof(decisionStruct));

//   mqd_t decisionQueue;
//     struct mq_attr queue_attr;
//     //printf("queue name in %s is %s\n",__func__,queue_name);
//     //printf("queue size in %s is %d\n",__func__,queue_size);
//     //printf("queue name in %s is %s",__func__,queue_name);
//     queue_attr.mq_maxmsg  = DECISION_QUEUE_SIZE;
//     queue_attr.mq_msgsize = sizeof(decisionStruct);
//     //queue_attr.mq_flags   = O_NONBLOCK;
//     decisionQueue = mq_open(DECISIONQUEUENAME, O_CREAT | O_RDWR , 0666, &queue_attr);


// mqd_t spiQueue;
// struct mq_attr queue_attr;
// queue_attr.mq_maxmsg  = SPI_QUEUE_SIZE;
// queue_attr.mq_msgsize = sizeof(spiStruct);
// //queue_attr.mq_flags   = O_NONBLOCK;
// spiQueue = mq_open(SPIQUEUENAME, O_CREAT | O_RDWR , 0666, &queue_attr);

mqd_t spiQueue = mqueue_init(SPIQUEUENAME, SPI_QUEUE_SIZE, sizeof(spiStruct));
mqd_t logQueue = mqueue_init(SPIQUEUENAME, SPI_QUEUE_SIZE, sizeof(spiStruct));

if(decisionQueue < 0)
{
    perror("Failed to create decision queue");
}
if(spiQueue < 0)
{
    perror("Failed to create spi queue");
}

prev_trid=DEFAULT_TRID;
while(1)
{
    int k;
    if(spi_handler==1)
    {
        printf("Entered while wala handler\n");
        spi_handler=0;
        tx=POLL_REQ;
        rx=0;
        spi_transfer(spi_fd,&tx,&rx,2);
		printf("rx is %x",rx);
		present_source= (rx & SOURCE_BITMASK);
		present_trid= (rx & TRID_BITMASK) >> 8;
		printf("source is %x and trid is %x\n",present_source,present_trid);

    if(present_trid != prev_trid )
		{
			
            printf("Entered if condition\n");
			for(k=0;k<100000;k++);
			for(k=0;k<100000;k++);
			for(k=0;k<100000;k++);
			for(k=0;k<100000;k++);

			tx1=DATA_REQ;
			rx1=0;

			spi_transfer(spi_fd, &tx1, &rx1, 2);
			//printf("requesting valid data %x\n",default_tx1);
			printf("valid data is %x\n",rx1);
            dataToSend.source=present_source;
            dataToSend.data=rx1;
			printf("Data  is %x\n",dataToSend.data);
			printf("Source is %x\n",dataToSend.source);

			// if(dataToSend.source==0x55)
			// {
			// 	dataReceived.sourceAndCommand=0x00;
			// }
			// else if(dataToSend.source=0xaa)
			// {
			// 	dataReceived.sourceAndCommand=0x01;
			// }
//			spi_transfer(spi_fd, &dataReceived.sourceAndCommand, &rx1, 2);

            if(mq_send(decisionQueue,(char*)&dataToSend,sizeof(decisionStruct),0)!=0)
			{
				printf("Data sending from spitask to decisionTak failed");
			}
			else
			{
				printf("Before mq receive in spitask\n");
				if(mq_receive(spiQueue,(char*)&dataReceived,sizeof(spiStruct),0)>-1)
            	{
                printf("data received from decision queue in spitask %x\n",dataReceived.sourceAndCommand);
				spi_transfer(spi_fd, &dataReceived.sourceAndCommand, &rx1, 2);
            	}
				
			}
			
           
			//printf("Came out of mq_receive in spiTask\n");
		prev_trid= present_trid;

		}
	for(k=0;k<100000;k++);
    for(k=0;k<100000;k++);
    for(k=0;k<100000;k++);
    for(k=0;k<100000;k++);	        
    }
}
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
    
	/*
	 * spi mode
	 */
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

	/*
	 * bits per word
	 */
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

	/*
	 * max speed hz
	 */
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



