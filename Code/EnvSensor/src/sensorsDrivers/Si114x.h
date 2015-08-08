/*
 * Si114x.h
 *
 *  Created on: 18-07-2015
 *      Author: terg
 */

#ifndef SI114X_H_
#define SI114X_H_

#include <stdint.h>
#include "em_gpio.h"

/*----------------------------------------------------*/
/*							!!!						  */
/* In Si114x_reg.h you need to define the used sensor */
/*							!!!						  */
/*----------------------------------------------------*/

uint8_t Si114x_Detect(void);

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
