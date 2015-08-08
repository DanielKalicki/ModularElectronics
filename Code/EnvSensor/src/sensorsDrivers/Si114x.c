/*
 * Si114x.c
 *
 *  Created on: 18-07-2015
 *      Author: terg
 */

#include "Si114x.h"
#include "..\ucPeripheralDrivers\i2c_connection.h"
#include "..\ucPeripheralDrivers\leuart_connection.h"

/* TODO
 * read block of two bytes data like SI114X_ALS_VIS_DATA[0:1]_REG
 */

#define TRUE	(1)
#define FALSE	(0)

#if defined(SI1141) || defined(SI1142) || defined(SI1143)
const uint8_t SI114x_ADDR=0x5A*2;
#endif

#if defined(SI1145) || defined(SI1146) || defined(SI1147)
const uint8_t SI114x_ADDR=0x60*2;
#endif

typedef enum {
	SI114X_PART_ID_REG  		= 0x00,
	SI114X_REV_ID_REG			= 0x01,
	SI114X_SEQ_ID_REG			= 0x02,
	SI114X_INT_CFG_REG			= 0x03,
	SI114X_IRQ_ENABLE_REG		= 0x04,

	SI114X_HW_KEY_REG			= 0x07,
	SI114X_MEAS_RATE0_REG		= 0x08,
	SI114X_MEAS_RATE1_REG		= 0x09,

	SI114X_PS_LED21_REG			= 0x0F,
	SI114X_PS_LED3_REG			= 0x10,

	SI114X_UCOEF0_REG			= 0x13,
	SI114X_UCOEF1_REG			= 0x14,
	SI114X_UCOEF2_REG			= 0x15,
	SI114X_UCOEF3_REG			= 0x16,
	SI114X_PARAM_WR_REG			= 0x17,
	SI114X_COMMAND_REG			= 0x18,

	SI114X_RESPONSE_REG			= 0x20,
	SI114X_IRQ_STATUS_REG		= 0x21,
	SI114X_ALS_VIS_DATA0_REG	= 0x22,
	SI114X_ALS_VIS_DATA1_REG	= 0x23,
	SI114X_ALS_IR_DATA0_REG		= 0x24,
	SI114X_ALS_IR_DATA1_REG		= 0x25,
	SI114X_PS1_DATA0_REG		= 0x26,
	SI114X_PS1_DATA1_REG		= 0x27,
	SI114X_PS2_DATA0_REG		= 0x28,
	SI114X_PS2_DATA1_REG		= 0x29,
	SI114X_PS3_DATA0_REG		= 0x2A,
	SI114X_PS3_DATA1_REG		= 0x2B,

	SI114X_AUX_UV_DATA0_REG		= 0x2C,
	SI114X_AUX_UV_DATA1_REG		= 0x2D,
	SI114X_PARAM_RD_REG			= 0x2E,
	SI114X_CHIP_STAT_REG		= 0x30
	/* TODO ANA_IN_KEY register is not implemented */
} SI114x_Registers_t;

static void Si114x_SetRegister(SI114x_Registers_t Si114x_reg, uint8_t val)
{
	i2c_RegisterSet(I2C0, SI114x_ADDR, (uint8_t)Si114x_reg, val);
}

static void Si114x_GetRegister(SI114x_Registers_t Si114x_reg, uint8_t *val)
{
	i2c_RegisterGet(I2C0, SI114x_ADDR, (uint8_t)Si114x_reg, val);
}

/* --------------------------------------------- */
/*												 */
/* 			Register Set/Get functions 			 */
/*												 */
/* --------------------------------------------- */
/* -------------- PART ID ---------------*/
uint8_t Si114x_GetPartId(void)
{
	uint8_t ret = 0;
	Si114x_GetRegister(SI114X_PART_ID_REG, &ret);
	return(ret);
}

/* -------------- REV ID ---------------*/
uint8_t Si114x_GetRevId(void)
{
	uint8_t ret = 0;
	Si114x_GetRegister(SI114X_REV_ID_REG, &ret);
	return(ret);
}

/* -------------- SEQ ID ---------------*/
uint8_t Si114x_GetSeqId(void)
{
	uint8_t ret = 0;
	Si114x_GetRegister(SI114X_SEQ_ID_REG, &ret);
	return(ret);
}

/* -------------- INT_CFG ---------------*/
typedef enum {
	SI114X_INT_OE_DISABLE = 0,
	SI114X_INT_OE_ENABLE = 1
} Si114x_Int_Cfg_t;

#define SI114X_INT_CFG_MASK	0x01
#define SI114X_INT_CFG_SHIFT	0

void Si114x_SetIntCfg(Si114x_Int_Cfg_t IntCfg)
{
	uint8_t val = (IntCfg & SI114X_INT_CFG_MASK) << SI114X_INT_CFG_SHIFT;
	Si114x_SetRegister(SI114X_INT_CFG_REG, val);
}

/* -------------- IRQ_ENABLE ---------------*/
typedef enum {
	SI114X_PS3_IE_DISABLE = 0,
	SI114X_PS3_IE_ENABLE = 1
} Si114x_Irq_Enable_PS3_t;
#define SI114X_IRQ_EN_PS3_SHIFT	4

typedef enum {
	SI114X_PS2_IE_DISABLE = 0,
	SI114X_PS2_IE_ENABLE = 1
} Si114x_Irq_Enable_PS2_t;
#define SI114X_IRQ_EN_PS2_SHIFT	3

typedef enum {
	SI114X_PS1_IE_DISABLE = 0,
	SI114X_PS1_IE_ENABLE = 1
} Si114x_Irq_Enable_PS1_t;
#define SI114X_IRQ_EN_PS1_SHIFT	2

typedef enum {
	SI114X_ALS_IE_DISABLE = 0,
	SI114X_ALS_IE_ENABLE = 1
} Si114x_Irq_Enable_ALS_t;
#define SI114X_IRQ_EN_ALS_SHIFT	0

