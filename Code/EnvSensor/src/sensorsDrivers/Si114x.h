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
void Si114x_Init(void);

void Si114x_ForceAmbientLightMeasurment(void);
uint8_t Si114x_ReadAmbientLight(uint16_t *AmbientLight);

#endif /* SI114X_H_ */
