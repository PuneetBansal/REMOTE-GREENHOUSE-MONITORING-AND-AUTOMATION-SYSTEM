/* FreeRTOS 8.2 Tiva Demo
 *
 * main.c
 * Reference: The project is based on the freeRTOS example code on https://github.com/akobyl/TM4C129_FreeRTOS_Demo/blob/master/main.c
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
void TestCallback();

TaskHandle_t TempTaskHandle;
TaskHandle_t SMTaskHandle;
TaskHandle_t IBTaskHandle;

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

    vTaskStartScheduler();
    UARTprintf("I should not have come here\n");

//    SYSTEM_CLOCK
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
//      SPI testing
//    static uint8_t trid;
//    int i;
//    uint16_t packet;
//    uint8_t source1 = 0x30;
//    uint8_t source2 = 0x03;
//    uint16_t data;
//    uint16_t value = 0xcc55;
//    spi_init(SLAVE, 500000);
//    spi_data_write(0x0011, 1);
//    UARTprintf("Going inside while\n");
//    while(1)
//    {
//        packet = (uint16_t)++trid<<8 | source1;
//        spi_data_write(packet, 1);
//        data = spi_data_read();
//        UARTprintf("RX 1 - %x\n\r",data);
//        spi_data_write(value, 1);
//        data = spi_data_read();
//        UARTprintf("RX 2 - %x\n\r",data);
//        spi_data_write(packet, 1);
//        for(i = 0; i<10000; i++);
//        for(i = 0; i<10000; i++);
//        for(i = 0; i<10000; i++);
//
//        packet = (uint16_t)++trid<<8 | source2;
//        spi_data_write(packet, 1);
//        data = spi_data_read();
//        UARTprintf("RX 1 - %x\n\r",data);
//        spi_data_write(0xcc66, 1);
//        data = spi_data_read();
//        UARTprintf("RX 2 - %x\n\r",data);
//        spi_data_write(packet, 1);
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

//  Temperature Sensor testing
//    char tempbuffer[10];
//    uint16_t buffer;
//    uint32_t i;
//    float temp;
//    spi_init(MASTER, 100000);
//    while(1)
//    {
//        spi_data_write(0x1234,1);
//        buffer = spi_data_read();
//        temp = temperature(buffer);
//        UARTprintf("D2 is %d\n",((buffer>>2) && 0x01));
//        sprintf(tempbuffer,"%f",temp);
//        UARTprintf("Temperature is %s\n",tempbuffer);
//        for(i=0; i<1000000; i++);
//        for(i=0; i<1000000; i++);
//        for(i=0; i<1000000; i++);
//        for(i=0; i<1000000; i++);
//        for(i=0; i<1000000; i++);
//    }
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
