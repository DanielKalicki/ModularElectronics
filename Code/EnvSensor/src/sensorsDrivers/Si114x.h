/*
 * Si114x.h
 *
 *  Created on: 18-07-2015
 *      Author: terg
 */

#ifndef SI114X_H_
#define SI114X_H_

#include <stdint.h>

/* Define used sensor for i2c address */
#define SI1147

uint8_t Si114x_Detect(void);

#include "em_gpio.h"

typedef struct
{
	GPIO_Port_TypeDef irq_port;
	unsigned int irq_pin;
} Si114x_Settings_t;

void Si114x_Init(Si114x_Settings_t Si114x_Settings);

void Si114x_Interrupt(void);

void Si114x_ForceAmbientLightMeasurment(void);
uint8_t Si114x_ReadAmbientLight(uint16_t *AmbientLight);

void Si114x_ForceUvIndexMeasurment(void);
uint8_t Si114x_ReadUvIndex(uint16_t *UvIndex);

uint8_t Si114x_ReadPsMeasurments(uint16_t *PS1, uint16_t *PS2, uint16_t *PS3);

#endif /* SI114X_H_ */
