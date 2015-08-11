#include "leuart_connection.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"


void LeUart_Init(struct LeUart_Settings LeUartSettings)
{
	//Oscillators
	CMU_ClockEnable(cmuClock_LEUART0, true);                 // enable LEUART peripheral clock

	GPIO_PinModeSet(LeUartSettings.leuart_com_port, LeUartSettings.leuart_tx_pin, gpioModePushPull, 1); // set TX pin to push-pull output, initialize high (otherwise glitches can occur)
	GPIO_PinModeSet(LeUartSettings.leuart_com_port, LeUartSettings.leuart_rx_pin, gpioModeInput,    0);    // set RX pin as input (no filter)

	LEUART_TypeDef *leuart = LEUART0;
	LEUART_Init_TypeDef leuartInit = LEUART_INIT_DEFAULT;
	leuartInit.baudrate = LeUartSettings.leuart_baudrate;

	LEUART0->ROUTE = LEUART_ROUTE_TXPEN | LEUART_ROUTE_TXPEN | (LeUartSettings.leuart_port_location << _LEUART_ROUTE_LOCATION_SHIFT);

	LEUART_Init(leuart , &leuartInit);
}

void LeUart_SendChar(char c)
{
	LEUART_Tx(LEUART0, c);
}
void LeUart_SendText(char * text)
{
	int i = 0;
	uint8_t len = 150;
	for (;i < len; ++i){
		if(text[i]==0) break;
		LeUart_SendChar(text[i]);
	}
	/* Check that transmit buffer is empty */
	while (!(LEUART0->STATUS & LEUART_STATUS_TXBL))
	  ;
}
