#ifndef __UART_CONNECTION_H
#define __UART_CONNECTION_H

#include "em_device.h"
#include "em_usart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define COM_PORT gpioPortD
#define TX_PIN   7
#define RX_PIN   6

#define UART_BUFFER_SIZE	100

volatile struct messageBuffer{
	uint8_t data[UART_BUFFER_SIZE];
	uint32_t wrI;	//write index
	bool overflow;	//buffer overflow indicator
	bool ready;
} rxBuff;

void initUART_9600  (void);
void initUART_baud(unsigned int baud);
void uart_sendChar(char c);
void uart_sendText(char * text);

void clearRxBuffer();

#ifdef __cplusplus
}
#endif

#endif
