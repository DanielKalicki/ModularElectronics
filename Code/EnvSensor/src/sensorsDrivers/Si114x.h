/*
 * Si114x.h
 *
 *  Created on: 18-07-2015
 *      Author: terg
 */

#ifndef SI114X_H_
#define SI114X_H_

#include <stdint.h>

uint8_t Si114x_detect(void);
void Si114x_init(void);

void Si114x_forceAmbientLightMeasurment(void);
int Si114x_readAmbientLight(uint16_t *ambientLight);

#endif /* SI114X_H_ */