void Si114x_SetIrqEnable(Si114x_Irq_Enable_ALS_t IrqEnableALS,
						 Si114x_Irq_Enable_PS1_t IrqEnablePs1,
						 Si114x_Irq_Enable_PS2_t IrqEnablePs2,
						 Si114x_Irq_Enable_PS3_t IrqEnablePs3)
{
	uint8_t val = (IrqEnablePs3 << SI114X_IRQ_EN_PS3_SHIFT)
				+ (IrqEnablePs2 << SI114X_IRQ_EN_PS2_SHIFT)
				+ (IrqEnablePs1 << SI114X_IRQ_EN_PS1_SHIFT)
				+ (IrqEnableALS << SI114X_IRQ_EN_ALS_SHIFT);
	Si114x_SetRegister(SI114X_IRQ_ENABLE_REG, val);
}

/* ---------------- SI114X_HW_KEY -----------------*/
void Si114x_SetHwKey(void)
{
	/* This register needs to be set to 0x17 for proper
	 * Si114x operation.
	 */
	Si114x_SetRegister(SI114X_HW_KEY_REG, 0x17);
}

/* -------------- SI114X_MEAS_RATE0 ---------------*/
/* 	MEAS_RATE1 and MEAS_RATE0 together form a 16-bit value: MEAS_RATE [15:0].
	The 16-bit value, when multiplied by 31.25 us, represents the time duration between
	wake-up periods where measurements are made. Once the device wakes up, all
	measurements specified in CHLIST are made.
	Note that for the Si1145/6/7 with SEQ_ID=0x01, there is a code error that places
	MEAS_RATE0 at 0x0A with MEAS_RATE1 at 0x08 instead. This will be fixed in
	future revisions of the Si1145/6/7

	32000 = 1s
	32 	  = 1ms
*/
void Si114x_SetMeasurmentRate(uint16_t measRate)
{
	Si114x_SetRegister(SI114X_MEAS_RATE0_REG, (uint8_t)(measRate));
	Si114x_SetRegister(SI114X_MEAS_RATE1_REG, (uint8_t)((measRate >> 8) & 0xFF));

#ifdef DEBUG
	uint8_t reg1;
	uint8_t reg2;
	Si114x_GetRegister(SI114X_MEAS_RATE0_REG, &reg1);
	Si114x_GetRegister(SI114X_MEAS_RATE1_REG, &reg2);
	char buff[30];
	sprintf(buff, "MeasRate:0x%02x%02x=%d\n", reg2, reg1, (reg1 + reg2 * 256u));
	LeUart_SendText(buff);
#endif
}

/* -------------- SI114X_PS_LED21 -------------- */
/*  LEDx_I Represents the irLED current sunk by the LEDx pin during a PS measurement.
	LED3_I, LED2_I, and LED1_I current encoded as follows:
	0000: No current
	0001: Minimum current
	1111: Maximum current
	Refer to Table 2, “Performance Characteristics1,” on page 5 for LED current values
*/
void Si114x_SetLedCurrents(uint8_t Led1_Current, uint8_t Led2_Current, uint8_t Led3_Current)
{
#if defined(SI1141) || defined(SI1145)
	Led2_Current = 0;
	Led3_Current = 0;
#endif

#if defined(SI1142) || defined(SI1146)
	Led3_Current = 0;
#endif

	uint8_t val1 = (Led1_Current & 0x0F) + ((Led2_Current & 0x0F) << 4);
	uint8_t val2 = Led3_Current & 0x0F;

	Si114x_SetRegister(SI114X_PS_LED21_REG, val1);
	Si114x_SetRegister(SI114X_PS_LED3_REG, val2);

#ifdef DEBUG
	uint8_t reg1;
	uint8_t reg2;
	Si114x_GetRegister(SI114X_PS_LED21_REG, &reg1);
	Si114x_GetRegister(SI114X_PS_LED3_REG, &reg2);
	char buff[30];
	sprintf(buff, "\nLED Current settings: 0x%02x, 0x%02x\n", reg1, reg2);
	LeUart_SendText(buff);
#endif
}

/* -------------- SI114X_UCOEF[0:3] -------------- */
void Si114x_EnableUvReading()
{
	Si114x_SetRegister(SI114X_UCOEF0_REG, 0x7B);
	Si114x_SetRegister(SI114X_UCOEF1_REG, 0x6B);
	Si114x_SetRegister(SI114X_UCOEF2_REG, 0x01);
	Si114x_SetRegister(SI114X_UCOEF3_REG, 0x00);
}
void Si114x_DisableUvReading()
{
	Si114x_SetRegister(SI114X_UCOEF0_REG, 0x00);
	Si114x_SetRegister(SI114X_UCOEF1_REG, 0x00);
	Si114x_SetRegister(SI114X_UCOEF2_REG, 0x00);
	Si114x_SetRegister(SI114X_UCOEF3_REG, 0x00);
}

/* -------------- PARAM_WR -------------- */
void Si114x_SetParamWr(uint8_t val)
{
	Si114x_SetRegister(SI114X_PARAM_WR_REG, val);
}

/* -------------- COMMAND -------------- */
typedef enum{
	SI114X_CMD_PARAM_QUERY,
	SI114X_CMD_PARAM_SET,
	SI114X_CMD_NOP,
	SI114X_CMD_RESET,
	SI114X_CMD_BUSADDR,
	SI114X_CMD_GET_CAL,

	SI114X_CMD_PS_FORCE,
	SI114X_CMD_ALS_FORCE,
	SI114X_CMD_PSALS_FORCE,

	SI114X_CMD_PS_PAUSE,
	SI114X_CMD_ALS_PAUSE,
	SI114X_CMD_PSALS_PAUSE,

	SI114X_CMD_PS_AUTO,
	SI114X_CMD_ALS_AUTO,
	SI114X_CMD_PSALS_AUTO,
} Si114x_Command_Name_t;

