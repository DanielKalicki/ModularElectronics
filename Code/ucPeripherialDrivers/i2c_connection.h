#ifndef __I2C_CONNECTION_H
#define __I2C_CONNECTION_H

#include "main.h"

#include "em_device.h"
#include "em_i2c.h"
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

//i2c master
#define I2CDRV_TRANSFER_TIMEOUT 3000

volatile I2C_TransferReturn_TypeDef I2C_Status;	//Global variable

//TODO remove later
#define I2C_REG_BUFFER_SIZE       	100
volatile uint8_t i2c_registers[I2C_REG_BUFFER_SIZE];

void initI2C_Master(void);
int i2c_RegisterGet(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t *val);
int i2c_RegisterSet(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint16_t  val);
int i2c_Register_Write_Block (I2C_TypeDef *i2c,uint8_t addr, uint8_t reg, uint8_t length, uint8_t *data);
int i2c_Register_Read_Block (I2C_TypeDef *i2c,uint8_t addr, uint8_t reg, uint8_t length, uint8_t *data);
bool i2c_Detect    (I2C_TypeDef *i2c, uint8_t addr);
void i2c_Scan (I2C_TypeDef *i2c);

#ifdef __cplusplus
}
#endif

#endif
