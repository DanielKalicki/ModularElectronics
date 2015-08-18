/*
 * ADP1650.c
 *
 *  Created on: 31-07-2015
 *      Author: terg
 */

#include "ADP1650.h"
#include "em_gpio.h"
#include "..\ucPeripheralDrivers\i2c_connection.h"
#include "em_gpio.h"

#define ADP1650_ADDR	(0x30*2)

/* Global array containing the current registers values */
static uint8_t ADP1650_Reg_Stat[10];

/*--------------------------------------------
 * 				Registers access
 *-------------------------------------------*/

static typedef enum {
	ADP1650_DESIGN_INFORMATION_REG  	= 0x00,
	APD1650_VREF_TIMER_REG				= 0x02,
	ADP1650_CURRENT_SET_REG				= 0x03,
	ADP1650_OUTPUT_MODE_REG				= 0x04,
	ADP1650_FAULT_INFORMATION_REG		= 0x05,
	ADP1650_INPUT_CONTROL_REG			= 0x06,
	ADP1650_ADDITIONAL_MODE_ADMOD_REG	= 0x07,
	ADP1650_ADDITIONAL_MODE_ADC_REG		= 0x08,
	ADP1650_BATTERY_LOW_MODE_REG		= 0x09

} ADP1650_Registers_t;

static void ADP1650_SetRegister(ADP1650_Registers_t ADP1650_reg, uint8_t val)
{
	i2c_RegisterSet(I2C0, ADP1650_ADDR, (uint8_t)ADP1650_reg, val);
}

static void ADP1650_ReadRegister(ADP1650_Registers_t ADP1650_reg, uint8_t *val)
{
	i2c_RegisterGet(I2C0, ADP1650_ADDR, (uint8_t)ADP1650_reg, val);
}

/*--------------------------------------------
 * 		APD1650_VREF_TIMER_REG setting
 *-------------------------------------------*/

/*--------------- GPIO 1 ----------------*/
static typedef enum
{
	ADP1650_GPIO1_HIGH_IMP			= 0,
	ADP1650_GPIO1_INDICATOR_LED		= 1,
	ADP1650_GPIO1_TxMASK2_MODE		= 2,
	APD1650_GPIO1_RESERVED			= 3
} ADP1650_GPIO1_conf_state_t;

#define ADP1650_GPIO1_SET_MASK	0x30
#define ADP1650_GPIO1_SET_SHIFT	4

static void ADP1650_SetGPIO1(ADP1650_GPIO1_conf_state_t GPIO1_State)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_DESIGN_INFORMATION_REG] & !ADP1650_GPIO1_SET_MASK)
			    | ((uint8_t)GPIO1_State << ADP1650_GPIO1_SET_SHIFT);
	ADP1650_SetRegister(ADP1650_DESIGN_INFORMATION_REG, val);
	ADP1650_Reg_Stat[ADP1650_DESIGN_INFORMATION_REG] = val;
}

/*--------------- GPIO 2 ----------------*/
static typedef enum
{
	ADP1650_GPIO2_HIGH_IMP			= 0,
	ADP1650_GPIO2_INDICATOR_LED		= 1,
	ADP1650_GPIO2_TxMASK2_MODE		= 2,
	APD1650_GPIO2_ANALOG_INPUT		= 3
} ADP1650_GPIO2_conf_state_t;

#define ADP1650_GPIO2_SET_MASK	0xC0
#define ADP1650_GPIO2_SET_SHIFT	6

static void ADP1650_SetGPIO2(ADP1650_GPIO2_conf_state_t GPIO2_State)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_DESIGN_INFORMATION_REG] & !ADP1650_GPIO2_SET_MASK)
			    | ((uint8_t)GPIO2_State << ADP1650_GPIO2_SET_SHIFT);
	ADP1650_SetRegister(ADP1650_DESIGN_INFORMATION_REG, val);
	ADP1650_Reg_Stat[ADP1650_DESIGN_INFORMATION_REG] = val;
}

/*--------------- Flash Time ----------------*/
static typedef enum
{
	ADP1650_FLASH_TIME_100MS	= 0,
	ADP1650_FLASH_TIME_200MS	= 1,
	ADP1650_FLASH_TIME_300MS	= 2,
	ADP1650_FLASH_TIME_400MS	= 3,
	ADP1650_FLASH_TIME_500MS	= 4,
	ADP1650_FLASH_TIME_600MS	= 5,
	ADP1650_FLASH_TIME_700MS	= 6,
	ADP1650_FLASH_TIME_800MS	= 7,
	ADP1650_FLASH_TIME_900MS	= 8,
	ADP1650_FLASH_TIME_1000MS	= 9,
	ADP1650_FLASH_TIME_1100MS	= 10,
	ADP1650_FLASH_TIME_1200MS	= 11,
	ADP1650_FLASH_TIME_1300MS	= 12,
	ADP1650_FLASH_TIME_1400MS	= 13,
	ADP1650_FLASH_TIME_1500MS	= 14,
	ADP1650_FLASH_TIME_1600MS	= 15
} ADP1650_Flash_Timer_t;

