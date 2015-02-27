#ifndef __I2C_CONNECTION_H
#define __I2C_CONNECTION_H

#include "em_device.h"
#include "em_i2c.h"
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

#define I2CDRV_SCL_PORT 		gpioPortC
#define I2CDRV_SCL_PIN  		1
#define I2CDRV_SDA_PORT 		gpioPortC
#define I2CDRV_SDA_PIN  		0
#define I2CDRV_PORT_LOCATION 	4
#define I2CDRV_TRANSFER_TIMEOUT 300000

I2C_TransferReturn_TypeDef I2C_Status;	//Global variable

void initI2C(void);
void I2C0_IRQHandler(void);
int i2c_RegisterGet(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t *val);
int i2c_RegisterSet(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint16_t  val);
int i2c_Register_Read_Block (I2C_TypeDef *i2c,uint8_t addr, uint8_t reg, uint8_t length, uint8_t *data);
bool i2c_Detect(I2C_TypeDef *i2c, uint8_t addr);
void i2c_Scan (I2C_TypeDef *i2c);

#ifdef __cplusplus
}
#endif

#endif
