#ifndef __UART_CONNECTION_H
#define __UART_CONNECTION_H

#include "em_device.h"
#include "em_usart.h"
#include "em_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

struct UART_Settings{
	GPIO_Port_TypeDef uart_com_port;
	unsigned int uart_tx_pin;
	unsigned int uart_rx_pin;
	uint8_t uart_port_location;
	uint32_t uart_speed;
};

#define UART_BUFFER_SIZE	100

volatile struct messageBuffer{
	uint8_t data[UART_BUFFER_SIZE];
	uint32_t wrI;	//write index
	bool overflow;	//buffer overflow indicator
	bool ready;
} rxBuff;

void uart_init(struct UART_Settings uartSettings);

void uart_sendChar(char c);
void uart_sendText(char * text);

void clearRxBuffer();

#ifdef __cplusplus
}
#endif

#endif
