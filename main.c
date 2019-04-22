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
#include "src/Logger.h"
#include "src/sensor.h"

#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

// Demo Task declarations
void demoLEDTask(void *pvParameters);
void demoSerialTask(void *pvParameters);

uint32_t g_ui32SysClock;

void motor_control_init();
void motor_control_config(uint32_t period_in_khz, uint8_t duty_cycle);
void TXFF_interrupt();

TaskHandle_t TempTaskHandle;
TaskHandle_t SMTaskHandle;

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

    // Creating all the required task
    xTaskCreate(TemperatureTask, "Temperature", 256, NULL, 1, TempTaskHandle);
    xTaskCreate(SoilMoistureTask, "Moisture", 256, NULL, 1, SMTaskHandle);

//    SYSTEM_CLOCK
//    SPI testing
//    int x = 5;
//    uint8_t data;
    spi_init(SLAVE, 500000);
//    spi_data_write(x++, 1);
//    int buffer[6] = {0x05,0x06,0x09,0xab,0x55,0x45};
//    SSIIntRegister(SSI2_BASE, TXFF_interrupt);
//    SSIIntClear(SSI2_BASE, SSI_TXEOT);
//    SSIIntEnable(SSI2_BASE, SSI_TXEOT);

//    ADC testing
//    uint32_t adc_value;
//    adc_init();
//    while(1)
//    {
//        adc_value = adc_get_data();
//        UARTprintf("adc_value = %d\n\r",adc_value);
//    }
//    while(1)
//    {
//            send_data = false;
//            spi_data_write(x, 1);
//            data = spi_data_read();
//            UARTprintf("data is %x",data);
//    }


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
//    motor_control_init();
//    motor_control_config(500, 25);
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


void TXFF_interrupt()
{
    SSIIntClear(SSI2_BASE, SSI_TXEOT);
    SSIIntDisable(SSI2_BASE, SSI_TXEOT);
    UARTprintf("Interrupt happened");
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
