/*
 * AS3953.c
 *
 *  Created on: 20-07-2015
 *      Author: terg
 */
#include "AS3953.h"
#include "..\ucPeripheralDrivers\spi_connection.h"

//TODO V remove those files are temporary
#include "em_usart.h"
#include "..\ucPeripheralDrivers\spi.h"
#include "em_gpio.h"

static GPIO_Port_TypeDef cs_port;
static unsigned int cs_pin;

// This function drives the CS pin low
static void cs_clr(void)
{
  GPIO_PinOutClear(cs_port, cs_pin);
}
// This function drives the CS pin high
static void cs_set(void)
{
  GPIO_PinOutSet(cs_port, cs_pin);
}

static void clockTest_short()
{
	long int i=0;
	for(;i<121L;++i)
	{
	  if(i==120L)
		  GPIO_PortOutSet(gpioPortF, 0x4);
	  if(i==60L)
		  GPIO_PortOutClear(gpioPortF, 0x4);
	}
}

void AS3953_Init(AS3953_Settings_t AS3953_Setting)
{
	uint8_t conf[4];

	GPIO_PinModeSet(AS3953_Setting.spi_cs_port, AS3953_Setting.spi_cs_pin, gpioModePushPull, 1);      // configure CS pin as output, initialize high

	cs_port = AS3953_Setting.spi_cs_port;
	cs_pin =  AS3953_Setting.spi_cs_pin;

	cs_set(); for (int i=0;i<20;i++) { clockTest_short(); }	//TODO delete this delay

	AS3953_Read_Conf(conf);

	if (	AS3953_Setting.conf_word[0]!=conf[0] ||
			AS3953_Setting.conf_word[1]!=conf[1] ||
			AS3953_Setting.conf_word[2]!=conf[2] ||
			AS3953_Setting.conf_word[3]!=conf[3]
		)
	{
		AS3953_EEPROM_Write(2, AS3953_Setting.conf_word, 4);
		clockTest_short();

		AS3953_Read_Conf(conf);
	}

	/* By setting the mode to gpioModeInput its value can be read */
	GPIO_PinModeSet(AS3953_Setting.irq_port, AS3953_Setting.irq_pin, gpioModeInput, 1);
	if ((AS3953_Setting.irq_pin & 0x01) == 0)
	{
		/* Enable GPIO_EVEN interrupt in NVIC */
		NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	}
	else
	{
		/* Enable GPIO_ODD interrupt in NVIC */
		NVIC_EnableIRQ(GPIO_ODD_IRQn);
	}
	 /* Configure interrupt on falling edge for pins PA0 by default */
	GPIO_IntConfig(AS3953_Setting.irq_port, AS3953_Setting.irq_pin, true, false, true);
}

void AS3953_Read_UID(uint8_t *uid)
{
	AS3953_EEPROM_Read(0, uid, 4);
}
void AS3953_Read_Lock(uint8_t *lock)
{
	AS3953_EEPROM_Read(3, lock, 4);
}
void AS3953_Read_Conf(uint8_t *conf)
{
	AS3953_EEPROM_Read(2, conf, 4);
}

void AS3953_Write_Register(uint8_t reg, uint8_t data)
{
	cs_clr(); clockTest_short();
	//send reg address
	USART_SpiTransfer(USART1, reg&0x1F);
	//send data
	USART_SpiTransfer(USART1, data);
	cs_set();

	/*uint8_t dataTx[2];
	dataTx[0]=reg&0x1F;
	dataTx[1]=data;*/
}
uint8_t AS3953_Read_Register(uint8_t reg)
{
	uint8_t rxData[2];

	cs_clr(); clockTest_short();

	rxData[0] = USART_SpiTransfer(USART1, (reg&0x1F)+0x20);
	rxData[1] = USART_SpiTransfer(USART1, 0x00);

	cs_set();

	/*uint8_t dataRx[1];
	uint8_t dataTx[1];
	dataTx[0]=(reg&0x1F)+0x20;
	spi_WriteBlock(1, dataTx);
	spi_ReadBlock(1,dataRx);
	return dataRx[0];*/

	return rxData[1];
}

void AS3953_Command(uint8_t cmd)
{
	cs_clr(); clockTest_short();
	USART_SpiTransfer(USART1,0xC0|cmd);
	cs_set();

	/*spi_WriteBlock(1, &cmd);*/
}

