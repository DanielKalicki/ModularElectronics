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

void initUART(void);
void uart_sendChar(char c);
void uart_sendText(char * text);

#ifdef __cplusplus
}
#endif

#endif
