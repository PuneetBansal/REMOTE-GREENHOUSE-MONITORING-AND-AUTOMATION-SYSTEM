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
#include "actuator.h"

extern uint32_t g_ui32SysClock;
QueueHandle_t IBQueue;
extern QueueHandle_t LCDQueue;
uint8_t trid = 0x00;
uint16_t packet;
extern TaskHandle_t IBTaskHandle;
extern TaskHandle_t FanTaskHandle;
extern TaskHandle_t MotorTaskHandle;


int prev_state = 0;

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
//    UARTprintf("SPI task\n");
//    SPI testing
//    uint16_t received_data;
//    uint16_t control_message;
//    IBStruct rec_msg;
    prev_state = 0;

    //Initialize the queue
    IBQueue = xQueueCreate( 15, sizeof(IBStruct));

    spi_init(SLAVE, 500000);

    spi_data_write(0x0011, 1);
    while(1)
    {
        test_function();
////      Wait till the message is received from queue
//        xQueueReceive(IBQueue, &rec_msg, portMAX_DELAY);
////      Increase priority of task
////        vTaskPrioritySet(IBTaskHandle, 2);
////      structure packet of values to send
//        packet = ((uint16_t)++trid<<8) | rec_msg.source;
//        UARTprintf("source - packet - data is %x - %x - %x\n",rec_msg.source,packet,rec_msg.data);
////      Send the packet the beagle bone black
//        spi_data_write(packet, 1);
//        received_data = spi_data_read();
//        UARTprintf("RX 1 - %x\n\r",received_data);
//
////      Send the data to beagle bone black
//        spi_data_write(rec_msg.data, 1);
//        received_data = spi_data_read();
//        UARTprintf("RX 2 - %x\n\r",received_data);
//        spi_data_write(packet, 1);
//
////      wait till valid control message is received from beagle bone green
//        control_message = spi_data_read();
//        UARTprintf("Control Message - %x\n\r",control_message);
//
//        decode_message(control_message);
////      send the message using queue to corresponding actuator.
////      Restore priority of task to original
////        vTaskPrioritySet(IBTaskHandle, 1);
    }
}

void decode_message(uint16_t ctrl_msg)
{
    LCDStruct dataToSend;
    uint8_t actual_msg= (ctrl_msg >> 8);
    dataToSend.task = 2;
    if((ctrl_msg & 0xff) == 0x55)
    {
        dataToSend.source=0x55;
        dataToSend.actuation_data=actual_msg;
        xQueueSend(LCDQueue, &dataToSend, pdMS_TO_TICKS(0));
        xTaskNotify(FanTaskHandle,actual_msg, eSetValueWithoutOverwrite);
        //Send the data to the fan actuator queue
    }
    else if((ctrl_msg & 0xff) == 0xaa)
    {
        dataToSend.source=0xaa;
        dataToSend.actuation_data=actual_msg;
        xQueueSend(LCDQueue, &dataToSend, pdMS_TO_TICKS(0));
        xTaskNotify(MotorTaskHandle,actual_msg, eSetValueWithoutOverwrite);
        //send the data to the motor actuator queue
    }
}


void test_function()
{
    uint32_t buffer;
    IBStruct rec_msg;
//    uint16_t received_data;
//    uint16_t control_message;
    int bytes_rec;
    static uint8_t state = STATE_SEND_TRID;

    switch(state)
    {
    case STATE_SEND_TRID:
        UARTprintf("In state 1\n");
        xQueueReceive(IBQueue, &rec_msg, portMAX_DELAY);
        packet = ((uint16_t)++trid<<8) | rec_msg.source;
        UARTprintf("source - packet - data is %x - %x - %x\n",rec_msg.source,packet,rec_msg.data);
        if(prev_state != state)
        {
            spi_data_write((uint16_t)packet, 1);
            prev_state = 1;
        }
        bytes_rec = SSIDataGetNonBlocking(SSI2_BASE, &buffer);
        UARTprintf("RX 1 - %x\n\r",buffer);
        if(bytes_rec != 0 && buffer == 0x01)
        {
            state = STATE_SEND_DATA;
        }
        else if(buffer == 0x02)
        {
            state = STATE_SEND_DATA;
        }
        else
        {
            UARTprintf("Bhai khud dekh lena control\n");
            self_control(rec_msg);
        }
        break;
    case STATE_SEND_DATA:
        UARTprintf("In state 2\n");
        if(prev_state != state)
        {
            spi_data_write(rec_msg.data, 1);
            prev_state = 2;
        }
        bytes_rec = SSIDataGetNonBlocking(SSI2_BASE, &buffer);
//        SSIDataGet(SSI2_BASE, &buffer);
        UARTprintf("RX 2 - %x\n\r",buffer);
        if(buffer == 0x02 && bytes_rec != 0)
        {
            state = STATE_GET_CTRL;
            spi_data_write(packet, 1);
        }
        else if(buffer == 0x01)
        {
            state = STATE_SEND_TRID;
        }
        else if(buffer == 0x02)
        {
            state = STATE_GET_CTRL;
        }
        else if(buffer != 0x01 || buffer != 0x02)
        {
            state = STATE_GET_CTRL;
        }
        break;
    case STATE_GET_CTRL:
        UARTprintf("In state 3\n");
        bytes_rec = SSIDataGetNonBlocking(SSI2_BASE, &buffer);
//        SSIDataGet(SSI2_BASE, &buffer);
        state = STATE_SEND_TRID;
        prev_state = 3;
        if(buffer == 0x01 || buffer == 0x02)
        {
            break;
        }
        UARTprintf("Control Message - %x\n\r",buffer);
        decode_message(buffer);
        //Send control message
        break;
    }
}


