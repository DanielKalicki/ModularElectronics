/*
 * Si7013.h
 *
 *  Created on: 18-07-2015
 *      Author: terg
 */

#ifndef SI7013_H_
#define SI7013_H_

#include <stdint.h>

uint8_t Si7013_detect(void);
void Si7013_init(void);

void Si7013_forceRhMeasurment(void);
int Si7013_readHumidityAndTemperature(uint32_t *rhData, int32_t *tData);

#endif /* SI7013_H_ */