void AS3953_FIFO_clr(void)
{
	AS3953_Command(0xC4);	//clear fifo command
}
void AS3953_FIFO_Init(uint16_t byteNumb)
{
	AS3953_FIFO_clr();

	AS3953_Write_Register(AS3953_NUM_TX_BYTE_1_REG_ADDR, (byteNumb & 0xF8) >> 3);
	AS3953_Write_Register(AS3953_NUM_TX_BYTE_2_REG_ADDR, (byteNumb & 0x07) << 5);
}
void AS3953_FIFO_Write(uint8_t *data, uint8_t len)
{
	cs_clr(); clockTest_short();

	USART_SpiTransfer(USART1, 0x80);	//mask it fifo write

	for (int i = 0; i < len; i++)
	{
		USART_SpiTransfer(USART1, data[i]);
	}

	cs_set();

	/*uint8_t initData[1];
	initData[0]=0x80;
	spi_WriteBlock(1,initData);
	spi_WriteBlock(len,data);*/
}
void AS3953_FIFO_Read(uint8_t *data, uint8_t len)
{
	cs_clr(); clockTest_short();

	USART_SpiTransfer(USART1, 0xBF);
	for (int i=0;i<len;i++){
		data[i]=USART_SpiTransfer(USART1,0x00);
	}

	cs_set();

	/*uint8_t initData[1];
	initData[0]=0xBF;
	spi_WriteBlock(1,initData);
	spi_ReadBlock(len, data);*/
}

void AS3953_EEPROM_Write(uint8_t word, uint8_t *data, uint8_t len)
{
	//TODO protect agains writting to lock bits.
	cs_clr(); clockTest_short();

	USART_SpiTransfer(USART1, 0x40);//mask it eeprom write
	USART_SpiTransfer(USART1, ((word & 0x1F) << 1));

	for (int i = 0; i < len; i++)
	{
		USART_SpiTransfer(USART1, data[i]);
	}

	cs_set();

	/*uint8_t initData[2];
	initData[0]=0x40;
	initData[1]=((word & 0x1F) << 1);
	spi_WriteBlock(2,initData);
	spi_WriteBlock(len,data);*/
}
uint8_t AS3953_EEPROM_Read(uint8_t addr, uint8_t *data, uint8_t len)
{
	cs_clr(); clockTest_short();

	if(addr > 0x1F) 			return 1;
	//if (!len || len % 4) 	return 1;

	USART_SpiTransfer(USART1, 0x7F);	//mask it
	USART_SpiTransfer(USART1, addr << 1);

	for (int i = 0; i < len; i++){
		data[i] = USART_SpiTransfer(USART1, 0x00);
	}

	cs_set();


	/*uint8_t initData[2];
	initData[0]=0x7F;
	initData[1]=addr<<1;
	spi_WriteBlock(2,initData);
	spi_ReadBlock(len, data);*/

	return 0;
}

#define _IO_CONF_PICC_AFE_STAT_MASK		0x01
#define _IO_CONF_PICC_AFE_STAT_SHIFT	7
#define _IO_CONF_PICC_POWER_STAT_MASK	0x07
#define _IO_CONF_PICC_POWER_STAT_SHIFT	4
void AS3953_Status(AS3953_PICC_AFE_PowerStatus_t *power, AS3953_Status_t *status)
{
	uint8_t regVal = AS3953_Read_Register(AS3953_IO_CONF_REG_ADDR);

	switch((regVal >> _IO_CONF_PICC_AFE_STAT_SHIFT) & _IO_CONF_PICC_AFE_STAT_MASK)
	{
	case 0:
		*power = PICC_AFE_OFF;
		break;
	case 1:
		*power = PICC_AFE_ON;
		break;
	}

	switch ((regVal >> _IO_CONF_PICC_POWER_STAT_SHIFT) & _IO_CONF_PICC_POWER_STAT_MASK)
	{
	case 0:
		*status = POWER_OFF;
		break;
	case 1:
		*status = IDLE;
		break;
	case 2:
		*status = READY;
		break;
	case 3:
		*status = ACTIVE;
		break;
	case 5:
		*status = HALT;
		break;
	case 6:
		*status = READYX;
		break;
	case 7:
		*status = ACTIVEX;
		break;
	case 4:
		*status = L4;
		break;
	default:
		*status=POWER_OFF; //there is no error status
		break;
	}
}

#define _FIFO_RX_STAT_MASK		0x3F
uint8_t AS3953_FifoRxStatus()
{
	uint8_t regVal = AS3953_Read_Register(AS3953_FIFO_STAT_1_REG_ADDR);
	return (regVal & _FIFO_RX_STAT_MASK);
}