void Si114x_SetCommand(Si114x_Command_Name_t cmd, uint8_t val)
{
	uint8_t data = val;
	switch (cmd)
	{
	case SI114X_CMD_PARAM_QUERY:
		data = (val & 0x1F) + 0x80;
		break;
	case SI114X_CMD_PARAM_SET:
		data = (val & 0x1F) + 0xA0;
		break;
	case SI114X_CMD_NOP:
		data = 0x00;
		break;
	case SI114X_CMD_RESET:
		data = 0x01;
		break;
	case SI114X_CMD_BUSADDR:
		data = 0x02;
		break;
	case SI114X_CMD_PS_FORCE:
		data = 0x05;
		break;
	case SI114X_CMD_GET_CAL:
		data = 0x12;
		break;
	case SI114X_CMD_ALS_FORCE:
		data = 0x06;
		break;
	case SI114X_CMD_PSALS_FORCE:
		data = 0x07;
		break;
	case SI114X_CMD_PS_PAUSE:
		data = 0x09;
		break;
	case SI114X_CMD_ALS_PAUSE:
		data = 0x0A;
		break;
	case SI114X_CMD_PSALS_PAUSE:
		data = 0x0B;
		break;
	case SI114X_CMD_PS_AUTO:
		data = 0x0D;
		break;
	case SI114X_CMD_ALS_AUTO:
		data = 0x0E;
		break;
	case SI114X_CMD_PSALS_AUTO:
		data = 0x0F;
		break;
	}
	Si114x_SetRegister(SI114X_COMMAND_REG, data);
}

/* -------------- PARAM_RD -------------- */
void Si114x_SetParamRd(uint8_t val)
{
	Si114x_SetRegister(SI114X_PARAM_RD_REG, val);
}

/* -------------- RESPONSE -------------- */
/*  The Response register is used in conjunction with command processing. When an error
	is encountered, the response register will be loaded with an error code. All error codes
	will have the MSB is set.
	The error code is retained until a RESET or NOP command is received by the
	sequencer. Other commands other than RESET or NOP will be ignored. However, any
	autonomous operation in progress continues normal operation despite any error.
	0x00–0x0F: No Error. Bits 3:0 form an incrementing roll-over counter. The roll over
	counter in bit 3:0 increments when a command has been executed by the Si114x. Once
	autonomous measurements have started, the execution timing of any command
	becomes non-deterministic since a measurement could be in progress when the
	COMMAND register is written. The host software must make use of the rollover counter
	to ensure that commands are processed.
	0x80: Invalid Command Encountered during command processing
	0x88: ADC Overflow encountered during PS1 measurement
	0x89: ADC Overflow encountered during PS2 measurement
	0x8A: ADC Overflow encountered during PS3 measurement
	0x8C: ADC Overflow encountered during ALS-VIS measurement
	0x8D: ADC Overflow encountered during ALS-IR measurement
	0x8E: ADC Overflow encountered during AUX measurement
*/
uint8_t Si114x_GetResponse()
{
	uint8_t ret = 0;
	Si114x_GetRegister(SI114X_RESPONSE_REG, &ret);
	return(ret);
}

/* -------------- IRQ STATUS -------------- */
#define SI114X_CMD_INT_MASK 0x20
#define SI114X_PS3_INT_MASK 0x10
#define SI114X_PS2_INT_MASK 0x08
#define SI114X_PS1_INT_MASK 0x04
#define SI114X_ALS_INT_MASK 0x01

void Si114x_GetIrqStatus(uint8_t *ALS_Int, uint8_t *PS1_Int, uint8_t *PS2_Int, uint8_t *PS3_Int, uint8_t *CMD_Int)
{
	uint8_t ret = 0;
	Si114x_GetRegister(SI114X_IRQ_STATUS_REG, &ret);

	if (ret & SI114X_CMD_INT_MASK)
	{
		*CMD_Int = 1;
	}
	else
	{
		*CMD_Int = 0;
	}
	if (ret & SI114X_PS3_INT_MASK)
	{
		*PS3_Int = 1;
	}
	else
	{
		*PS3_Int = 0;
	}
	if (ret & SI114X_PS2_INT_MASK)
	{
		*PS2_Int = 1;
	}
	else
	{
		*PS2_Int = 0;
	}
	if (ret & SI114X_PS1_INT_MASK)
	{
		*PS1_Int = 1;
	}
	else
	{
		*PS1_Int = 0;
	}
	if (ret & SI114X_ALS_INT_MASK)
	{
		*ALS_Int = 1;
	}
	else
	{
		*ALS_Int = 0;
	}

}

void Si114x_ClrIrqStatus(void)
{
	Si114x_SetRegister(SI114X_IRQ_STATUS_REG, 0x00);
}

/* -------------- CHIP_STAT -------------- */
#define SI114X_CHIP_STAT_RUNNING_MASK 0x04
#define SI114X_CHIP_STAT_SUSPEND_MASK 0x02
#define SI114X_CHIP_STAT_SLEEP_MASK	  0x01

void Si114x_GetChipStat(uint8_t *Si114x_Sleep, uint8_t *Si114x_Suspend, uint8_t *Si114x_Running)
{
	uint8_t ret = 0;
	Si114x_GetRegister(SI114X_CHIP_STAT_REG, &ret);

	if (ret & SI114X_CHIP_STAT_RUNNING_MASK)
	{
		*Si114x_Running = 1;
	}
	else
	{
		*Si114x_Running = 0;
	}
	if (ret & SI114X_CHIP_STAT_SUSPEND_MASK)
	{
		*Si114x_Suspend = 1;
	}
	else
	{
		*Si114x_Suspend = 0;
	}
	if (ret & SI114X_CHIP_STAT_SLEEP_MASK)
	{
		*Si114x_Sleep = 1;
	}
	else
	{
		*Si114x_Sleep = 0;
	}
}

/* -------------- ALS_VIS_DATA[0:1] -------------- */
uint16_t Si114x_GetAlsVisData(void)
{
	uint16_t ret;

	uint8_t ALS_Vis_Data_Low;
	Si114x_GetRegister(SI114X_ALS_VIS_DATA0_REG, &ALS_Vis_Data_Low);
	uint8_t ALS_Vis_Data_High;
	Si114x_GetRegister(SI114X_ALS_VIS_DATA1_REG, &ALS_Vis_Data_High);

	ret = (ALS_Vis_Data_High << 8) + ALS_Vis_Data_Low;

	return(ret);
}

/* -------------- ALS_IR_DATA[0:1] -------------- */
uint16_t Si114x_GetAlsIrData(void)
{
	uint16_t ret;

	uint8_t ALS_Ir_Data_Low;
	Si114x_GetRegister(SI114X_ALS_IR_DATA0_REG, &ALS_Ir_Data_Low);
	uint8_t ALS_Ir_Data_High;
	Si114x_GetRegister(SI114X_ALS_IR_DATA1_REG, &ALS_Ir_Data_High);

	ret = (ALS_Ir_Data_High << 8) + ALS_Ir_Data_Low;

	return(ret);
}

