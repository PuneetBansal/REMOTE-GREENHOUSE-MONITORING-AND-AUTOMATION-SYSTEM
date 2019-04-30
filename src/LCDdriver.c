/*
 * LCDdriver.c
 *
 *  Created on: Apr 15, 2019
 *      Author: nachiket kelkar & puneet bansal
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "LCDdriver.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

void lcd_init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP))
    {
    }

    /* Configure pins as output */
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4);
}


void lcd_write_data(char data)
{
    /* Write data on pins for LCD */
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_0, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_1, 0);

    GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0, (int)data);
    latch_data();
}


void lcd_write_string(char* data)
{
    int len,i = 0;
    len = strlen(data);
    while(i != len)
    {
        lcd_write_data(data[i++]);
    }
}


void lcd_write_command(uint8_t command)
{
    /* Write data on pins for LCD */
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_0, 0);
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_1, 0);

    GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0, command);
    latch_data();
}


void latch_data()
{
    /* Write data on pins for LCD */
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_4, GPIO_PIN_4);
    delay(1000);
    GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_4, 0);
}


void lcd_pos(uint8_t row, uint8_t position)
{
    uint8_t command;
    switch(row)
    {
    case 0:
        command = 0x80 | position;
        break;
    case 1:
        command = 0x80 | (64+position);
        break;
    case 2:
        command = 0x80 | (16+position);
        break;
    case 3:
        command = 0x80 | (80+position);
    }
    lcd_write_command(command);
}


void delay(uint16_t x)
{
    int i = 0;
    for(i=0; i< x; i++);
    for(i=0; i< x; i++);
    for(i=0; i< x; i++);
    for(i=0; i< x; i++);
}


void lcd_on()
{
    lcd_write_command(0x30);
    lcd_write_command(0x30);
    lcd_write_command(0x30);

    SYSTEM_SET;
    DISPLAY_OFF;
    DISPLAY_ON;
    ENTRY_MODE;
    CLEAR_DISPLAY;
    delay(4000);
    CURSOR_HOME;
    delay(4000);
}

void lcd_print_digit(long no)
{
    char buffer[10];
    ltoa(no, buffer);
    lcd_write_string(buffer);
}

void lcd_print_float(float no)
{
    lcd_print_digit(no);
    no = no - (long)no;
    no = no * 1000;
    lcd_write_data('.');
    lcd_print_digit(no);
}
