/*
 * actuator.c
 *
 *  Created on: Apr 22, 2019
 *      Author: punee
 */

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"
#include "../FreeRTOS/include/projdefs.h"

#include "spi.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "actuator.h"
#include "LCDdriver.h"

QueueHandle_t LCDQueue;

void LCDTask(void *pvParameters)
{
    UARTprintf("Entered LCD Task");
    LCDStruct dataReceived;
    lcd_init();
    LCDQueue=xQueueCreate(10, sizeof(LCDStruct));
    float val=0;
    uint8_t actData=0;

while(1)
{
    if(xQueueReceive(LCDQueue, &dataReceived, portMAX_DELAY))
    {
        switch(dataReceived.source)
        {
        case 0x55:
            lcd_pos(0, 1);
            val=dataReceived.sensing_data*0.25;
            lcd_print_float(val);

            lcd_pos(2, 1);
            lcd_print_digit(dataReceived.actuation_data);
            UARTprintf("Temperature values\n");
            break;
        case 0xaa:
            lcd_pos(0, 8);
            val=dataReceived.sensing_data;
            lcd_print_float(val);

            lcd_pos(2, 8);
            lcd_print_digit(dataReceived.actuation_data);
            UARTprintf("Soil Moisture Values\n");
            break;
         }

    }
    else
    {
        UARTprintf("Reading from LCD queue failed\n");
    }
}

}

void FanTask(void *pvParameters)
{
    UARTprintf("Entered Fan Task");
    uint32_t notificationVal;

    while(1)
    {
        xTaskNotifyWait(0x00, 0xffffffff , &notificationVal , portMAX_DELAY);
        switch(notificationVal)
        {
        case 0: UARTprintf("Turn off the fan");
        break;

        case 1: UARTprintf("Turn on the fan");
        break;
        }
    }

}

void MotorTask(void *pvParameters)
{
    UARTprintf("Entered Motor Task");
    uint32_t notificationVal;

    while(1)
    {
        xTaskNotifyWait(0x00, 0xffffffff , &notificationVal , portMAX_DELAY);
        switch(notificationVal)
        {
        case 0: UARTprintf("Turn off the motor");
        break;

        case 1: UARTprintf("Turn on the motor");
        break;
        }
    }
}