/* -------------- PS1_DATA[0:1] -------------- */
uint16_t Si114x_GetPs1Data(void)
{
	uint16_t ret;

	uint8_t PS1_Data_Low;
	Si114x_GetRegister(SI114X_PS1_DATA0_REG, &PS1_Data_Low);
	uint8_t PS1_Data_High;
	Si114x_GetRegister(SI114X_PS1_DATA1_REG, &PS1_Data_High);

	ret = (PS1_Data_High << 8) + PS1_Data_Low;

	return(ret);
}

/* -------------- PS2_DATA[0:1] -------------- */
uint16_t Si114x_GetPs2Data(void)
{
	uint16_t ret;

	uint8_t PS2_Data_Low;
	Si114x_GetRegister(SI114X_PS2_DATA0_REG, &PS2_Data_Low);
	uint8_t PS2_Data_High;
	Si114x_GetRegister(SI114X_PS2_DATA1_REG, &PS2_Data_High);

	ret = (PS2_Data_High << 8) + PS2_Data_Low;

	return(ret);
}

/* -------------- PS3_DATA[0:1] -------------- */
uint16_t Si114x_GetPs3Data(void)
{
	uint16_t ret;

	uint8_t PS3_Data_Low;
	Si114x_GetRegister(SI114X_PS3_DATA0_REG, &PS3_Data_Low);
	uint8_t PS3_Data_High;
	Si114x_GetRegister(SI114X_PS3_DATA1_REG, &PS3_Data_High);

	ret = (PS3_Data_High << 8) + PS3_Data_Low;

	return(ret);
}

/* -------------- AUX_UV_DATA[0:1] -------------- */
uint16_t Si114x_GetAuxUvData(void)
{
	uint16_t ret;

	uint8_t AuxUv_Data_Low;
	Si114x_GetRegister(SI114X_AUX_UV_DATA0_REG, &AuxUv_Data_Low);
	uint8_t AuxUv_Data_High;
	Si114x_GetRegister(SI114X_AUX_UV_DATA1_REG, &AuxUv_Data_High);

	ret = (AuxUv_Data_High << 8) + AuxUv_Data_Low;

	return(ret);
}

/* --------------------------------------------- */
/*												 */
/* 		 Parameter RAM Set/Get functions	 	 */
/*												 */
/* --------------------------------------------- */

typedef enum{
	SI114X_I2C_ADDR_REG				= 0x00,
	SI114X_CHLIST_REG				= 0x01,
	SI114X_PSLED12_SELECT_REG		= 0x02,
	SI114X_PSLED3_SELECT_REG		= 0x03,

	SI114X_PS_ENCODING_REG			= 0x05,
	SI114X_ALS_ENCODING_REG			= 0x06,
	SI114X_PS1_ADCMUX_REG			= 0x07,
	SI114X_PS2_ADCMUX_REG			= 0x08,
	SI114X_PS3_ADCMUX_REG			= 0x09,
	SI114X_PS_ADC_COUNTER_REG		= 0x0A,
	SI114X_PS_ADC_GAIN_REG			= 0x0B,
	SI114X_PS_ADC_MISC_REG			= 0x0C,

	SI114X_ALS_IR_ADCMUX_REG		= 0x0E,
	SI114X_AUX_ADCMUX_REG			= 0x0F,
	SI114X_ALS_VIS_ADC_COUNTER_REG	= 0x10,
	SI114X_ALS_VIS_ADC_GAIN_REG		= 0x11,
	SI114X_ALS_VIS_ADC_MISC_REG		= 0x12,

	SI114X_LED_REC_REG				= 0x1C,
	SI114X_ALS_IR_ADC_COUNTER_REG	= 0x1D,
	SI114X_ALS_IR_ADC_GAIN_REG		= 0x1E,
	SI114X_ALS_IR_ADC_MISC_REG		= 0x1F
} Si114x_Parameter_RAM_Registers_t;

/* -------------- CHLIST_REG -------------- */
#define SI114X_EN_UV_CHANNEL_SHIFT		7
#define SI114X_EN_AUX_CHANNEL_SHIFT		6
#define SI114X_EN_ALS_IR_CHANNEL_SHIFT	5
#define SI114X_EN_ALS_VIS_CHANNEL_SHIFT	4
#define SI114X_EN_PS3_CHANNEL_SHIFT		2
#define SI114X_EN_PS2_CHANNEL_SHIFT		1
#define SI114X_EN_PS1_CHANNEL_SHIFT		0

typedef struct{
	uint8_t UV;
	uint8_t AUX;
	uint8_t ALS_IR;
	uint8_t ALS_Vis;
	uint8_t PS3;
	uint8_t PS2;
	uint8_t PS1;
}Si114x_EnableChannel_t;

void Si114x_EnableChannel(Si114x_EnableChannel_t EnableChannel)
{
	uint8_t val;
	val = ((EnableChannel.UV & 0x01) 		<< SI114X_EN_UV_CHANNEL_SHIFT)
		+ ((EnableChannel.AUX & 0x01) 		<< SI114X_EN_AUX_CHANNEL_SHIFT)
		+ ((EnableChannel.ALS_IR & 0x01)	<< SI114X_EN_ALS_IR_CHANNEL_SHIFT)
		+ ((EnableChannel.ALS_Vis & 0x01)	<< SI114X_EN_ALS_VIS_CHANNEL_SHIFT)
		+ ((EnableChannel.PS3 & 0x01)		<< SI114X_EN_PS3_CHANNEL_SHIFT)
		+ ((EnableChannel.PS2 & 0x01)		<< SI114X_EN_PS2_CHANNEL_SHIFT)
		+ ((EnableChannel.PS1 & 0x01)		<< SI114X_EN_PS1_CHANNEL_SHIFT);
	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_CHLIST_REG);

#ifdef DEBUG
	Si114x_SetCommand(SI114X_CMD_PARAM_QUERY, SI114X_CHLIST_REG);
	uint8_t reg;
	Si114x_GetRegister(SI114X_PARAM_RD_REG, &reg);
	char buff[30];
	sprintf(buff, "\nSI114X_CHLIST_REG: 0x%02x\n", reg);
	LeUart_SendText(buff);
#endif
}

