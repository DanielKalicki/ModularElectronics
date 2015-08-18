/*
 * spi_connection.h
 *
 *  Created on: 20-07-2015
 *      Author: terg
 */

#ifndef SPI_CONNECTION_H_
#define SPI_CONNECTION_H_

#include "em_device.h"
#include "em_gpio.h"

struct SPI_Settings{
	GPIO_Port_TypeDef spi_miso_port;
	unsigned int spi_miso_pin;

	GPIO_Port_TypeDef spi_mosi_port;
	unsigned int spi_mosi_pin;

	GPIO_Port_TypeDef spi_sclk_port;
	unsigned int spi_sclk_pin;

	uint8_t spi_location;
};

void spi_init(struct SPI_Settings spiSettings);

void spi_WriteBlock(uint8_t length, uint8_t* data);
void spi_WriteReadBlock(uint8_t lenTx, uint8_t* dataTx, uint8_t lenRx, uint8_t* dataRx);
void spi_ReadBlock(uint8_t length, uint8_t* data);

void spi_cs_clr(void);
void spi_cs_set(void);


#endif /* SPI_CONNECTION_H_ */