#define ADP1650_FLASH_TIMER_MASK	0x0F
#define ADP1650_FLASH_TIMER_SHIFT	0

static void ADP1650_SetFlashTimer(ADP1650_Flash_Timer_t Flash_Timer)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_DESIGN_INFORMATION_REG] & !ADP1650_FLASH_TIMER_MASK)
			    | ((uint8_t)Flash_Timer << ADP1650_FLASH_TIMER_SHIFT);
	ADP1650_SetRegister(ADP1650_DESIGN_INFORMATION_REG, val);
	ADP1650_Reg_Stat[ADP1650_DESIGN_INFORMATION_REG] = val;
}

/*--------------------------------------------
 * 		ADP1650_CURRENT_SET_REG setting
 *-------------------------------------------*/

/*-------------- Flash Current ---------------*/
static typedef enum
{
	ADP1650_FLASH_CURRENT_300mA		= 0,
	ADP1650_FLASH_CURRENT_350mA		= 1,
	ADP1650_FLASH_CURRENT_400mA		= 2,
	ADP1650_FLASH_CURRENT_450mA		= 3,
	ADP1650_FLASH_CURRENT_500mA		= 4,
	ADP1650_FLASH_CURRENT_550mA		= 5,
	ADP1650_FLASH_CURRENT_600mA		= 6,
	ADP1650_FLASH_CURRENT_650mA		= 7,
	ADP1650_FLASH_CURRENT_700mA		= 8,
	ADP1650_FLASH_CURRENT_750mA		= 9,
	ADP1650_FLASH_CURRENT_800mA		= 10,
	ADP1650_FLASH_CURRENT_850mA		= 11,
	ADP1650_FLASH_CURRENT_900mA		= 12,
	ADP1650_FLASH_CURRENT_950mA		= 13,
	ADP1650_FLASH_CURRENT_1000mA	= 14,
	ADP1650_FLASH_CURRENT_1050mA	= 15,
	ADP1650_FLASH_CURRENT_1100mA	= 16,
	ADP1650_FLASH_CURRENT_1150mA	= 17,
	ADP1650_FLASH_CURRENT_1200mA	= 18,
	ADP1650_FLASH_CURRENT_1250mA	= 19,
	ADP1650_FLASH_CURRENT_1300mA	= 20,
	ADP1650_FLASH_CURRENT_1350mA	= 21,
	ADP1650_FLASH_CURRENT_1400mA	= 22,
	ADP1650_FLASH_CURRENT_1450mA	= 23,
	ADP1650_FLASH_CURRENT_1500mA	= 24
} ADP1650_Flash_Current_t;

#define ADP1650_FLASH_CURRENT_MASK	0xF8
#define ADP1650_FLASH_CURRENT_SHIFT	3

static void ADP1650_SetFlashCurrent(ADP1650_Flash_Current_t Flash_Current)
{
	uint8_t val = (ADP1650_Reg_Stat[APD1650_VREF_TIMER_REG] & !ADP1650_FLASH_CURRENT_MASK)
			    | ((uint8_t)Flash_Current << ADP1650_FLASH_CURRENT_SHIFT);
	ADP1650_SetRegister(APD1650_VREF_TIMER_REG, val);
	ADP1650_Reg_Stat[APD1650_VREF_TIMER_REG] = val;
}

/*------------- Torch Current --------------*/
static typedef enum
{
	ADP1650_TORCH_CURRENT_25mA		= 0,
	ADP1650_TORCH_CURRENT_50mA		= 1,
	ADP1650_TORCH_CURRENT_75mA		= 2,
	ADP1650_TORCH_CURRENT_100mA		= 3,
	ADP1650_TORCH_CURRENT_125mA		= 4,
	ADP1650_TORCH_CURRENT_150mA		= 5,
	ADP1650_TORCH_CURRENT_175mA		= 6,
	ADP1650_TORCH_CURRENT_200mA		= 7,
} ADP1650_Torch_Current_t;

#define ADP1650_TORCH_CURRENT_MASK	0x07
#define ADP1650_TORCH_CURRENT_SHIFT	0

static void ADP1650_SetTorchCurrent(ADP1650_Torch_Current_t Torch_Current)
{
	uint8_t val = (ADP1650_Reg_Stat[APD1650_VREF_TIMER_REG] & !ADP1650_TORCH_CURRENT_MASK)
			    | ((uint8_t)Torch_Current << ADP1650_TORCH_CURRENT_SHIFT);
	ADP1650_SetRegister(APD1650_VREF_TIMER_REG, val);
	ADP1650_Reg_Stat[APD1650_VREF_TIMER_REG] = val;
}

