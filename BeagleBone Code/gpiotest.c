#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <stdint.h>

#include "gpio.h"

void main()
{
    uint64_t i;
    printf("Entered main\n");
    gpio_init(56,1);
    printf("Before write\n");
    
    while(1)
    {
    gpio_write_value(56,0);
    for(i=0;i<1000000;i++)
    {

    }
    gpio_write_value(56,1);
    for(i=0;i<1000000;i++)
    {

    }

    }
}