#define _FIFO_TX_STAT_MASK					0x03
#define _FIFO_TX_STAT_SHIFT					1
#define _FIFO_TX_PARITY_BIT_MISSING_MASK	0x01
#define _FIFO_TX_LAST_FIFO_NOTCOMPLETE_MASK	0x10
#define _FIFO_TX_OVERFLOW_MASK				0x20
#define _FIFO_TX_UNDERFLOW_MASK				0x40
uint8_t AS3953_FifoTxStatus(AS3953_FifoErrors_t* errors)
{
	uint8_t regVal = AS3953_Read_Register(AS3953_FIFO_STAT_2_REG_ADDR);

	/* Framing error (Parity bit is missing in last byte) */
	if (regVal & _FIFO_TX_PARITY_BIT_MISSING_MASK)
	{
		errors->ParityBitMissing = 1;
	}
	else
	{
		errors->ParityBitMissing = 0;
	}

	/* Last Fifo not complete error */
	if (regVal & _FIFO_TX_LAST_FIFO_NOTCOMPLETE_MASK)
	{
		errors->Last_Fifo_NotComplete = 1;
	}
	else
	{
		errors->Last_Fifo_NotComplete = 0;
	}

	/* Overflow error */
	if (regVal & _FIFO_TX_OVERFLOW_MASK)
	{
		errors->Fifo_Overflow = 1;
	}
	else
	{
		errors->Fifo_Overflow = 0;
	}

	/* Underflow error */
	if (regVal & _FIFO_TX_UNDERFLOW_MASK)
	{
		errors->Fifo_Underflow = 1;
	}
	else
	{
		errors->Fifo_Underflow = 0;
	}

	return ((regVal >> _FIFO_TX_STAT_SHIFT) & _FIFO_TX_STAT_MASK);
}

#define _TX_BYTE_NUMBER_MSB_MASK	0x1F
#define _TX_BYTE_NUMBER_MSB_SHIFT	3
#define _TX_BYTE_NUMBER_LSB_MASK	0xE0
#define _TX_BYTE_NUMBER_LSB_SHIFT	5
uint8_t AS3953_TxBytesNumber()
{
	uint8_t regVal1 = AS3953_Read_Register(AS3953_NUM_TX_BYTE_1_REG_ADDR);
	uint8_t regVal2 = AS3953_Read_Register(AS3953_NUM_TX_BYTE_1_REG_ADDR);
	return (((regVal1 & _TX_BYTE_NUMBER_MSB_MASK) << _TX_BYTE_NUMBER_MSB_SHIFT )
		   + ((regVal2 & _TX_BYTE_NUMBER_LSB_MASK) >> _TX_BYTE_NUMBER_LSB_SHIFT));
}

#define _RATS_CID_FSDI_BIT_MASK		0xF0
#define _RATS_CID_FSDI_BIT_SHIFT	0x04
#define _RATS_CID_CID_BIT_MASK		0x0F
#define _RATS_CID_CID_BIT_SHIFT		0x01
void AS3953_RATS(uint8_t* RATS_FSDI_BitNumber, uint8_t* RATS_CID_BitNumber)
{
	uint8_t regVal = AS3953_Read_Register(AS3953_RATS_REG_ADDR);
	*RATS_FSDI_BitNumber = (regVal & _RATS_CID_FSDI_BIT_MASK) >> _RATS_CID_FSDI_BIT_SHIFT;
	*RATS_CID_BitNumber  = (regVal & _RATS_CID_CID_BIT_MASK)  >> _RATS_CID_CID_BIT_SHIFT;
}

