#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_emu.h"

#define DEBUG

#include "ucPeripheralDrivers\uart_connection.h"
#include "ucPeripheralDrivers\RTC_.h"
#include "ucPeripheralDrivers\leuart_connection.h"

#include "icDrivers\FPGMMOPA6C.h"

#include <stdio.h>

void initOscillators(void)
{
	  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);  // select HFCORECLK/2 as clock source to LFB
	  CMU_ClockEnable(cmuClock_CORELE, true);                  // enable the Low Energy Peripheral Interface clock

	  CMU_ClockEnable(cmuClock_HFPER, true);
	  CMU_ClockEnable(cmuClock_GPIO, true);                    // enable GPIO peripheral clock
}

void clockTest_short() {
	long int i=0;
	for(;i<12001L;++i) {
	  if(i==12000L)
		  GPIO_PortOutSet(gpioPortF, 0x4);
	  if(i==6000L)
		  GPIO_PortOutClear(gpioPortF, 0x4);
	}
}

void debug_wait()
{
	for (int i = 0; i < 20; i++)
	{
		clockTest_short();
	}
}

uint8_t Uart_RxValidate_DoNothing(void)
{
	return UART_RX_VALIDATE_RESULT_FINISHED;
}

void init_uart_interface(void){
	/* UART interface initialization */
	struct UART_Settings uartSettings;
	uartSettings.uart_com_port = gpioPortD;
	uartSettings.uart_tx_pin = 7;
	uartSettings.uart_rx_pin = 6;
	uartSettings.uart_port_location = 3;
	uartSettings.uart_baudrate = 9600;

	uart_init(uartSettings, Uart_RxValidate_GpsMessage);
}

#ifdef NOT_DEFINE
void init_uart_interface_for_debuging(void){
	/* UART interface initialization */
	struct UART_Settings uartSettings;
	uartSettings.uart_com_port=gpioPortC;
	uartSettings.uart_tx_pin=0;
	uartSettings.uart_rx_pin=1;
	uartSettings.uart_port_location=0;
	uartSettings.uart_baudrate=115200;

	uart_init(uartSettings, Uart_RxValidate_DoNothing);
}
#endif
void init_leuart_interface(void){
	/* UART interface initialization */
	struct LeUart_Settings leuartSettings;
	leuartSettings.leuart_com_port=gpioPortF;
	leuartSettings.leuart_tx_pin=0;
	leuartSettings.leuart_rx_pin=1;
	leuartSettings.leuart_port_location=3;
	leuartSettings.leuart_baudrate=115200;

	LeUart_Init(leuartSettings);
}
#ifdef DEBUG

#endif

void RTC_IRQHandler(void)
{
#ifdef DEBUG
	/* setup uart to send commands to debug interface */
	//init_uart_interface_for_debuging();

	/* send character indicating RTC time interrupt */
	LeUart_SendChar('r');
	LeUart_SendChar('\n');

	char buff[30];
	sprintf(buff, "Rx: %d\n", rxBuff.wrI);
	LeUart_SendText(buff);

	if (true == rxBuff.overflow)
	{
		LeUart_SendText("Overflow\n");
		for (int i = 0; i < rxBuff.wrI; i++)
		{
			LeUart_SendChar(rxBuff.data[i]);
		}
		RTC_setTime(10000);
	}

	/* transmit received data from gps module */
	if (true == rxBuff.ready)
	{
		for (int i = 0; i < rxBuff.wrI; i++)
		{
			LeUart_SendChar(rxBuff.data[i]);
		}
		LeUart_SendChar('\n');
		clearRxBuffer();
	}

	/* wait */
	//debug_wait();

	/* setup uart to listen for commands from gps module */
	//init_uart_interface();

	/* wait */
	debug_wait();

#endif
	RTC_clearInt();
}


int main(void)
{
	CHIP_Init();
	initOscillators();

	//init_uart_interface();
#ifdef DEBUG
	//init_uart_interface_for_debuging();
	//uart_sendText("GPS module Startup\n");
	init_leuart_interface();
	LeUart_SendText("GPS module Startup\n");
#endif

	//RTC initialization
	RTC_init();
	RTC_setTime(1000);
	RTC_enableInt();
	while(1)
	{
		EMU_EnterEM1();
	}
}
