#include "myTimer.h"
#include "genericIncludes.h"

/*
void signal_handler(int , siginfo_t * , void* );

int myTimerCreate(uint32_t ns, uint8_t sec, uint8_t signalNo)
{
struct sigevent spiEvent;
struct sigaction spiAction;
struct itimerspec spiSpec;
timer_t spiTimer;


spiAction.sa_flags     = SA_SIGINFO;
spiAction.sa_sigaction = signal_handler;

if((sigaction(SIGRTMIN+signalNo, &spiAction, NULL))<0)
{
    perror("Failed setting timer handler for SPI");
}

//Assigning signal to timer
spiEvent.sigev_notify             = SIGEV_SIGNAL;
spiEvent.sigev_signo              = SIGRTMIN + signalNo;
spiEvent.sigev_value.sival_ptr    = &spiTimer;

if((timer_create(CLOCK_REALTIME, &spiEvent, &spiTimer)) < 0)
{
    perror("Timer creation failed for spi task");
}

//Setting the time and starting the timer
spiSpec.it_interval.tv_nsec = ns;//100000000; //To poll the slave every 1s
spiSpec.it_interval.tv_sec  = sec;
spiSpec.it_value.tv_nsec    = ns;//100000000;
spiSpec.it_value.tv_sec     = sec;

if((timer_settime(spiTimer, 0, &spiSpec, NULL)) < 0)
{
    perror("Starting timer in SPI task failed");
}

return 0;
}

void signal_handler(int sig, siginfo_t * var1, void* var2)
{
switch(sig)
{
    case 34: spi_handler=1;
    break;

    case 35: connection_handler=1;
    printf("Remote Node not active\n");
    break;
}
}*/