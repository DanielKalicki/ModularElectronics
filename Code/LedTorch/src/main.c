#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_emu.h"

#include "ucPeripheralDrivers\leuart_connection.h"
#include "ucPeripheralDrivers\i2c_connection.h"
#include "ucPeripheralDrivers\RTC_.h"

#include "communicationDrivers\mod_comm.h"

#include "icDrivers\ADP1650.h"

#include <stdio.h>

void clockTest_short()
{
    long int i = 0;
    for (; i < 12001L; ++i)
    {
        if (i == 12000L)
        {
            GPIO_PortOutSet(gpioPortF, 0x8);
        }
        if (i == 6000L)
        {
            GPIO_PortOutClear(gpioPortF, 0x8);
        }
    }
}

void initOscillators(void)
{
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);  // select HFCORECLK/2 as clock source to LFB
    CMU_ClockEnable(cmuClock_CORELE, true);                  // enable the Low Energy Peripheral Interface clock

    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);                    // enable GPIO peripheral clock
}

void init_uart_interface(void)
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

#ifdef DEBUG
    LeUart_SendText("Led Torch module startup\n");
#endif
}

void init_i2c_interface(void)
{
    //i2c initialization
    struct I2C_Settings i2cSettings;
    i2cSettings.i2c_SCL_port = gpioPortD;
    i2cSettings.i2c_SCL_pin = 7;
    i2cSettings.i2c_SDA_port = gpioPortD;
    i2cSettings.i2c_SDA_pin = 6;
    i2cSettings.i2c_port_location = 1;
    i2c_InitMaster(i2cSettings);
#ifdef DEBUG
    i2c_Scan(I2C0);
#endif
}

uint8_t id = 0xC3;
uint8_t message[9] = {0xAA, 0xAA, 0xAA, 0x00, 0xFF, 0xAA, 0x00, 0xAA, 0xFF};
uint8_t crc[4] = {0xFF, 0x55, 0x66, 0x77};

void RTC_IRQHandler(void)
{
    //TODO read errors from AD1650
    ADP1650_TorchLedOff();

    LeUart_SendChar('r');

    ADP1650_PrintRegisters();

    //ModComm_ReceiveBroadcasts();

    for (int i = 0; i < 10; i++)
    {
        clockTest_short();
    }

    ModComm_Broadcast(id, &message[0], 9, &crc[0]);

    /* Clear RTC interrupts */
    Rtc_ClearInt();
}

/* -------------GPIO IRQ -----------*/
void GPIO_EVEN_IRQHandler(void)
{
    ModComm_GPIO_IRQ();
    GPIO_IntClear(0xFFFF);
}

void GPIO_ODD_IRQHandler(void)
{
    ModComm_GPIO_IRQ();
    GPIO_IntClear(0xFFFF);
}

/* -------------MAIN -----------*/
int main(void)
{
    CHIP_Init();
    initOscillators();

    /* UART initialization */
    init_uart_interface();

    /* I2C initialization */
    init_i2c_interface();

    /* Led Torch ic initialization */
    ADP1650_GPIO_Settings_t ADP1650_uC_ConnectionPins;
    ADP1650_uC_ConnectionPins.gpio1_pin = 14;
    ADP1650_uC_ConnectionPins.gpio1_port = gpioPortC;
    ADP1650_uC_ConnectionPins.gpio1_state = ADP160_UC_PIN_OUTPUT;
    ADP1650_uC_ConnectionPins.gpio2_pin = 15;
    ADP1650_uC_ConnectionPins.gpio2_port = gpioPortC;
    ADP1650_uC_ConnectionPins.gpio2_state = ADP160_UC_PIN_OUTPUT;
    ADP1650_uC_ConnectionPins.strobe_pin = 2;
    ADP1650_uC_ConnectionPins.strobe_port = gpioPortF;
    ADP1650_uC_ConnectionPins.strobe_state = ADP160_UC_PIN_OUTPUT;
    ADP1650_Init(ADP1650_uC_ConnectionPins);

    ADP1650_TorchLedOn();

    ModComm_Settings_t modComm_Settings;
    modComm_Settings.data_pin  = 0;
    modComm_Settings.data_port = gpioPortC;
    modComm_Settings.clk_pin  = 1;
    modComm_Settings.clk_port = gpioPortC;
    modComm_Settings.busy_pin  = 7;
    modComm_Settings.busy_port = gpioPortB;
    ModComm_Init(modComm_Settings, false);
#ifdef DEBUG
    LeUart_SendText("ModComm Initialization\n");
#endif

    /* RTC initialization */
    Rtc_Init();
    Rtc_SetTime(500);
    Rtc_EnableInt();

    while (1)
    {
        if (ModComm_Progress()){
            EMU_EnterEM1();
        }
        else {
            EMU_EnterEM2(false);
        }
    }
}
