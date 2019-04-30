/* FreeRTOS 8.2 Tiva Demo
 *
 * main.c
 * Author: Nachiket Kelkar and Puneet Bansal
 * Reference: The project is based on the freeRTOS example code on
 * https://github.com/akobyl/TM4C129_FreeRTOS_Demo/blob/master/main.c
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "inc/hw_memmap.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "pwm.h"

#include "src/spi.h"
#include "src/LCDdriver.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "src/Logger.h"
#include "src/sensor.h"

/*Additions*/
#include "src/actuator.h"

#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

// Demo Task declarations
void demoLEDTask(void *pvParameters);
void demoSerialTask(void *pvParameters);

uint32_t g_ui32SysClock;

void motor_control_init();
void motor_control_config(uint32_t period_in_khz, uint8_t duty_cycle);
void TXFF_interrupt();
void TestCallback();

TaskHandle_t TempTaskHandle;
TaskHandle_t SMTaskHandle;
TaskHandle_t IBTaskHandle;
TaskHandle_t LCDTaskHandle;
TaskHandle_t MotorTaskHandle;
TaskHandle_t FanTaskHandle;


// Main function
int main(void)
{
    // Initialize system clock to 120 MHz
    g_ui32SysClock = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                                SYSTEM_CLOCK);

    Logger_Init();
    UARTStdioConfig(0, 115200, g_ui32SysClock);

    UARTprintf("Creating tasks\n");

    // Creating all the required task
    xTaskCreate(TemperatureTask, "Temperature", 256, NULL, 1, &TempTaskHandle);
    xTaskCreate(SoilMoistureTask, "Moisture", 256, NULL, 1, &SMTaskHandle);
    xTaskCreate(InterBoardSPI, "InterBoardCom", 256, NULL, 1, &IBTaskHandle);
    xTaskCreate(LCDTask, "LCDTask", 256, NULL, 1, &LCDTaskHandle);
    xTaskCreate(MotorTask, "MotorTask", 256, NULL, 1, &MotorTaskHandle);
    xTaskCreate(FanTask, "FanTask", 256, NULL, 1, &FanTaskHandle);


    vTaskStartScheduler();
    UARTprintf("I should not have come here\n");
}

/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
