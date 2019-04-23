/*
 * spi.c
 *
 *  Created on: Apr 15, 2019
 *      Author: nachi
 */

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"
#include "spi.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

extern uint32_t g_ui32SysClock;
QueueHandle_t IBQueue;
uint8_t trid;
uint16_t packet;

void spi_init(uint32_t mode, uint32_t clk_speed)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));

    GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);
    GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);
    GPIOPinConfigure(GPIO_PD2_SSI2FSS);
    GPIOPinConfigure(GPIO_PD3_SSI2CLK);

    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    SSIConfigSetExpClk(SSI2_BASE, g_ui32SysClock, SSI_FRF_MOTO_MODE_0, mode, clk_speed, 16);
    SSIEnable(SSI2_BASE);
}

void spi_data_write(uint64_t data_to_write, uint8_t no_of_bytes)
{
        SSIDataPut(SSI2_BASE, (uint16_t)data_to_write);
}

uint16_t spi_data_read()
{
    uint32_t buffer;
    SSIDataGet(SSI2_BASE, &buffer);
    return (uint16_t)buffer;
}

void InterBoardSPI(void *pvParameters)
{
//    SPI testing
    uint16_t received_data;
    IBStruct rec_msg;

    //Initialize the queue
    IBQueue = xQueueCreate( 10, sizeof(IBStruct));

    spi_init(SLAVE, 500000);

    TimerHandle_t StartSend = xTimerCreate("Test", pdMS_TO_TICKS(1000), pdTRUE, (void*)1, TestCallback);
    BaseType_t return_val = xTimerStart(StartSend, pdMS_TO_TICKS(100));

    while(1)
    {
//      Wait till the message is received from queue
        xQueueReceive(IBQueue, &rec_msg, portMAX_DELAY);
//      get the structured buffer of values to send
        packet = ((uint16_t)++trid<<8) | rec_msg.source;
//      Send the data the beagle bone black (5 bytes)
        spi_data_write(packet, 1);
        received_data = spi_data_read();
        UARTprintf("RX 1 - %x\n\r",received_data);
        spi_data_write(rec_msg.data, 1);
        received_data = spi_data_read();
        UARTprintf("RX 2 - %x\n\r",received_data);
        spi_data_write(packet, 1);
//      wait till valid control message is received from beagle bone green
//      send the message using queue to corresponding actuator.
//      repeat all the steps
    }
}

void TestCallback(TimerHandle_t xtimer)
{
    // Notify the task to take the readings
    static uint16_t data = 0x5050;
    IBStruct data_to_send;
    data_to_send.source = 0x30;
    data_to_send.data = data ++;
    xQueueSend(IBQueue, &data_to_send, pdMS_TO_TICKS(0));
}
