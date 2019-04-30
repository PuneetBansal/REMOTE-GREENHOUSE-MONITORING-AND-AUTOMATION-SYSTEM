#include <mqueue.h>
#include <stdbool.h>
#include <stdint.h>

/* Message queues for all the tasks */
#define SPIQUEUENAME "/spiqueue1"
#define DECISIONQUEUENAME "/decisionqueue1"
#define LOGQUEUENAME "/logqueuequeue10"

/* Message queue size for all the tasks */
#define SPI_QUEUE_SIZE	10
#define DECISION_QUEUE_SIZE	10
#define LOG_QUEUE_SIZE	10


typedef enum
{
info,
alert,
debug
}loglevel_enum;

typedef enum{
sensing,
actuation,
}type_enum;

typedef enum{
none_state,
active,
degraded,
notActive,
}remoteNodeStatus_typedef;

/* Structure to communicate to the maintask*/
typedef struct
{	uint16_t sourceAndCommand;
}spiStruct;

/* Structure to communicate to the temperature task*/
typedef struct{
	uint8_t source;
	uint16_t data;
}decisionStruct;

typedef struct{
	uint8_t source;
	uint16_t data;
	loglevel_enum logLevel; 
	type_enum type;	
	remoteNodeStatus_typedef remoteStatus;
}logStruct;


/*user defined functions*/

/**
 * @name: mqueue_init
 * 
 * @param1: message queue name
 * @param2: max message queue size
 * @param3: size of the data to send
 * 
 * @description: wrapper around mq_open function. Sets the attributes of the queue and opens the queue with the specified parameters.
 * 
 * return: message queue file descriptor.
 * */
mqd_t mqueue_init(const char*, int, int);