test_function1()
{
    uint32_t buffer;
    IBStruct rec_msg;
//    uint16_t received_data;
//    uint16_t control_message;
    int bytes_rec;
    static uint8_t state = STATE_SEND_TRID;

    switch(state)
    {
    case STATE_SEND_TRID:
        UARTprintf("In state 1\n");
        xQueueReceive(IBQueue, &rec_msg, portMAX_DELAY);
        packet = ((uint16_t)++trid<<8) | rec_msg.source;
        UARTprintf("source - packet - data is %x - %x - %x\n",rec_msg.source,packet,rec_msg.data);
        if(prev_state != state)
        {
            spi_data_write((uint16_t)packet, 1);
            prev_state = 1;
        }
        bytes_rec = SSIDataGetNonBlocking(SSI2_BASE, &buffer);
        UARTprintf("RX 1 - %x\n\r",buffer);
        if(bytes_rec != 0 && buffer == 0x01)
        {
            state = STATE_SEND_DATA;
        }
        else if(buffer == 0x02)
        {
            state = STATE_SEND_TRID;
        }
        else if(buffer != 0x01 | buffer != 0x02)
        {
            state = STATE_SEND_TRID;
        }
        else
        {
            UARTprintf("Bhai khud dekh lena control\n");
            self_control(rec_msg);
        }
        break;
    case STATE_SEND_DATA:
        UARTprintf("In state 2\n");
        if(prev_state != state)
        {
            spi_data_write(rec_msg.data, 1);
            prev_state = 2;
        }
        bytes_rec = SSIDataGetNonBlocking(SSI2_BASE, &buffer);
//        SSIDataGet(SSI2_BASE, &buffer);
        UARTprintf("RX 2 - %x\n\r",buffer);
        if(buffer == 0x02 && bytes_rec != 0)
        {
            state = STATE_GET_CTRL;
            spi_data_write(packet, 1);
        }
        else if(buffer == 0x01)
        {
            state = STATE_SEND_TRID;
        }
        else if(buffer == 0x02)
        {
            state = STATE_GET_CTRL;
        }
        else if(buffer != 0x01 || buffer != 0x02)
        {
            state = STATE_GET_CTRL;
        }
        break;
    case STATE_GET_CTRL:
        UARTprintf("In state 3\n");
        bytes_rec = SSIDataGetNonBlocking(SSI2_BASE, &buffer);
//        SSIDataGet(SSI2_BASE, &buffer);
        state = STATE_SEND_TRID;
        prev_state = 3;
        if(buffer == 0x01 || buffer == 0x02)
        {
            break;
        }
        UARTprintf("Control Message - %x\n\r",buffer);
        decode_message(buffer);
        //Send control message
        break;
    }
}


void self_control(IBStruct rec_msg)
{
    uint16_t control_msg_to_send;
    uint8_t source;
    uint16_t data;

    source = rec_msg.source;
    data   = rec_msg.data;
    switch(source)
    {
    case 0x55:
        if((data*0.25) > 30)
        {
            control_msg_to_send = 0x0100 | source;
        }
        else
        {
            control_msg_to_send = 0x0000 | source;
        }
        break;
    case 0xAA:
        if(data > 0x1aa)
        {
            control_msg_to_send = 0x0000 | source;
        }
        else
        {
            control_msg_to_send = 0x0100 | source;
        }
        break;
    }
    decode_message(control_msg_to_send);
}
