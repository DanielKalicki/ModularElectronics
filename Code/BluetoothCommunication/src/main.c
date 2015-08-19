#include "em_device.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_i2c.h"
#include "em_emu.h"
#include "em_rtc.h"
#include "em_cmu.h"

#include "ucPeripheralDrivers\leuart_connection.h"
#include "ucPeripheralDrivers\RTC_.h"
#include "icDrivers\HM11.h"

#define REGISTER_SIZE 50
volatile uint8_t registers[REGISTER_SIZE];

enum registerMap
{
    REG_DATA_LENGTH = 0,

    REG_MODULE_ID_1 = 1,
    REG_MODULE_ID_2 = 2,
    REG_MODULE_ID_3 = 3
};

void initOscillators(void)
{
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2); /* select HFCORECLK/2 as clock source to LFB */
    CMU_ClockEnable(cmuClock_CORELE, true);                 /* enable the Low Energy Peripheral Interface clock */

    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);                    /* enable GPIO peripheral clock */
}

void clockTest()
{
    long int i = 0;
    for (; i < 120001L; ++i)
    {
        if (i == 120000L)
        {
            GPIO_PortOutSet(gpioPortF, 0x4);
        }
        if (i == 60000L)
        {
            GPIO_PortOutClear(gpioPortF, 0x4);
        }
    }
}
void clockTest_short()
{
    long int i = 0;
    for (; i < 121L; ++i)
    {
        if (i == 120L)
        {
            GPIO_PortOutSet(gpioPortF, 0x4);
        }
        if (i == 60L)
        {
            GPIO_PortOutClear(gpioPortF, 0x4);
        }
    }
}

/* -------------RTC--------------*/

int RTC_interrupt = 0;
void RTC_IRQHandler(void)
{



    clockTest_short();  clockTest_short();  clockTest_short();  clockTest_short();  clockTest_short();  clockTest_short();  clockTest_short();

    Rtc_ClearInt();
}

void initInterfaces()
{
#ifdef DEBUG
    /* LeUart interface initialization */
    struct LeUart_Settings leuartSettings;
    leuartSettings.leuart_com_port = gpioPortF;
    leuartSettings.leuart_tx_pin = 0;
    leuartSettings.leuart_rx_pin = 1;
    leuartSettings.leuart_port_location = 3;
    leuartSettings.leuart_baudrate = 115200;
    LeUart_Init(leuartSettings);
    LeUart_SendText("Startup\n");
#endif

    /* UART interface initialization */
    struct UART_Settings uartSettings;
    uartSettings.uart_com_port = gpioPortD;
    uartSettings.uart_tx_pin = 7;
    uartSettings.uart_rx_pin = 6;
    uartSettings.uart_port_location = 2;
    uartSettings.uart_baudrate = 115200;

    Uart_Init(uartSettings, Uart_RxValidate_Terminator);
}

/*-------------MAIN-------------*/
int main(void)
{
    CHIP_Init();
    initOscillators();

    initInterfaces();

    /* RTC initialization */
    Rtc_Init();
    Rtc_SetTime(500);
    Rtc_EnableInt();

    uint8_t initHM11_fail = 1;
    while (initHM11_fail)
    {
        initHM11_fail = HM11_Init();
        clockTest();
    }

    while (1)
    {
        EMU_EnterEM1();
    }
}
