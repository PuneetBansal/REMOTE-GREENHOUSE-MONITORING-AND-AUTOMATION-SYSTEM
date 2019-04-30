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
    gpio_init(55,1);
    gpio_init(54,1);
    gpio_init(53,1);
    printf("Before write\n");
    
    while(1)
    {
        gpio_write_value(56,0);
        gpio_write_value(55,0);
        gpio_write_value(54,1);
        gpio_write_value(53,1);
       for(i=0;i<5000000;i++);
       gpio_write_value(56,1);
       gpio_write_value(55,1);
       gpio_write_value(54,0);
       gpio_write_value(53,0);
       for(i=0;i<5000000;i++);
    //pwm_generate(9);
    //pwm_generate(8);
    //toggle_led();
    }
}