/* -------------- PSLED12_SELECT -------------- */
/*  Specifies the LED pin driven during the PSx Measurement. Note that any combination
	of irLEDs is possible.
	000: NO LED DRIVE
	xx1: LED1 Drive Enabled
	x1x: LED2 Drive Enabled (Si1146 and Si1147 only. Clear for Si1145)
	1xx: LED3 Drive Enabled (Si1147 only. Clear for Si1145 and Si1146)
*/
#define SI114X_PS3_LED_SHIFT		0
#define SI114X_PS2_LED_SHIFT		4
#define SI114X_PS1_LED_SHIFT		0

/* To select Led configuration you can or the macros together:
 * Led1 and Led2 enabled ---> SI114X_LED1_ENABLE | SI114X_LED2_ENABLE
 */
#define SI114X_LED1_ENABLE 	 (0x01)
#define SI114X_LED2_ENABLE 	 (0x02)
#define SI114X_LED3_ENABLE 	 (0x04)

typedef struct{
	uint8_t PS1_Led;
	uint8_t PS2_Led;
	uint8_t PS3_Led;
} Si114x_PsX_Led_Select_t;

void Si114x_SetPsLedSelect(Si114x_PsX_Led_Select_t PsXLedSelect)
{

#ifdef DEBUG
	char buff[30];
	uint8_t reg;
#endif
#if defined(SI1141) || defined(SI1145)
	/* Those ic can drive only one led */
	PsXLedSelect.PS3_Led = PsXLedSelect.PS3_Led & 0x01;
	PsXLedSelect.PS2_Led = PsXLedSelect.PS2_Led & 0x01;
	PsXLedSelect.PS1_Led = PsXLedSelect.PS1_Led & 0x01;
#endif

#if defined(SI1142) || defined(SI1146)
	/* Those ic can drive only two led */
	PsXLedSelect.PS3_Led = PsXLedSelect.PS3_Led & 0x03;
	PsXLedSelect.PS2_Led = PsXLedSelect.PS2_Led & 0x03;
	PsXLedSelect.PS1_Led = PsXLedSelect.PS1_Led & 0x03;
#endif

	uint8_t val = 0x00;
	val = ((PsXLedSelect.PS2_Led & 0x07) << SI114X_PS2_LED_SHIFT)
		+ ((PsXLedSelect.PS1_Led & 0x07) << SI114X_PS1_LED_SHIFT);
	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_PSLED12_SELECT_REG);

	/* TODO This readout function makes the setting to be done correctly
		    this ensures a small delay between setting the PS leds registers */
	Si114x_SetCommand(SI114X_CMD_PARAM_QUERY, SI114X_PSLED12_SELECT_REG);
	Si114x_GetRegister(SI114X_PARAM_RD_REG, &reg);
#ifdef DEBUG
	sprintf(buff, "SI114X_PSLED12_SELECT_REG: 0x%02x\n", reg);
	LeUart_SendText(buff);
#endif

	val = (PsXLedSelect.PS3_Led & 0x07) << SI114X_PS3_LED_SHIFT;
	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_PSLED3_SELECT_REG);

#ifdef DEBUG
	Si114x_SetCommand(SI114X_CMD_PARAM_QUERY, SI114X_PSLED3_SELECT_REG);
	Si114x_GetRegister(SI114X_PARAM_RD_REG, &reg);
	sprintf(buff, "SI114X_PSLED3_SELECT_REG: 0x%02x\n", reg);
	LeUart_SendText(buff);
#endif

}

/* -------------- PS_ENCODING -------------- */
/*  When set, the ADC reports the least significant 16 bits of the 17-bit ADC when performing
	PS3 Measurement. Reports the 16 MSBs when cleared.
*/
#define SI114X_AS1_ALIGN_SHIFT		4
#define SI114X_AS2_ALIGN_SHIFT		5
#define SI114X_AS3_ALIGN_SHIFT		6

void Si114x_SetPsEncoding(uint8_t PS1_Align, uint8_t PS2_Align, uint8_t PS3_Align)
{
	uint8_t val = 0x00;
	val = ((PS1_Align & 0x01) << SI114X_AS1_ALIGN_SHIFT)
		+ ((PS2_Align & 0x01) << SI114X_AS2_ALIGN_SHIFT)
		+ ((PS3_Align & 0x01) << SI114X_AS3_ALIGN_SHIFT);
	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_PS_ENCODING_REG);
}

/* -------------- ALS_ENCODING -------------- */
/*  When set, the ADC reports the least significant 16 bits of the 17-bit ADC when
	performing ALS VIS Measurement. Reports the 16 MSBs when cleared..
*/
#define SI114X_ALS_IR_ALIGN_SHIFT		5
#define SI114X_ALS_VIS_ALIGN_SHIFT		6

void Si114x_SetAlsEncoding(uint8_t ALS_IR_Align, uint8_t ALS_Vis_Align)
{
	uint8_t val = 0x00;
	val = ((ALS_IR_Align  & 0x01) << SI114X_ALS_IR_ALIGN_SHIFT)
		+ ((ALS_Vis_Align & 0x01) << SI114X_ALS_VIS_ALIGN_SHIFT);
	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_ALS_ENCODING_REG);
}


/* -------------- PS[1:3]_ADCMUX -------------- */
/*  Selects ADC Input for PS1 Measurement.
	The following selections are valid when PS_ADC_MODE = 1 (default). This setting
	is for normal Proximity Detection function.
	0x03: Large IR Photodiode
	0x00: Small IR Photodiode
	In addition, the following selections are valid for PS_ADC_MODE = 0. With this
	setting, irLED drives are disabled and the PS channels are no longer operating in
	normal Proximity Detection function. The results have no reference and the references
	needs to be measured in a separate measurement.
	0x02: Visible Photodiode
	A separate 'No Photodiode' measurement should be subtracted from this reading.
	Note that the result is a negative value. The result should therefore be negated to
	arrive at the Ambient Visible Light reading.
	0x03: Large IR Photodiode
	A separate “No Photodiode” measurement should be subtracted to arrive at
	Ambient IR reading.
	0x00: Small IR Photodiode
	A separate “No Photodiode” measurement should be subtracted to arrive at
	Ambient IR reading.
	0x06: No Photodiode
	This is typically used as reference for reading ambient IR or visible light.
	0x25: GND voltage
	This is typically used as the reference for electrical measurements.
	0x65: Temperature
	(Should be used only for relative temperature measurement. Absolute Temperature
	not guaranteed) A separate GND measurement should be subtracted from
	this reading.
	0x75: VDD voltage
	A separate GND measurement is needed to make the measurement meaningful */
