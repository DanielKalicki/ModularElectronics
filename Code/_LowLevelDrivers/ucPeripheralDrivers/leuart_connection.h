#ifndef LEUART_CONNECTION_H_
#define LEUART_CONNECTION_H_

#include "em_device.h"
#include "em_leuart.h"
#include "em_gpio.h"

struct LeUart_Settings{
	GPIO_Port_TypeDef leuart_com_port;
	unsigned int leuart_tx_pin;
	unsigned int leuart_rx_pin;
	uint8_t leuart_port_location;
	uint32_t leuart_baudrate;
};
void LeUart_Init(struct LeUart_Settings LeUartSettings);

void LeUart_SendChar(char c);
void LeUart_SendText(char * text);

#endif /* LEUART_CONNECTION_H_ */
