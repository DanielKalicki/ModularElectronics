/*
 * AS3953.h
 *
 *  Created on: 20-07-2015
 *      Author: terg
 */

#ifndef AS3953_H_
#define AS3953_H_

#include "em_gpio.h"

typedef struct
{
	uint8_t conf_word[4];

	GPIO_Port_TypeDef spi_cs_port;
	unsigned int spi_cs_pin;

	GPIO_Port_TypeDef irq_port;
	unsigned int irq_pin;
} AS3953_Settings_t;

void AS3953_Init(AS3953_Settings_t AS3953_Settings);

void AS3953_Read_UID(uint8_t *uid);
void AS3953_Read_Lock(uint8_t *lock);
void AS3953_Read_Conf(uint8_t *conf);

void AS3953_Write_Register(uint8_t reg, uint8_t data);
uint8_t AS3953_Read_Register(uint8_t reg);

void AS3953_Command(uint8_t cmd);
void AS3953_FIFO_clr(void);
void AS3953_FIFO_Init(uint16_t byteNumb);
void AS3953_FIFO_Write(uint8_t *data, uint8_t len);
void AS3953_FIFO_Read(uint8_t *data, uint8_t len);
void AS3953_EEPROM_Write(uint8_t word, uint8_t *data, uint8_t len);
uint8_t AS3953_EEPROM_Read(uint8_t addr, uint8_t *data, uint8_t len);

#define AS3953_IO_CONF_REG_ADDR			0x00
#define AS3953_MODE_REG_ADDR			0x01
#define AS3953_BIT_RATE_REG_ADDR		0x02
#define AS3953_RFID_STATUS_REG_ADDR		0x04
#define AS3953_RATS_REG_ADDR			0x05
#define AS3953_MASK_MAIN_INT_REG_ADDR	0x08
#define AS3953_MASK_AUX_INT_REG_ADDR	0x09
#define AS3953_MAIN_INT_REG_ADDR		0x0A
#define AS3953_AUX_INT_REG_ADDR			0x0B
#define AS3953_FIFO_STAT_1_REG_ADDR		0x0C
#define AS3953_FIFO_STAT_2_REG_ADDR		0x0D
#define AS3953_NUM_TX_BYTE_1_REG_ADDR	0x10
#define AS3953_NUM_TX_BYTE_2_REG_ADDR	0x11

typedef enum
{
	PICC_AFE_OFF = 0,
	PICC_AFE_ON = 1
} AS3953_PICC_AFE_PowerStatus_t;
typedef enum
{
	POWER_OFF = 0,
	IDLE = 1,
	READY = 2,
	ACTIVE = 3,
	L4 = 4,
	HALT = 5,
	READYX = 6,
	ACTIVEX = 7
} AS3953_Status_t;
void AS3953_Status(AS3953_PICC_AFE_PowerStatus_t *power, AS3953_Status_t *status);

uint8_t AS3953_FifoRxStatus();

typedef struct
{
	uint8_t Fifo_Underflow;
	uint8_t Fifo_Overflow;
	uint8_t Last_Fifo_NotComplete;
	uint8_t ParityBitMissing;
} AS3953_FifoErrors_t;
uint8_t AS3953_FifoTxStatus(AS3953_FifoErrors_t *errors);

void AS3953_RATS(uint8_t* RATS_FSDI_BitNumber, uint8_t *RATS_CID_BitNumber);

typedef struct
{
	uint8_t PowerUp_IRQ;
	uint8_t Active_State_Enter_IRQ;
	uint8_t WakeUp_Command_Send_IRQ;
	uint8_t Receive_Start_IRQ;
	uint8_t Receive_End_IRQ;
	uint8_t Transmition_End_IRQ;
	uint8_t Fifo_Water_Level_IRQ;
} AS3953_MainInterrupts_t;
void AS3953_Read_MainInterrupts(AS3953_MainInterrupts_t *Main_Interrupts);

typedef struct
{
	uint8_t Deselect_Command_Reception_IRQ;
	uint8_t Framing_Error_IRQ;
	uint8_t Parity_Error_IRQ;
	uint8_t CRC_Error_IRQ;
	uint8_t FIFO_Error_IRQ;
	uint8_t EEPROM_Successful_Termination;
	uint8_t EEPROM_Programming_Error_IRQ;
	uint8_t EEPROM_Access_Due_To_PICC_Activation;
} AS3953_AuxInterrupts_t;
void AS3953_Read_AuxInterrupts(AS3953_AuxInterrupts_t *Aux_Interrupts);

void AS3953_sendData(uint8_t *data, uint8_t len);

#endif /* AS3953_H_ */