typedef enum{
	SI114X_PS_ADC_SMALL_IR				= 0x00,
	SI114X_PS_ADC_VISIBLE_PHOTODIODE	= 0x02,
	SI114X_PS_ADC_LARGE_IR				= 0x03,
	SI114X_PS_ADC_NO_PHOTODIODE			= 0x06,
	SI114X_PS_ADC_GND_VOLT				= 0x25,
	SI114X_PS_ADC_TEMPERATURE			= 0x65,
	SI114X_PS_ADC_VDD_VOLT				= 0x75
} Si114x_PS_Meas_ADC_t;

void Si114x_SetPs1ADC(Si114x_PS_Meas_ADC_t adc)
{
	Si114x_SetParamWr((uint8_t)adc);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_PS1_ADCMUX_REG);

#ifdef DEBUG
	Si114x_SetCommand(SI114X_CMD_PARAM_QUERY, SI114X_PS1_ADCMUX_REG);
	uint8_t reg;
	Si114x_GetRegister(SI114X_PARAM_RD_REG, &reg);
	char buff[30];
	sprintf(buff, "SI114X_PS1_ADCMUX_REG: 0x%02x\n", reg);
	LeUart_SendText(buff);
#endif
}
void Si114x_SetPs2ADC(Si114x_PS_Meas_ADC_t adc)
{
	Si114x_SetParamWr((uint8_t)adc);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_PS2_ADCMUX_REG);

#ifdef DEBUG
	Si114x_SetCommand(SI114X_CMD_PARAM_QUERY, SI114X_PS2_ADCMUX_REG);
	uint8_t reg;
	Si114x_GetRegister(SI114X_PARAM_RD_REG, &reg);
	char buff[30];
	sprintf(buff, "SI114X_PS2_ADCMUX_REG: 0x%02x\n", reg);
	LeUart_SendText(buff);
#endif
}
void Si114x_SetPs3ADC(Si114x_PS_Meas_ADC_t adc)
{
	Si114x_SetParamWr((uint8_t)adc);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_PS3_ADCMUX_REG);

#ifdef DEBUG
	Si114x_SetCommand(SI114X_CMD_PARAM_QUERY, SI114X_PS3_ADCMUX_REG);
	uint8_t reg;
	Si114x_GetRegister(SI114X_PARAM_RD_REG, &reg);
	char buff[30];
	sprintf(buff, "SI114X_PS3_ADCMUX_REG: 0x%02x\n", reg);
	LeUart_SendText(buff);
#endif
}

/* -------------- AUX_ADCMUX -------------- */
typedef enum{
	SI114X_AUX_ADC_SMALL_IR				= 0x00,
	SI114X_AUX_ADC_LARGE_IR				= 0x03,
	SI114X_AUX_ADC_TEMPERATURE			= 0x65,
	SI114X_AUX_ADC_VDD_VOLT				= 0x75
} Si114x_Aux_Meas_ADC_t;

void Si114x_SetAuxADC(Si114x_Aux_Meas_ADC_t adc)
{
	Si114x_SetParamWr((uint8_t)adc);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_AUX_ADCMUX_REG);
}

/* -------------- PS_ADC_COUNTER -------------- */
/*  Recovery period the ADC takes before making a PS measurement.
	000: 1 ADC Clock (50 ns times 2PS_ADC_GAIN)
	001: 7 ADC Clock (350 ns times 2PS_ADC_GAIN)
	010: 15 ADC Clock (750 ns times 2PS_ADC_GAIN)
	011: 31 ADC Clock (1.55 µs times 2PS_ADC_GAIN)
	100: 63 ADC Clock (3.15 µs times 2PS_ADC_GAIN)
	101: 127 ADC Clock (6.35 µs times 2PS_ADC_GAIN)
	110: 255 ADC Clock (12.75 µs times 2PS_ADC_GAIN)
	111: 511 ADC Clock (25.55 µs times 2PS_ADC_GAIN)
	The recommended PS_ADC_REC value is the one’s complement of PS_ADC_GAIN. */

#define SI114X_PS_ADC_REC_SHIFT 4

void Si114x_SetPsAdcCounter(uint8_t adcRecPeriod)
{
	uint8_t val = 0x00;
	val = (adcRecPeriod & 0x07) << SI114X_PS_ADC_REC_SHIFT;

	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_PS_ADC_COUNTER_REG);

#ifdef DEBUG
	Si114x_SetCommand(SI114X_CMD_PARAM_QUERY, SI114X_PS_ADC_COUNTER_REG);
	uint8_t reg;
	Si114x_GetRegister(SI114X_PARAM_RD_REG, &reg);
	char buff[30];
	sprintf(buff, "SI114X_PS_ADC_COUNTER_REG: 0x%02x\n", reg);
	LeUart_SendText(buff);
#endif
}

/* -------------- ALS_VIS_ADC_COUNTER -------------- */
#define SI114X_ALS_VIS_ADC_REC_SHIFT 4

void Si114x_SetAlsVisAdcCounter(uint8_t adcRecPeriod)
{
	uint8_t val = 0x00;
	val = (adcRecPeriod & 0x07) << SI114X_ALS_VIS_ADC_REC_SHIFT;

	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_ALS_VIS_ADC_COUNTER_REG);
}

/* -------------- ALS_IR_ADC_COUNTER -------------- */
#define SI114X_ALS_IR_ADC_REC_SHIFT 4

void Si114x_SetAlsIrAdcCounter(uint8_t adcRecPeriod)
{
	uint8_t val = 0x00;
	val = (adcRecPeriod & 0x07) << SI114X_ALS_IR_ADC_REC_SHIFT;

	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_ALS_IR_ADC_COUNTER_REG);
}

