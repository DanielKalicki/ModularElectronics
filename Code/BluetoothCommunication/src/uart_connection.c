#include "uart_connection.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"

//volatile struct messageBuffer rxBuff={ {0}, 0, false };

void initUART_9600(void){
	//Oscillators
	CMU_ClockEnable(cmuClock_USART1, true);                 // enable USART1 peripheral clock

	//GPIOs
	GPIO_PinModeSet(COM_PORT, TX_PIN, gpioModePushPull, 1); // set TX pin to push-pull output, initialize high (otherwise glitches can occur)
	GPIO_PinModeSet(COM_PORT, RX_PIN, gpioModeInput, 0);    // set RX pin as input (no filter)

	USART_InitAsync_TypeDef uartInit =
	  {	//230400
	    .enable = usartDisable,     // wait to enable transmitter and receiver
	    .refFreq = 0,               // setting refFreq to 0 will invoke the CMU_ClockFreqGet() function and measure the HFPER clock
	    .baudrate = 9600,	        // desired baud rate
	    .oversampling = usartOVS16, // set oversampling to x16
	    .databits = usartDatabits8, // 8 data bits
	    .parity = usartNoParity,    // no parity bits
	    .stopbits = usartStopbits1, // 1 stop bit
	    .mvdis = false,             // use majority voting
	    .prsRxEnable = false,       // not using PRS input
	    .prsRxCh = usartPrsRxCh0,   // doesn't matter what channel we select
	 };
	 USART_InitAsync(USART1, &uartInit);      // apply configuration to USART1
	 USART1->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC2; // clear buffers, enable transmitter and receiver pins

	 USART_IntClear(USART1, _USART_IF_MASK);
	 USART_IntEnable(USART1, USART_IF_RXDATAV);
	 NVIC_ClearPendingIRQ(USART1_RX_IRQn);
	 NVIC_ClearPendingIRQ(USART1_TX_IRQn);
	 NVIC_EnableIRQ(USART1_RX_IRQn);
	 NVIC_EnableIRQ(USART1_TX_IRQn);

	 USART_Enable(USART1, usartEnable);       // enable transmitter and receiver
}

void initUART_baud(unsigned int baud){
	//Oscillators
	CMU_ClockEnable(cmuClock_USART1, true);                 // enable USART1 peripheral clock

	//GPIOs
	GPIO_PinModeSet(COM_PORT, TX_PIN, gpioModePushPull, 1); // set TX pin to push-pull output, initialize high (otherwise glitches can occur)
	GPIO_PinModeSet(COM_PORT, RX_PIN, gpioModeInput, 0);    // set RX pin as input (no filter)

	USART_InitAsync_TypeDef uartInit =
	  {	//230400
	    .enable = usartDisable,     // wait to enable transmitter and receiver
	    .refFreq = 0,               // setting refFreq to 0 will invoke the CMU_ClockFreqGet() function and measure the HFPER clock
	    .baudrate = baud,	        // desired baud rate
	    .oversampling = usartOVS16, // set oversampling to x16
	    .databits = usartDatabits8, // 8 data bits
	    .parity = usartNoParity,    // no parity bits
	    .stopbits = usartStopbits1, // 1 stop bit
	    .mvdis = false,             // use majority voting
	    .prsRxEnable = false,       // not using PRS input
	    .prsRxCh = usartPrsRxCh0,   // doesn't matter what channel we select
	 };
	 USART_InitAsync(USART1, &uartInit);      // apply configuration to USART1
	 USART1->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC2; // clear buffers, enable transmitter and receiver pins

	 USART_IntClear(USART1, _USART_IF_MASK);
	 USART_IntEnable(USART1, USART_IF_RXDATAV);
	 NVIC_ClearPendingIRQ(USART1_RX_IRQn);
	 NVIC_ClearPendingIRQ(USART1_TX_IRQn);
	 NVIC_EnableIRQ(USART1_RX_IRQn);
	 NVIC_EnableIRQ(USART1_TX_IRQn);

	 USART_Enable(USART1, usartEnable);       // enable transmitter and receiver
}

void uart_sendChar(char c){
	while(!(USART1->STATUS & (1 << 6)));   // wait for TX buffer to empty
	USART1->TXDATA = c; 			       // send character

	// this function can be replaced with USART_Tx();
}
void uart_sendText(char * text){
	int i=0;
	uint8_t len=150;
	for (;i<len;++i){
		if(text[i]==0) break;
		while(!(USART1->STATUS & (1 << 6)));   // wait for TX buffer to empty
		USART1->TXDATA = text[i];       		// send character
	}
}

void USART1_RX_IRQHandler(void) {
  //uart_sendChar('I');
  /* Check for RX data valid interrupt */
  if (USART1->STATUS & USART_STATUS_RXDATAV) {
	//buffer is NOT full
		/* Copy data into RX Buffer */
	uint8_t rxData = USART_Rx(USART1);
	if( !rxBuff.overflow ){
		rxBuff.data[rxBuff.wrI] = rxData;
		rxBuff.wrI++;
		if(rxData==0){
			rxBuff.ready=true;
		}

		/* Flag Rx overflow */
		if ( rxBuff.wrI >= UART_BUFFER_SIZE){
		  rxBuff.overflow = true;
		}
	}

    /* Clear RXDATAV interrupt */
    USART_IntClear(USART1, USART_IF_RXDATAV);
  }
}

void clearRxBuffer(){
	rxBuff.wrI=0;
	rxBuff.ready=false;
	rxBuff.overflow=false;
}
