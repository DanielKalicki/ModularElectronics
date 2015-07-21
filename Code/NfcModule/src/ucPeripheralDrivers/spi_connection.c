/*
 * spi_connection.c
 *
 *  Created on: 20-07-2015
 *      Author: terg
 */


#include "spi_connection.h"
#include "em_usart.h"
#include "spi.h"

static GPIO_Port_TypeDef cs_port;
static unsigned int cs_pin;

void spi_init(struct SPI_Settings spiSettings){

	CMU_ClockEnable(cmuClock_USART1, true);                  // enable USART1 peripheral clock

	/* Configure GPIO */
	GPIO_PinModeSet(spiSettings.spi_miso_port, spiSettings.spi_miso_pin, gpioModeInput, 1);      // configure MISO pin as input, no filter
	GPIO_PinModeSet(spiSettings.spi_mosi_port, spiSettings.spi_mosi_pin, gpioModePushPull, 1);    // configure MOSI pin as output, initialize high
	GPIO_PinModeSet(spiSettings.spi_cs_port, spiSettings.spi_cs_pin, gpioModePushPull, 1);      // configure CS pin as output, initialize high
	GPIO_PinModeSet(spiSettings.spi_sclk_port, spiSettings.spi_sclk_pin, gpioModePushPull, 0);    // configure SCLK pin as output, initialize low

	cs_port = spiSettings.spi_cs_port;
	cs_pin =  spiSettings.spi_cs_pin;

	/* Configure SPI Port (USART1 in sync mode) */
	USART_InitSync_TypeDef usart_settings;
	usart_settings.enable = usartEnable;        // enable bidirectional data (TX and RX)
	usart_settings.refFreq = 0;                 // measure source clock
	usart_settings.baudrate = 100000;        	// 12Mbps is max data rate with 24MHz clock source
	usart_settings.databits = usartDatabits8;   // 8 data bits per frame
	usart_settings.master = true;               // configure as SPI master
	usart_settings.msbf = true;                 // transmit msb first
	usart_settings.clockMode = usartClockMode1; // clock idles low, data setup on rising edge, sampled on falling edge

	//TODO location
	USART_IntClear(USART1, 0x1FF9);                // clear interrupt flags (optional)
	USART_InitSync(USART1, &spi_init);              // apply configuration to USART1
	USART1->ROUTE = (0 << 8) | ( 1 << 3)| (3 << 0); // use location #3, enable TX/RX/CLK pins
	USART_Enable(USART1, usartEnable);

	spi_cs_set(); for(int i=0;i<10;i++) clockTest();
}

// This function drives the CS pin low
void spi_cs_clr(void) {
  GPIO_PinOutClear(cs_port, cs_pin);
}

// This function drives the CS pin high
void spi_cs_set(void) {
  GPIO_PinOutSet(cs_port, cs_pin);
}