/* -------------- PS_ADC_GAIN -------------- */
/*  Increases the irLED pulse width and ADC integration time by a factor of
	(2 ^ PS_ADC_GAIN) for all PS measurements.
	Care must be taken when using this feature. At an extreme case, each of the
	three PS measurements can be configured to drive three separate irLEDs,
	each of which, are configured for 359 mA. The internal sequencer does not
	protect the device from such an error. To prevent permanent damage to the
	device, do not enter any value greater than 5 without consulting with Silicon
	Labs.
	For Example:
	0x0: ADC Clock is divided by 1
	0x4: ADC Clock is divided by 16
	0x5: ADC Clock is divided by 32 */
void Si114x_SetPsAdcGain(uint8_t adcGain, uint8_t protectionDisable)
{
	uint8_t val = 0x00;
	val = (adcGain & 0x07);
	if (protectionDisable == 0)
	{
		if (val > 0x05)
		{
			val = 0x05;
		}
	}
	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_PS_ADC_GAIN_REG);

#ifdef DEBUG
	Si114x_SetCommand(SI114X_CMD_PARAM_QUERY, SI114X_PS_ADC_GAIN_REG);
	uint8_t reg;
	Si114x_GetRegister(SI114X_PARAM_RD_REG, &reg);
	char buff[30];
	sprintf(buff, "SI114X_PS_ADC_GAIN_REG: 0x%02x\n", reg);
	LeUart_SendText(buff);
#endif
}

/* -------------- ALS_VIS_ADC_GAIN -------------- */
/*  Increases the ADC integration time for ALS Visible measurements by a
	factor of (2 ^ ALS_VIS_ADC_GAIN). This allows visible light measurement
	under dark glass. The maximum gain is 128 (0x7).
	For Example:
	0x0: ADC Clock is divided by 1
	0x4: ADC Clock is divided by 16
	0x6: ADC Clock is divided by 64 */
void Si114x_SetAlsVisAdcGain(uint8_t adcGain)
{
	uint8_t val = 0x00;
	val = (adcGain & 0x07);
	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_ALS_VIS_ADC_GAIN_REG);
}

/* -------------- ALS_IR_ADC_GAIN -------------- */
void Si114x_SetAlsIrAdcGain(uint8_t adcGain)
{
	uint8_t val = 0x00;
	val = (adcGain & 0x07);
	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_ALS_IR_ADC_GAIN_REG);
}

/* -------------- PS_ADC_MISC -------------- */
/* PS_RANGE *********************************
    When performing PS measurements, the ADC can be programmed to operate in
	high sensitivity operation or high signal range. The high signal range is useful in
	operation under direct sunlight.
	0: Normal Signal Range
	1: High Signal Range (Gain divided by 14.5)
 * PS_ADC_MODE ******************************
	PS Channels can either operate normally as PS channels, or it can be used to perform
	raw ADC measurements:
	0: Raw ADC Measurement Mode
	1: Normal Proximity Measurement Mode
*/
#define SI114X_PS_RANGE_SHIFT 5
#define SI114X_PS_ADC_MODE_SHIFT 2

void Si114x_SetPsAdcMisc(uint8_t PsRange, uint8_t PsAdcMode)
{
	uint8_t val = 0x00;
	val = ((PsRange & 0x01)   << SI114X_PS_RANGE_SHIFT)
		+ ((PsAdcMode & 0x01) << SI114X_PS_ADC_MODE_SHIFT);

	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_PS_ADC_MISC_REG);

#ifdef DEBUG
	Si114x_SetCommand(SI114X_CMD_PARAM_QUERY, SI114X_PS_ADC_MISC_REG);
	uint8_t reg;
	Si114x_GetRegister(SI114X_PARAM_RD_REG, &reg);
	char buff[30];
	sprintf(buff, "SI114X_PS_ADC_MISC_REG: 0x%02x\n", reg);
	LeUart_SendText(buff);
#endif
}

/* -------------- ALS_VIS_ADC_MISC -------------- */
/* PS_RANGE *********************************
    When performing PS measurements, the ADC can be programmed to operate in
	high sensitivity operation or high signal range. The high signal range is useful in
	operation under direct sunlight.
	0: Normal Signal Range
	1: High Signal Range (Gain divided by 14.5) */
#define SI114X_ALS_VIS_RANGE_SHIFT 5

void Si114x_SetAlsVisAdcMisc(uint8_t AlsVisRange)
{
	uint8_t val = 0x00;
	val = ((AlsVisRange & 0x01) << SI114X_ALS_VIS_RANGE_SHIFT);

	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_ALS_VIS_ADC_MISC_REG);
}

/* -------------- ALS_IR_ADC_MISC -------------- */
#define SI114X_ALS_IR_RANGE_SHIFT 5

void Si114x_SetAlsIrAdcMisc(uint8_t AlsIrRange)
{
	uint8_t val = 0x00;
	val = ((AlsIrRange & 0x01) << SI114X_ALS_IR_RANGE_SHIFT);

	Si114x_SetParamWr(val);
	Si114x_SetCommand(SI114X_CMD_PARAM_SET, SI114X_ALS_IR_ADC_MISC_REG);
}

/* --------------------------------------------- */
/*												 */
/* 		 IC related function - higher level 	 */
/*												 */
/* --------------------------------------------- */
uint8_t Si114x_Detect(void)
{
	return i2c_Detect(I2C0, SI114x_ADDR);
}

