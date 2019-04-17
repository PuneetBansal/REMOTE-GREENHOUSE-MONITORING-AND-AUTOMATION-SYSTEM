/* FreeRTOS 8.2 Tiva Demo
 *
 * main.c
 * Reference: The project is based on the freeRTOS example code on https://github.com/akobyl/TM4C129_FreeRTOS_Demo/blob/master/main.c
 */

#include <stdint.h>
#include <stdbool.h>
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

#include "src/spi.h"
#include "src/LCDdriver.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"

#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

// Demo Task declarations
void demoLEDTask(void *pvParameters);
void demoSerialTask(void *pvParameters);

uint32_t g_ui32SysClock;

void motor_control_init();
void motor_control_config(uint32_t period_in_khz, uint8_t duty_cycle);

// Main function
int main(void)
{
    // Initialize system clock to 120 MHz
    g_ui32SysClock = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                                SYSTEM_CLOCK);
//    SYSTEM_CLOCK
//    SPI testing
//    spi_init(MASTER, 250000);


//    LCD testing
//    lcd_init();
//    lcd_on();
//    lcd_write_string("Temp  : 23.576");
//    lcd_pos(1, 0);
//    lcd_write_string("Mois  : 56%");
//    lcd_pos(2, 0);
//    lcd_write_string("Fspeed: 10%");
//    lcd_pos(3, 0);
//    lcd_write_string("Motor : ON");
//    lcd_print_float(23.56);

//    PWM test
    motor_control_init();
    motor_control_config(500, 25);
    return 0;
}


void motor_control_init()
{
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
}



void motor_control_config(uint32_t period_in_khz, uint8_t duty_cycle)
{
    uint64_t N;
    uint32_t width;
    N = 120000/period_in_khz;
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, N);
    width = duty_cycle / 100;
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, PWMGenPeriodGet(PWM0_BASE, PWM_GEN_0) * width);
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}


// Flash the LEDs on the launchpad
void demoLEDTask(void *pvParameters)
{
    for (;;)
    {
        // Turn on LED 1
        LEDWrite(0x0F, 0x01);
        vTaskDelay(1000);

        // Turn on LED 2
        LEDWrite(0x0F, 0x02);
        vTaskDelay(1000);

        // Turn on LED 3
        LEDWrite(0x0F, 0x04);
        vTaskDelay(1000);

        // Turn on LED 4
        LEDWrite(0x0F, 0x08);
        vTaskDelay(1000);
    }
}


// Write text over the Stellaris debug interface UART port
void demoSerialTask(void *pvParameters)
{
    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, 57600, SYSTEM_CLOCK);


    for (;;)
    {
        UARTprintf("\r\nHello, world from FreeRTOS 9.0!");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
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