#define _IRQ_POWER_UP_MASK				0x80
#define _IRQ_ACTIVE_STATE_ENTER_MASK	0x40
#define _IRQ_WAKEUP_COMMAND_SEND_MASK	0x20
#define _IRQ_RECEIVE_START_MASK			0x10
#define _IRQ_RECEIVE_END_MASK			0x08
#define _IRQ_TRANSMITION_END_MASK		0x04
#define _IRQ_FIFO_WATER_LEVEL_MASK		0x02
void AS3953_Read_MainInterrupts(AS3953_MainInterrupts_t* Main_Interrupts)
{
	uint8_t regVal = AS3953_Read_Register(AS3953_MAIN_INT_REG_ADDR);

	if (regVal & _IRQ_POWER_UP_MASK)
	{
		Main_Interrupts->PowerUp_IRQ = 1;
	}
	else
	{
		Main_Interrupts->PowerUp_IRQ = 0;
	}

	if (regVal & _IRQ_ACTIVE_STATE_ENTER_MASK)
	{
		Main_Interrupts->Active_State_Enter_IRQ = 1;
	}
	else
	{
		Main_Interrupts->Active_State_Enter_IRQ = 0;
	}

	if (regVal & _IRQ_WAKEUP_COMMAND_SEND_MASK)
	{
		Main_Interrupts->WakeUp_Command_Send_IRQ = 1;
	}
	else
	{
		Main_Interrupts->WakeUp_Command_Send_IRQ = 0;
	}

	if (regVal & _IRQ_RECEIVE_START_MASK)
	{
		Main_Interrupts->Receive_Start_IRQ = 1;
	}
	else
	{
		Main_Interrupts->Receive_Start_IRQ = 0;
	}

	if (regVal & _IRQ_RECEIVE_END_MASK)
	{
		Main_Interrupts->Receive_End_IRQ = 1;
	}
	else
	{
		Main_Interrupts->Receive_End_IRQ = 0;
	}

	if (regVal & _IRQ_TRANSMITION_END_MASK)
	{
		Main_Interrupts->Transmition_End_IRQ = 1;
	}
	else
	{
		Main_Interrupts->Transmition_End_IRQ = 0;
	}

	if (regVal & _IRQ_FIFO_WATER_LEVEL_MASK)
	{
		Main_Interrupts->Fifo_Water_Level_IRQ = 1;
	}
	else
	{
		Main_Interrupts->Fifo_Water_Level_IRQ = 0;
	}
}


#define _IRQ_DESELECT_RECEPTION_MASK				0x80
#define _IRQ_FRAMING_ERROR_MASK						0x40
#define _IRQ_PARITY_ERROR_MASK						0x20
#define _IRQ_CRC_ERROR_MASK							0x10
#define _IRQ_FIFO_ERROR_MASK						0x08
#define _IRQ_SUCCESSFUL_EEPROM_TERMINATION_MASK		0x04
#define _IRQ_EEPROM_PROGRAMMING_ERROR_MASK			0x02
#define _IRQ_EEPROM_ACCESS_DUE_TO_PICC_ACTIVATION	0X01
void AS3953_Read_AuxInterrupts(AS3953_AuxInterrupts_t* Aux_Interrupts)
{
	uint8_t regVal = AS3953_Read_Register(AS3953_AUX_INT_REG_ADDR);

	if (regVal & _IRQ_DESELECT_RECEPTION_MASK)
	{
		Aux_Interrupts->Deselect_Command_Reception_IRQ = 1;
	}
	else
	{
		Aux_Interrupts->Deselect_Command_Reception_IRQ = 0;
	}

	if (regVal & _IRQ_FRAMING_ERROR_MASK)
	{
		Aux_Interrupts->Framing_Error_IRQ = 1;
	}
	else
	{
		Aux_Interrupts->Framing_Error_IRQ = 0;
	}

	if (regVal & _IRQ_PARITY_ERROR_MASK)
	{
		Aux_Interrupts->Parity_Error_IRQ = 1;
	}
	else
	{
		Aux_Interrupts->Parity_Error_IRQ = 0;
	}

	if (regVal & _IRQ_CRC_ERROR_MASK)
	{
		Aux_Interrupts->CRC_Error_IRQ = 1;
	}
	else
	{
		Aux_Interrupts->CRC_Error_IRQ = 0;
	}

	if (regVal & _IRQ_FIFO_ERROR_MASK)
	{
		Aux_Interrupts->FIFO_Error_IRQ = 1;
	}
	else
	{
		Aux_Interrupts->FIFO_Error_IRQ = 0;
	}

	if (regVal & _IRQ_SUCCESSFUL_EEPROM_TERMINATION_MASK)
	{
		Aux_Interrupts->EEPROM_Successful_Termination = 1;
	}
	else
	{
		Aux_Interrupts->EEPROM_Successful_Termination = 0;
	}

	if (regVal & _IRQ_EEPROM_PROGRAMMING_ERROR_MASK)
	{
		Aux_Interrupts->EEPROM_Programming_Error_IRQ = 1;
	}
	else
	{
		Aux_Interrupts->EEPROM_Programming_Error_IRQ = 0;
	}

	if (regVal & _IRQ_EEPROM_ACCESS_DUE_TO_PICC_ACTIVATION)
	{
		Aux_Interrupts->EEPROM_Access_Due_To_PICC_Activation = 1;
	}
	else
	{
		Aux_Interrupts->EEPROM_Access_Due_To_PICC_Activation = 0;
	}
}


void AS3953_sendData(uint8_t *data, uint8_t len)
{
	AS3953_FIFO_Init(len);
	AS3953_FIFO_Write(data, len);
	AS3953_Command(0xC8);
}
