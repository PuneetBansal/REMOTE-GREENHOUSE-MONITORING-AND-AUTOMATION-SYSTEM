/*
 * actuator.c
 *
 *  Created on: Apr 22, 2019
 *      Author: puneet bansal and Nachiket Kelkar
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
    lcd_on();
    lcd_pos(0, 0);
    lcd_write_string("Temp");
    lcd_pos(0, 8);
    lcd_write_string("Moist");
    lcd_pos(2, 0);
    lcd_write_string("Fan");
    lcd_pos(2, 8);
    lcd_write_string("Motor");
    LCDQueue=xQueueCreate(10, sizeof(LCDStruct));
    float val=0;

while(1)
{
    if(xQueueReceive(LCDQueue, &dataReceived, portMAX_DELAY))
    {
        switch(dataReceived.source)
        {
        case 0x55:
            if(dataReceived.task == 1)
            {
                lcd_pos(1, 0);
                lcd_write_string("        ");
                lcd_pos(1, 0);
                val=dataReceived.sensing_data*0.25;
                if(val != 0)
                {
                    lcd_print_float(val);
                }
                else
                {
                    lcd_write_string("SEN NC");
                }
            }
            else
            {
                lcd_pos(3, 0);
                lcd_write_string("        ");
                lcd_pos(3, 0);
                lcd_print_digit((long)dataReceived.actuation_data);
            }
            break;
        case 0xaa:
            if(dataReceived.task == 1)
            {
                lcd_pos(1, 8);
                lcd_write_string("        ");
                val=dataReceived.sensing_data;
                lcd_pos(1, 8);
                if(val != 0)
                {
                    lcd_print_float(val);
                }
                else
                {
                    lcd_write_string("SEN NC");
                }
            }
            else
            {
                lcd_pos(3, 8);
                lcd_write_string("        ");
                lcd_pos(3, 8);
                lcd_print_digit(dataReceived.actuation_data);
                break;
            }
         }
    }
}

}

void FanTask(void *pvParameters)
{
    UARTprintf("Entered Fan Task");
    uint32_t notificationVal;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    while(1)
    {
        xTaskNotifyWait(0x00, 0xffffffff , &notificationVal , portMAX_DELAY);
        switch(notificationVal)
        {
        case 0: UARTprintf("Turn off the fan\n");
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);
        break;

        case 1: UARTprintf("Turn on the fan\n");
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
        break;
        }
    }

}

int duty_cycle = 0;

void MotorTask(void *pvParameters)
{
//    UARTprintf("Entered Motor Task");
    uint32_t notificationVal;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

    GPIOPadConfigSet(GPIO_PORTG_BASE, GPIO_PIN_0, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
    GPIODirModeSet(GPIO_PORTG_BASE, GPIO_PIN_0, GPIO_DIR_MODE_OUT);

    /* Initialize the timer for periodic measurements */
    TimerHandle_t MotorTimer = xTimerCreate("Motor", pdMS_TO_TICKS(1), pdTRUE, (void*)0, MotorCallback);

    /* Start the timer after 100ms */
    BaseType_t return_val = xTimerStart(MotorTimer, pdMS_TO_TICKS(0));

    while(1)
    {
        xTaskNotifyWait(0x00, 0xffffffff , &notificationVal , portMAX_DELAY);
        duty_cycle = notificationVal;
    }
}

void MotorCallback(TimerHandle_t xtimer)
{
    static int x = 0;
    if(x < duty_cycle)
    {
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, GPIO_PIN_0);
        x ++;
    }
    else
    {
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0);
        x ++;
    }
    if(x == 10)
    {
        x = 0;
    }
}
