#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_emu.h"

#define DEBUG

#include "ucPeripheralDrivers\uart_connection.h"
#include "ucPeripheralDrivers\i2c_connection.h"
#include "ucPeripheralDrivers\RTC_.h"

#include "icDrivers\ADP1650.h"

#include <stdio.h>

void clockTest_short() {
	long int i=0;
	for(;i<12001L;++i) {
	  if(i==12000L)
		  GPIO_PortOutSet(gpioPortF, 0x8);
	  if(i==6000L)
		  GPIO_PortOutClear(gpioPortF, 0x8);
	}
}

void initOscillators(void)
{
	  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);  // select HFCORECLK/2 as clock source to LFB
	  CMU_ClockEnable(cmuClock_CORELE, true);                  // enable the Low Energy Peripheral Interface clock

	  CMU_ClockEnable(cmuClock_HFPER, true);
	  CMU_ClockEnable(cmuClock_GPIO, true);                    // enable GPIO peripheral clock
}

void init_uart_interface(void){
	/* UART interface initialization */
	struct UART_Settings uartSettings;
	uartSettings.uart_com_port = gpioPortC;
	uartSettings.uart_tx_pin = 0;
	uartSettings.uart_rx_pin = 1;
	uartSettings.uart_port_location = 0;
	uartSettings.uart_speed = 115200;

	uart_init(uartSettings);

#ifdef DEBUG
	uart_sendText("Led Torch module startup\n");
#endif
}

void init_i2c_interface(void)
{
	//i2c initialization
	struct I2C_Settings i2cSettings;
	i2cSettings.i2c_SCL_port = gpioPortD;
	i2cSettings.i2c_SCL_pin =  7;
	i2cSettings.i2c_SDA_port = gpioPortD;
	i2cSettings.i2c_SDA_pin =  6;
	i2cSettings.i2c_port_location =  1;
	i2c_MasterInit(i2cSettings);
#ifdef DEBUG
	i2c_Scan(I2C0);
#endif
}

//TODO read errors from AD1650

void RTC_IRQHandler(void)
{

	/*static uint8_t counter = 1;
	if (counter == 1)
	{
		ADP1650_TorchLedOn();
	}
	else if (counter == 10)
	{
		ADP1650_TorchLedOff();
	}
	else if (counter == 15)
	{
		counter = 0;
	}
	counter++;*/

	uart_sendChar('r');

	ADP1650_PrintRegisters();

	for (int i = 0; i < 10; i++){
		clockTest_short();
	}

	/* Clear RTC interrupts */
	RTC_clearInt();
}

int main(void)
{
	CHIP_Init();
	initOscillators();

	/* UART initialization */
	init_uart_interface();

	/* I2C initialization */
	init_i2c_interface();

	/* RTC initialization */
	RTC_init();
	RTC_setTime(500);
	RTC_enableInt();

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

	while(1)
	{
	  EMU_EnterEM2(false);
	}
}
