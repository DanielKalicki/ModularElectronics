#ifndef __I2C_CONNECTION_H
#define __I2C_CONNECTION_H

#include "em_device.h"
#include "em_i2c.h"
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

//i2c master
/*#define I2CDRV_SCL_PORT 		gpioPortE
#define I2CDRV_SCL_PIN  		13
#define I2CDRV_SDA_PORT 		gpioPortE
#define I2CDRV_SDA_PIN  		12
#define I2CDRV_PORT_LOCATION 	6
#define I2CDRV_TRANSFER_TIMEOUT 3000*/
#define I2CDRV_SCL_PORT gpioPortE
#define I2CDRV_SCL_PIN  13
#define I2CDRV_SDA_PORT gpioPortE
#define I2CDRV_SDA_PIN  12
#define I2CDRV_PORT_LOCATION 6
#define I2CDRV_TRANSFER_TIMEOUT 3000

//i2c slave
#define I2CDRV_SCL_PORT_SLAVE 		gpioPortD
#define I2CDRV_SCL_PIN_SLAVE		7
#define I2CDRV_SDA_PORT_SLAVE 		gpioPortD
#define I2CDRV_SDA_PIN_SLAVE 		6
#define I2CDRV_PORT_LOCATION_SLAVE 	1
#define I2C_SLAVE_ADDRESS           i2c_slave_address
#define I2C_RXBUFFER_SIZE       	10
#define I2C_REG_BUFFER_SIZE       	100
#define CORE_FREQUENCY          	14000000

// i2c slave Transmission flags
volatile bool i2c_rxInProgress;
volatile bool i2c_startTx;
volatile uint8_t i2c_rxBuffer[I2C_RXBUFFER_SIZE];
volatile uint8_t i2c_rxBufferIndex;
volatile uint8_t i2c_registers[I2C_REG_BUFFER_SIZE];
volatile uint32_t slavesList[4];
volatile uint32_t slavesListCheck[4];
volatile uint8_t i2c_slave_address;

I2C_TransferReturn_TypeDef I2C_Status;	//Global variable

void initI2C_Master(void);
void I2C0_IRQHandler(void);
int i2c_RegisterGet(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t *val);
int i2c_RegisterSet(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint16_t  val);
int i2c_Register_Write_Block (I2C_TypeDef *i2c,uint8_t addr, uint8_t reg, uint8_t length, uint8_t *data);
int i2c_Register_Read_Block (I2C_TypeDef *i2c,uint8_t addr, uint8_t reg, uint8_t length, uint8_t *data);
bool i2c_Detect    (I2C_TypeDef *i2c, uint8_t addr);
void i2c_Scan (I2C_TypeDef *i2c);

//i2c slave
void initI2C_Slave(void);
void performI2CTransfer(void);
void receiveI2CData(void);
void disableI2cSlaveInterrupts(void);
void enableI2cSlaveInterrupts(void);
void receiveI2CSlaveData(void);

#ifdef __cplusplus
}
#endif

#endif
