/*
 * BMP180.h
 *
 *  Created on: 18-07-2015
 *      Author: terg
 */

#ifndef BMP180_H_
#define BMP180_H_

#include "em_i2c.h"

void BMP180_init(void);
uint8_t BMP180_detect(void);

void BMP180_forceTemperatureMesurement(void);
void BMP180_forcePressureMeasurment(void);
void BMP180_readTemperature(short *temp);
void BMP180_readPressure(uint32_t *pressure);

#endif /* BMP180_H_ */
