/*
 * Si114x.c
 *
 *  Created on: 18-07-2015
 *      Author: terg
 */

#include "Si114x.h"
#include "..\ucPeripheralDrivers\i2c_connection.h"
#include "..\ucPeripheralDrivers\leuart_connection.h"

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

static void SI114X_GetRegister(SI114x_Registers_t Si114x_reg, uint8_t *val)
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
	SI114X_GetRegister(SI114X_PART_ID_REG, &ret);
	return(ret);
}

/* -------------- REV ID ---------------*/
uint8_t Si114x_GetRevId(void)
{
	uint8_t ret = 0;
	SI114X_GetRegister(SI114X_REV_ID_REG, &ret);
	return(ret);
}

/* -------------- SEQ ID ---------------*/
uint8_t Si114x_GetSeqId(void)
{
	uint8_t ret = 0;
	SI114X_GetRegister(SI114X_SEQ_ID_REG, &ret);
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
void Si114x_SetHW(void)
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
*/
void Si114x_SetMeasurmentRate(uint16_t measRate)
{
	Si114x_SetRegister(SI114X_MEAS_RATE0_REG, (uint8_t)((measRate >> 8) & 0x0F));
	Si114x_SetRegister(SI114X_MEAS_RATE1_REG, (uint8_t)(measRate));
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
}

/* -------------- SI114X_UCOEF0 -------------- */

/* --------------------------------------------- */
/*												 */
/* 		 IC related function - higher level 	 */
/*												 */
/* --------------------------------------------- */
uint8_t Si114x_Detect(void)
{
	return i2c_Detect(I2C0, SI114x_ADDR);
}

void Si114x_Init(void)
{
	i2c_RegisterSet(I2C0, SI114x_ADDR,0x07,0x17);

	/* turn on ALS_VIS measurement */
	i2c_RegisterSet(I2C0, SI114x_ADDR,0x17,0x10);	/* write to PARAM reg */
	i2c_RegisterSet(I2C0, SI114x_ADDR,0x18,0xA1);	/* write to COMAND reg */

	/* ALS_VIS_ADC_GAIN is set to 1 by default */
	/* ALS_VIS_ADC_MISC is set to Normal Signal Range by default */
}

void Si114x_ForceAmbientLightMeasurment(void)
{
	/* Force a single ALS measurement */
	i2c_RegisterSet(I2C0, SI114x_ADDR, 0x18, 0x06);	/* write to COMAND reg */
}

uint8_t Si114x_ReadAmbientLight(uint16_t *AmbientLight)
{
	uint8_t response = 0;
	i2c_RegisterGet(I2C0, SI114x_ADDR, 0x20, &response);

	if((response&0xF0) != 0){
		/* ----there were an error---- */
#ifdef DEBUG
		char buff[30];
		sprintf(buff, "SI114x Error=%d\n", response);
		LeUart_SendText(buff);
#endif

		i2c_RegisterSet(I2C0, SI114x_ADDR, 0x18, 0x01);//send to the sequencer RESET cmd
		return -1;
	}
	else
	{
		/* ----no error occured---- */
		uint8_t ALS_VIS_l = 0;
		i2c_RegisterGet(I2C0, SI114x_ADDR, 0x22, &ALS_VIS_l);	//TODO read block using i2c
		uint8_t ALS_VIS_h = 0;
		i2c_RegisterGet(I2C0, SI114x_ADDR, 0x23, &ALS_VIS_h);

		*AmbientLight = (((uint16_t)ALS_VIS_h) << 8) + ALS_VIS_l;
	}
	return 0;
}
