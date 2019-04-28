#include "genericIncludes.h"
#include "mainTask.h"

pthread_t spiTask,decisionTask;
int main()
{
    spi_handler=0;
    if(pthread_create(&spiTask,NULL,&spiTaskRoutine,NULL)!=0)
    {
        perror("SPI Task create failed");
    }
    if(pthread_create(&decisionTask,NULL,&decisionTaskRoutine,NULL)!=0)
    {
        perror("Decision Task create failed");
    }
    while(1)
    {
    }
    pthread_join(spiTask,NULL);
    pthread_join(decisionTask,NULL);
    return 0;
}