/*--------------------------------------------
 * 		ADP1650_OUTPUT_MODE_REG setting
 *-------------------------------------------*/

/*--------- Inductor Peak Current -----------*/
static typedef enum
{
	ADP1650_INDUCTOR_PEAK_CURRENT_1750mA		= 0,
	ADP1650_INDUCTOR_PEAK_CURRENT_2250mA		= 1,
	ADP1650_INDUCTOR_PEAK_CURRENT_2750mA		= 2,
	ADP1650_INDUCTOR_PEAK_CURRENT_3000mA		= 3,
} ADP1650_Inductor_Peak_Current_t;

#define ADP1650_INDUCTOR_PEAK_CURRENT_MASK	0xC0
#define ADP1650_INDUCTOR_PEAK_CURRENT_SHIFT	6

static void ADP1650_SetInductorPeakCurrent(ADP1650_Inductor_Peak_Current_t Inductor_Peak_Current)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] & !ADP1650_INDUCTOR_PEAK_CURRENT_MASK)
			    | ((uint8_t)Inductor_Peak_Current << ADP1650_INDUCTOR_PEAK_CURRENT_SHIFT);
	ADP1650_SetRegister(ADP1650_OUTPUT_MODE_REG, val);
	ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] = val;
}

/*----------- Strobe Sense -------------*/
static typedef enum
{
	ADP1650_STROBE_EDGE_SENSITIVE		= 0,
	ADP1650_STROBE_LEVEL_SENSITIVE		= 1,
} ADP1650_Strobe_Sense_t;

#define ADP1650_STROBE_SENSE_MASK	0x20
#define ADP1650_STROBE_SENSE_SHIFT	5

static void ADP1650_SetStrobeSense(ADP1650_Strobe_Sense_t Strobe_Sense)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] & !ADP1650_STROBE_SENSE_MASK)
			    | ((uint8_t)Strobe_Sense << ADP1650_STROBE_SENSE_SHIFT);
	ADP1650_SetRegister(ADP1650_OUTPUT_MODE_REG, val);
	ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] = val;
}

/*---------- Frequency feedback ---------*/
static typedef enum
{
	ADP1650_FREQUENCY_FEEDBACK_NOT_ALLOWED	= 0,
	ADP1650_FREQUENCY_FEEDBACK_ALLOWED		= 1,
} ADP1650_Frequency_Feedback_t;

#define ADP1650_FREQUENCY_FEEDBACK_MASK		0x10
#define ADP1650_FREQUENCY_FEEDBACK_SHIFT 	4

static void ADP1650_SetFrequencyFeedback(ADP1650_Frequency_Feedback_t Frequency_Feedback)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] & !ADP1650_FREQUENCY_FEEDBACK_MASK)
			    | ((uint8_t)Frequency_Feedback << ADP1650_FREQUENCY_FEEDBACK_SHIFT);
	ADP1650_SetRegister(ADP1650_OUTPUT_MODE_REG, val);
	ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] = val;
}

/*---------- Output enable ---------*/
static typedef enum
{
	ADP1650_OUTPUT_OFF		= 0,
	ADP1650_OUTPUT_ON		= 1,
} ADP1650_Output_Enable_t;

#define ADP1650_OUTPUT_ENABLE_MASK		0x08
#define ADP1650_OUTPUT_ENABLE_SHIFT 	3

static void ADP1650_SetOutputEnable(ADP1650_Output_Enable_t Output_Enable)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] & !ADP1650_OUTPUT_ENABLE_MASK)
			    | ((uint8_t)Output_Enable << ADP1650_OUTPUT_ENABLE_SHIFT);
	ADP1650_SetRegister(ADP1650_OUTPUT_MODE_REG, val);
	ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] = val;
}

void ADP1650_Init(void)
{
	for (int i = 0; i < 10; i++)
	{
		ADP1650_ReadRegister((ADP1650_Registers_t)i, &ADP1650_Reg_Stat[i]);
	}

	ADP1650_SetGPIO1(ADP1650_GPIO1_HIGH_IMP);
	ADP1650_SetGPIO2(ADP1650_GPIO2_HIGH_IMP);
	ADP1650_SetFlashTimer(ADP1650_FLASH_TIME_500MS);

	ADP1650_SetFlashCurrent(ADP1650_FLASH_CURRENT_400mA);
	ADP1650_SetTorchCurrent(ADP1650_TORCH_CURRENT_100mA);

	ADP1650_SetInductorPeakCurrent(ADP1650_INDUCTOR_PEAK_CURRENT_1750mA);
	ADP1650_SetStrobeSense(ADP1650_STROBE_LEVEL_SENSITIVE);
	ADP1650_SetFrequencyFeedback(ADP1650_FREQUENCY_FEEDBACK_NOT_ALLOWED);
	ADP1650_SetOutputEnable();
}
