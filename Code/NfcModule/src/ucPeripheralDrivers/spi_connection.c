/*
 * spi_connection.c
 *
 *  Created on: 20-07-2015
 *      Author: terg
 */


#include "spi_connection.h"
#include "em_usart.h"
#include "spi.h"
#include "em_cmu.h"

void spi_init(struct SPI_Settings spiSettings)
{

	CMU_ClockEnable(cmuClock_USART1, true);                  // enable USART1 peripheral clock

	/* Configure GPIO */
	GPIO_PinModeSet(spiSettings.spi_miso_port, spiSettings.spi_miso_pin, gpioModeInput, 1);      // configure MISO pin as input, no filter
	GPIO_PinModeSet(spiSettings.spi_mosi_port, spiSettings.spi_mosi_pin, gpioModePushPull, 1);    // configure MOSI pin as output, initialize high
	GPIO_PinModeSet(spiSettings.spi_sclk_port, spiSettings.spi_sclk_pin, gpioModePushPull, 0);    // configure SCLK pin as output, initialize low

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
	USART_InitSync(USART1, &usart_settings);              // apply configuration to USART1
	USART1->ROUTE = (0 << 8) | ( 1 << 3)| (3 << 0); // use location #3, enable TX/RX/CLK pins
	USART_Enable(USART1, usartEnable);
}

void spi_WriteBlock(uint8_t length, uint8_t* data)
{
	for (int i=0;i<length;i++)
	{
		USART_SpiTransfer(USART1, data[i]);
	}
}

void spi_WriteReadBlock(uint8_t lenTx, uint8_t* dataTx, uint8_t lenRx, uint8_t* dataRx)
{
	for (int i=0;(i<lenRx || i<lenTx) ;i++)
	{
		if (i<lenRx)
		{
			if (i<lenTx)
			{
				dataRx[i] = USART_SpiTransfer(USART1, dataTx[i]);
			}
			else
			{
				dataRx[i] = USART_SpiTransfer(USART1, 0x00);
			}
		}
		else
		{
			USART_SpiTransfer(USART1, dataTx[i]);
		}
	}
}

void spi_ReadBlock(uint8_t length, uint8_t* data)
{
	for (int i=0;i<length;i++)
	{
		data[i] = USART_SpiTransfer(USART1, 0x00);
	}
}