void Si114x_Init(Si114x_Settings_t Si114x_Settings)
{
			/*--------------------------------*/
			/*        IRQ configuration       */
			/*--------------------------------*/
	/* By setting the mode to gpioModeInput its value can be read */
	GPIO_PinModeSet(Si114x_Settings.irq_port, Si114x_Settings.irq_pin, gpioModeInput, 1);
	if ((Si114x_Settings.irq_pin & 0x01) == 0)
	{
		/* Enable GPIO_EVEN interrupt in NVIC */
		NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	}
	else
	{
		/* Enable GPIO_ODD interrupt in NVIC */
		NVIC_EnableIRQ(GPIO_ODD_IRQn);
	}
	 /* Configure interrupt on falling edge by default */
	GPIO_IntConfig(Si114x_Settings.irq_port, Si114x_Settings.irq_pin, false, true, true);


			/*--------------------------------*/
			/*      Si114x configuration      */
			/*--------------------------------*/
	Si114x_SetHwKey();

	/* turn on ALS_VIS measurement */
	Si114x_EnableChannel_t enableChannel;
	enableChannel.ALS_IR 	= FALSE;
	enableChannel.ALS_Vis 	= TRUE;
	enableChannel.AUX 		= FALSE;
	enableChannel.PS1 		= TRUE;
	enableChannel.PS2 		= TRUE;
	enableChannel.PS3 		= TRUE;
	enableChannel.UV 		= TRUE;
	Si114x_EnableChannel(enableChannel);

	/* ALS_VIS_ADC_GAIN is set to 1 by default */
	/* ALS_VIS_ADC_MISC is set to Normal Signal Range by default */

	/* Set the device to wake-up every 1ms and performs all CHILIST */
	/* Maximum ALS + UV + 3x PS measurement time = 660us */
	/* Single PS measurement takes = 155us */
	Si114x_SetMeasurmentRate(3200);

	Si114x_PsX_Led_Select_t PsXLedSelect;
	PsXLedSelect.PS1_Led = SI114X_LED1_ENABLE;
	PsXLedSelect.PS2_Led = SI114X_LED2_ENABLE;
	PsXLedSelect.PS3_Led = SI114X_LED3_ENABLE;
	Si114x_SetPsLedSelect(PsXLedSelect);

	/* The encoding is set to 0 by default */
	/* Si114x_SetPsEncoding(0,0,0); */
	/* Si114x_SetAlsEncoding(0,0); */

	/* MUX settings */
	/* TODO This should work with IR diodes, but currently i dont have any
	 * so i used RED Leds so the PS measurements are made using a visible
	 * photodiode */
	Si114x_SetPs1ADC(SI114X_PS_ADC_VISIBLE_PHOTODIODE);
	Si114x_SetPs2ADC(SI114X_PS_ADC_VISIBLE_PHOTODIODE);
	Si114x_SetPs3ADC(SI114X_PS_ADC_VISIBLE_PHOTODIODE);
	/* Si114x_SetAuxADC() This is not set, the default values is working ok */

	/* Enable interrupts */
	// Si114x_SetIntCfg(SI114X_INT_OE_ENABLE);

	/* Enable measurement interrupts */
	//Si114x_SetIrqEnable(SI114X_PS3_IE_DISABLE, SI114X_PS2_IE_DISABLE,
	//					SI114X_PS1_IE_ENABLE , SI114X_ALS_IE_DISABLE);

	/* Set the led currents */
	Si114x_SetLedCurrents(0x0A,0x0A,0x0A);

	/* 0x07 = 511 ADC Clocks = 25.55us */
	/* 0x07 is the default value */
	Si114x_SetPsAdcCounter(0x07);
	Si114x_SetAlsVisAdcCounter(0x07);
	Si114x_SetAlsIrAdcCounter(0x07);

	/* Set Normal Signal Range on PS measurement  	 (0b)
	 * and Normal Proximity Measurement mode         (1b) */
	Si114x_SetPsAdcMisc(1, 1);

	/* Default values set to 1 ADC clock = 25.55us */
	/* This ensures low power consumption but the detection distance is reduced */
	Si114x_SetPsAdcGain(0x00, 0);
	Si114x_SetAlsVisAdcGain(0x00);
	Si114x_SetAlsIrAdcGain(0x00);

	Si114x_SetCommand(SI114X_CMD_PS_AUTO, 0);
}

void Si114x_Interrupt(void)
{
#ifdef DEBUG
	LeUart_SendChar('s');
	LeUart_SendChar('i');
#endif
	Si114x_ClrIrqStatus();
}

void Si114x_ForceAmbientLightMeasurment(void)
{
	/* Force a single ALS measurement */
	Si114x_SetCommand(SI114X_CMD_ALS_FORCE, 0);
}

uint8_t Si114x_ReadAmbientLight(uint16_t *AmbientLight)
{
	uint8_t response = 0;
	response = Si114x_GetResponse();

	if((response & 0xF0) != 0)
	{
		/* ----there was an error---- */
#ifdef DEBUG
		char buff[30];
		sprintf(buff, "\n\t\t\tSI114x ALS meas Error=0x%02x\n", response);
		LeUart_SendText(buff);
#endif

		/* send to the sequencer RESET cmd */
		Si114x_SetCommand(SI114X_CMD_NOP, 0);
		return -1;
	}
	else
	{
		/* ----no error occurred---- */
		*AmbientLight = Si114x_GetAlsVisData();
	}
	return 0;
}

void Si114x_ForceUvIndexMeasurment(void)
{
	Si114x_EnableUvReading();
	/* Force a single ALS measurement */
	Si114x_SetCommand(SI114X_CMD_ALS_FORCE, 0);
}

uint8_t Si114x_ReadUvIndex(uint16_t *UvIndex)
{
	uint8_t response = 0;
	uint8_t ret = 0;
	response = Si114x_GetResponse();

	if((response & 0xF0) != 0)
	{
		/* ----there was an error---- */
#ifdef DEBUG
		char buff[30];
		sprintf(buff, "\n\t\t\tSI114x UV meas Error=0x%02x\n", response);
		LeUart_SendText(buff);
#endif

		/* send to the sequencer RESET cmd */
		Si114x_SetCommand(SI114X_CMD_NOP, 0);
		ret = -1;
	}
	else
	{
		/* ----no error occurred---- */
		*UvIndex = Si114x_GetAuxUvData();
	}

	Si114x_DisableUvReading();

	return(ret);
}

uint8_t Si114x_ReadPsMeasurments(uint16_t *PS1, uint16_t *PS2, uint16_t *PS3)
{
	uint8_t response = 0;
	uint8_t ret = 0;
	response = Si114x_GetResponse();

	if((response & 0xF0) != 0)
	{
		/* ----there was an error---- */
#ifdef DEBUG
		char buff[30];
		sprintf(buff, "\n\t\t\tSI114x UV meas Error=0x%02x\n", response);
		LeUart_SendText(buff);
#endif

		/* send to the sequencer RESET cmd */
		Si114x_SetCommand(SI114X_CMD_NOP, 0);
		ret = -1;
	}
	else
	{
		/* ----no error occurred---- */
		*PS1 = Si114x_GetPs1Data();
		*PS2 = Si114x_GetPs2Data();
		*PS3 = Si114x_GetPs3Data();
	}

	Si114x_DisableUvReading();

	return(ret);
}
