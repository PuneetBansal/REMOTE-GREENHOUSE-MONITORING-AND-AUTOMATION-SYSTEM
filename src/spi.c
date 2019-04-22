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
#include "spi.h"

extern uint32_t g_ui32SysClock;

void spi_init(uint32_t mode, uint32_t clk_speed)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
//    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

    GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);
    GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);
    GPIOPinConfigure(GPIO_PD2_SSI2FSS);
    GPIOPinConfigure(GPIO_PD3_SSI2CLK);
//    GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);
//    GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
//    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
//    GPIOPinConfigure(GPIO_PA2_SSI0CLK);

    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
//    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

//    SSIClockSourceSet(SSI2_BASE, SSI_CLOCK_SYSTEM);
    SSIConfigSetExpClk(SSI2_BASE, g_ui32SysClock, SSI_FRF_MOTO_MODE_0, mode, clk_speed, 8);
    SSIEnable(SSI2_BASE);
}

void spi_data_write(uint64_t data_to_write, uint8_t no_of_bytes)
{
    int i=0;
    for(i=0; i < no_of_bytes; i++)
    {
        SSIDataPut(SSI2_BASE, data_to_write);
        data_to_write = data_to_write >> 8;
    }
}

uint8_t spi_data_read()
{
    uint32_t buffer;
    SSIDataGet(SSI2_BASE, &buffer);
    return (uint8_t)buffer;
}


