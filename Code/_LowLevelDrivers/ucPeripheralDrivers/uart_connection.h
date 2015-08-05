#ifndef __UART_CONNECTION_H
#define __UART_CONNECTION_H

#include "em_device.h"
#include "em_usart.h"
#include "em_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UART_RX_IRQ

/* 							!!!							  */
/* To use UART RX you need to define UART_RX_IRQ in spi.h */
/* 							!!!							  */

struct UART_Settings{
	GPIO_Port_TypeDef uart_com_port;
	unsigned int uart_tx_pin;
	unsigned int uart_rx_pin;
	uint8_t uart_port_location;
	uint32_t uart_baudrate;
};

#define UART_BUFFER_SIZE	400

volatile struct messageBuffer{
	uint8_t data[UART_BUFFER_SIZE];
	uint32_t wrI;	//write index
	bool overflow;	//buffer overflow indicator
	bool ready;
} rxBuff;

#define UART_MESSAGE_TERMINATOR	'\n'

#define UART_RX_VALIDATE_FUNC
#define UART_RX_VALIDATE_RESULT_FINISHED 		1
#define UART_RX_VALIDATE_RESULT_NOT_FINISHED	0

void uart_init(struct UART_Settings uartSettings
#ifdef UART_RX_VALIDATE_FUNC
		, uint8_t (*Uart_RxFunctionPointer)(void)
#endif
);

void uart_sendChar(char c);
void uart_sendText(char * text);

void clearRxBuffer();

#ifdef __cplusplus
}
#endif

#endif
