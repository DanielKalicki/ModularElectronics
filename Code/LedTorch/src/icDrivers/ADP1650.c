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

#ifdef DEBUG
#include "..\ucPeripheralDrivers\uart_connection.h"
#include <stdio.h>
#endif

#define ADP1650_ADDR	(0x30*2)

/* Global array containing the current registers values */
static uint8_t ADP1650_Reg_Stat[10];
static ADP1650_GPIO_Settings_t ADP1650_uC_pins;

/*--------------------------------------------
 * 				Registers access
 *-------------------------------------------*/

typedef enum {
	ADP1650_DESIGN_INFORMATION_REG  	= 0x00,
	ADP1650_VREF_TIMER_REG				= 0x02,
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

static void ADP1650_GetRegister(ADP1650_Registers_t ADP1650_reg, uint8_t *val)
{
	i2c_RegisterGet(I2C0, ADP1650_ADDR, (uint8_t)ADP1650_reg, val);
}

/*--------------------------------------------
 * 		ADP1650_VREF_TIMER_REG setting
 *-------------------------------------------*/

/*--------------- GPIO 1 ----------------*/
typedef enum
{
	ADP1650_GPIO1_HIGH_IMP			= 0,
	ADP1650_GPIO1_TORCH				= 1,
	ADP1650_GPIO1_TxMASK1_MODE		= 2,
	APD1650_GPIO1_RESERVED			= 3
} ADP1650_GPIO1_conf_state_t;

#define ADP1650_GPIO1_SET_MASK	0x30
#define ADP1650_GPIO1_SET_SHIFT	4

static void ADP1650_SetGPIO1(ADP1650_GPIO1_conf_state_t GPIO1_State)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_VREF_TIMER_REG] & ~ADP1650_GPIO1_SET_MASK)
			    | ((uint8_t)GPIO1_State << ADP1650_GPIO1_SET_SHIFT);
	ADP1650_SetRegister(ADP1650_VREF_TIMER_REG, val);
	ADP1650_Reg_Stat[ADP1650_VREF_TIMER_REG] = val;
}

/*--------------- GPIO 2 ----------------*/
typedef enum
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
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_VREF_TIMER_REG] & ~ADP1650_GPIO2_SET_MASK)
			    | ((uint8_t)GPIO2_State << ADP1650_GPIO2_SET_SHIFT);
	ADP1650_SetRegister(ADP1650_VREF_TIMER_REG, val);
	ADP1650_Reg_Stat[ADP1650_VREF_TIMER_REG] = val;
}

/*--------------- Flash Time ----------------*/

			/* Global function */

#define ADP1650_FLASH_TIMER_MASK	0x0F
#define ADP1650_FLASH_TIMER_SHIFT	0

void ADP1650_SetFlashTimer(ADP1650_Flash_Timer_t Flash_Timer)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_VREF_TIMER_REG] & ~ADP1650_FLASH_TIMER_MASK)
			    | ((uint8_t)Flash_Timer << ADP1650_FLASH_TIMER_SHIFT);
	ADP1650_SetRegister(ADP1650_VREF_TIMER_REG, val);
	ADP1650_Reg_Stat[ADP1650_VREF_TIMER_REG] = val;
}

/*--------------------------------------------
 * 		ADP1650_CURRENT_SET_REG setting
 *-------------------------------------------*/

/*-------------- Flash Current ---------------*/

			/* Global function */

#define ADP1650_FLASH_CURRENT_MASK	0xF8
#define ADP1650_FLASH_CURRENT_SHIFT	3

void ADP1650_SetFlashCurrent(ADP1650_Flash_Current_t Flash_Current)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_CURRENT_SET_REG] & ~ADP1650_FLASH_CURRENT_MASK)
			    | ((uint8_t)Flash_Current << ADP1650_FLASH_CURRENT_SHIFT);
	ADP1650_SetRegister(ADP1650_CURRENT_SET_REG, val);
	ADP1650_Reg_Stat[ADP1650_CURRENT_SET_REG] = val;
}

/*------------- Torch Current --------------*/

			/* Global function */

#define ADP1650_TORCH_CURRENT_MASK	0x07
#define ADP1650_TORCH_CURRENT_SHIFT	0

void ADP1650_SetTorchCurrent(ADP1650_Torch_Current_t Torch_Current)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_CURRENT_SET_REG] & ~ADP1650_TORCH_CURRENT_MASK)
			    | ((uint8_t)Torch_Current << ADP1650_TORCH_CURRENT_SHIFT);
	ADP1650_SetRegister(ADP1650_CURRENT_SET_REG, val);
	ADP1650_Reg_Stat[ADP1650_CURRENT_SET_REG] = val;
}

/*--------------------------------------------
 * 		ADP1650_OUTPUT_MODE_REG setting
 *-------------------------------------------*/

/*--------- Inductor Peak Current -----------*/
typedef enum
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
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] & ~ADP1650_INDUCTOR_PEAK_CURRENT_MASK)
			    | ((uint8_t)Inductor_Peak_Current << ADP1650_INDUCTOR_PEAK_CURRENT_SHIFT);
	ADP1650_SetRegister(ADP1650_OUTPUT_MODE_REG, val);
	ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] = val;
}

/*----------- Strobe Sense -------------*/
typedef enum
{
	ADP1650_STROBE_EDGE_SENSITIVE		= 0,
	ADP1650_STROBE_LEVEL_SENSITIVE		= 1,
} ADP1650_Strobe_Sense_t;

#define ADP1650_STROBE_SENSE_MASK	0x20
#define ADP1650_STROBE_SENSE_SHIFT	5

static void ADP1650_SetStrobeSense(ADP1650_Strobe_Sense_t Strobe_Sense)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] & ~ADP1650_STROBE_SENSE_MASK)
			    | ((uint8_t)Strobe_Sense << ADP1650_STROBE_SENSE_SHIFT);
	ADP1650_SetRegister(ADP1650_OUTPUT_MODE_REG, val);
	ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] = val;
}

/*---------- Frequency feedback ---------*/
typedef enum
{
	ADP1650_FREQUENCY_FEEDBACK_NOT_ALLOWED	= 0,
	ADP1650_FREQUENCY_FEEDBACK_ALLOWED		= 1,
} ADP1650_Frequency_Feedback_t;

#define ADP1650_FREQUENCY_FEEDBACK_MASK		0x10
#define ADP1650_FREQUENCY_FEEDBACK_SHIFT 	4

static void ADP1650_SetFrequencyFeedback(ADP1650_Frequency_Feedback_t Frequency_Feedback)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] & ~ADP1650_FREQUENCY_FEEDBACK_MASK)
			    | ((uint8_t)Frequency_Feedback << ADP1650_FREQUENCY_FEEDBACK_SHIFT);
	ADP1650_SetRegister(ADP1650_OUTPUT_MODE_REG, val);
	ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] = val;
}

/*---------- Output enable ---------*/
typedef enum
{
	ADP1650_OUTPUT_OFF		= 0,
	ADP1650_OUTPUT_ON		= 1,
} ADP1650_Output_Enable_t;

#define ADP1650_OUTPUT_ENABLE_MASK		0x08
#define ADP1650_OUTPUT_ENABLE_SHIFT 	3

void ADP1650_SetOutputEnable(ADP1650_Output_Enable_t Output_Enable)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] & ~ADP1650_OUTPUT_ENABLE_MASK)
			    | ((uint8_t)Output_Enable << ADP1650_OUTPUT_ENABLE_SHIFT);
	ADP1650_SetRegister(ADP1650_OUTPUT_MODE_REG, val);
	ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] = val;
}

/*---------- Strobe mode ---------*/
typedef enum
{
	ADP1650_SOFTWARE_STROBE_MODE	= 0,
	ADP1650_HARDWARE_STROBE_MODE	= 1,
} ADP1650_Strobe_Mode_t;

#define ADP1650_STROBE_MODE_MASK	0x04
#define ADP1650_STROBE_MODE_SHIFT 	2

void ADP1650_SetStrobeMode(ADP1650_Strobe_Mode_t Strobe_Mode)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] & ~ADP1650_STROBE_MODE_MASK)
			    | ((uint8_t)Strobe_Mode << ADP1650_STROBE_MODE_SHIFT);
	ADP1650_SetRegister(ADP1650_OUTPUT_MODE_REG, val);
	ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] = val;
}

/*---------- LED Output mode ---------*/
typedef enum
{
	ADP1650_LED_MODE_STANDBY		= 0,
	ADP1650_LED_MODE_VOUT_5V		= 1,
	ADP1650_LED_MODE_ASSIST_LIGHT	= 2,
	ADP1650_LED_MODE_FLASH			= 3
} ADP1650_LED_Output_Mode_t;

#define ADP1650_LED_OUTPUT_MODE_MASK	0x03
#define ADP1650_LED_OUTPUT_MODE_SHIFT 	0

void ADP1650_SetLedOutputMode(ADP1650_LED_Output_Mode_t LED_Mode)
{

	uint8_t val = (ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] & ~ADP1650_LED_OUTPUT_MODE_MASK)
			    | ((uint8_t)LED_Mode << ADP1650_LED_OUTPUT_MODE_SHIFT);

	ADP1650_SetRegister(ADP1650_OUTPUT_MODE_REG, val);
	ADP1650_Reg_Stat[ADP1650_OUTPUT_MODE_REG] = val;
}

/*--------------------------------------------
 * 	ADP1650_ADDITIONAL_MODE_ADMOD_REG setting
 *-------------------------------------------*/

/*---------- Dynamic OVP ---------*/
typedef enum
{
	ADP1650_DYNAMIC_OVP_OFF	= 0,
	ADP1650_DYNAMIC_OVP_ON	= 1,
} ADP1650_Dynamic_OVP_t;

#define ADP1650_DYNAMIC_OVP_MASK	0x80
#define ADP1650_DYNAMIC_OVP_SHIFT 	7

void ADP1650_SetDynamicOVP(ADP1650_Dynamic_OVP_t Dynamic_OVP)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_ADDITIONAL_MODE_ADMOD_REG] & ~ADP1650_DYNAMIC_OVP_MASK)
			    | ((uint8_t)Dynamic_OVP << ADP1650_DYNAMIC_OVP_SHIFT);
	ADP1650_SetRegister(ADP1650_ADDITIONAL_MODE_ADMOD_REG, val);
	ADP1650_Reg_Stat[ADP1650_ADDITIONAL_MODE_ADMOD_REG] = val;
}

/*----------- Switching Frequency -------------*/
typedef enum
{
	ADP1650_SWITCHING_FREQUENCY_DISABLED	= 0,
	ADP1650_SWITCHING_FREQUENCY_ENABLED		= 1,
} ADP1650_Switching_Frequency_t;

#define ADP1650_SWITCHING_FREQUENCY_MASK	0x40
#define ADP1650_SWITCHING_FREQUENCY_SHIFT	6

static void ADP1650_SetSwitchingFrequency(ADP1650_Switching_Frequency_t Switching_Frequency)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_ADDITIONAL_MODE_ADMOD_REG] & ~ADP1650_SWITCHING_FREQUENCY_MASK)
			    | ((uint8_t)Switching_Frequency << ADP1650_SWITCHING_FREQUENCY_SHIFT);
	ADP1650_SetRegister(ADP1650_ADDITIONAL_MODE_ADMOD_REG, val);
	ADP1650_Reg_Stat[ADP1650_ADDITIONAL_MODE_ADMOD_REG] = val;
}

/*----------- Strobe polarity -------------*/
typedef enum
{
	ADP1650_STROBE_POLARITY_LOW		= 0,
	ADP1650_STROBE_POLARITY_HIGH	= 1,
} ADP1650_Strobe_Polarity_t;

#define ADP1650_STROBE_POLARITY_MASK	0x20
#define ADP1650_STROBE_POLARITY_SHIFT	5

static void ADP1650_SetStrobePolarity(ADP1650_Strobe_Polarity_t Strobe_Polarity)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_ADDITIONAL_MODE_ADMOD_REG] & ~ADP1650_STROBE_POLARITY_MASK)
			    | ((uint8_t)Strobe_Polarity << ADP1650_STROBE_POLARITY_SHIFT);
	ADP1650_SetRegister(ADP1650_ADDITIONAL_MODE_ADMOD_REG, val);
	ADP1650_Reg_Stat[ADP1650_ADDITIONAL_MODE_ADMOD_REG] = val;
}

/*--------- Indicator LED Current -----------*/
typedef enum
{
	ADP1650_INDICATOR_LED_CURRENT_2_75_mA	= 0,
	ADP1650_INDICATOR_LED_CURRENT_5_5_mA	= 1,
	ADP1650_INDICATOR_LED_CURRENT_8_25_mA	= 2,
	ADP1650_INDICATOR_LED_CURRENT_11_mA		= 3,
} ADP1650_Indicator_LED_Current_t;

#define ADP1650_INDICATOR_LED_CURRENT_MASK	0x18
#define ADP1650_INDICATOR_LED_CURRENT_SHIFT	3

static void ADP1650_SetIndicatorLedCurrent(ADP1650_Indicator_LED_Current_t Indicator_LED_Current)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_ADDITIONAL_MODE_ADMOD_REG] & ~ADP1650_INDICATOR_LED_CURRENT_MASK)
			    | ((uint8_t)Indicator_LED_Current << ADP1650_INDICATOR_LED_CURRENT_SHIFT);
	ADP1650_SetRegister(ADP1650_ADDITIONAL_MODE_ADMOD_REG, val);
	ADP1650_Reg_Stat[ADP1650_ADDITIONAL_MODE_ADMOD_REG] = val;
}

/*--------- Input DC current limit -----------*/
typedef enum
{
	ADP1650_INPUT_DC_CURRENT_LIMIT_1500mA	= 0,
	ADP1650_INPUT_DC_CURRENT_LIMIT_1750mA	= 1,
	ADP1650_INPUT_DC_CURRENT_LIMIT_2000mA	= 2,
	ADP1650_INPUT_DC_CURRENT_LIMIT_2250mA	= 3,
} ADP1650_Input_DC_Current_Limit_t;

#define ADP1650_INPUT_DC_CURRENT_LIMIT_MASK		0x06
#define ADP1650_INPUT_DC_CURRENT_LIMIT_SHIFT	1

static void ADP1650_SetInputDcCurrentLimit(ADP1650_Input_DC_Current_Limit_t Input_DC_Current_Limit)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_ADDITIONAL_MODE_ADMOD_REG] & ~ADP1650_INPUT_DC_CURRENT_LIMIT_MASK)
			    | ((uint8_t)Input_DC_Current_Limit << ADP1650_INPUT_DC_CURRENT_LIMIT_SHIFT);
	ADP1650_SetRegister(ADP1650_ADDITIONAL_MODE_ADMOD_REG, val);
	ADP1650_Reg_Stat[ADP1650_ADDITIONAL_MODE_ADMOD_REG] = val;
}

/*----------- Strobe polarity -------------*/
typedef enum
{
	ADP1650_INPUT_DC_CURRENT_LIMIT_DISABLE	= 0,
	ADP1650_INPUT_DC_CURRENT_LIMIT_ENABLE	= 1,
} ADP1650_Input_DC_Current_Limit_Enable_t;

#define ADP1650_INPUT_DC_CURRENT_LIMIT_ENABLE_MASK	0x01
#define ADP1650_INPUT_DC_CURRENT_LIMIT_ENABLE_SHIFT	0

static void ADP1650_SetInputDcCurrentLimitEnable(ADP1650_Input_DC_Current_Limit_Enable_t Input_DC_Current_Limit_Enable)
{
	uint8_t val = (ADP1650_Reg_Stat[ADP1650_ADDITIONAL_MODE_ADMOD_REG] & ~ADP1650_INPUT_DC_CURRENT_LIMIT_ENABLE_MASK)
			    | ((uint8_t)Input_DC_Current_Limit_Enable << ADP1650_INPUT_DC_CURRENT_LIMIT_ENABLE_SHIFT);
	ADP1650_SetRegister(ADP1650_ADDITIONAL_MODE_ADMOD_REG, val);
	ADP1650_Reg_Stat[ADP1650_ADDITIONAL_MODE_ADMOD_REG] = val;
}


// TODO add Set for all ADP1650 registers

void ADP1650_PrintRegisters(void)
{

	for (int i = 0; i < 10; i++)
	{
		ADP1650_GetRegister((ADP1650_Registers_t)i, &ADP1650_Reg_Stat[i]);
	}

	char buff[30];
	for (int i = 0; i < 10; i++)
	{
		sprintf(buff, "[0x%02x]: 0x%02x\t", i, ADP1650_Reg_Stat[i]);
		uart_sendText(buff);
	}
	uart_sendChar('\n');
	uart_sendChar('\n');
}

/*------- uC pin clear and set functions -------- */
static inline void ADP1650_GPIO1_Set()
{
	GPIO_PinOutSet(ADP1650_uC_pins.gpio1_port, ADP1650_uC_pins.gpio1_pin);

}
static inline void ADP1650_GPIO1_Clr()
{
	GPIO_PinOutClear(ADP1650_uC_pins.gpio1_port, ADP1650_uC_pins.gpio1_pin);
}

static inline void ADP1650_GPIO2_Set()
{
	GPIO_PinOutSet(ADP1650_uC_pins.gpio2_port, ADP1650_uC_pins.gpio2_pin);
}
static inline void ADP1650_GPIO2_Clr()
{
	GPIO_PinOutClear(ADP1650_uC_pins.gpio2_port, ADP1650_uC_pins.gpio2_pin);
}

static inline void ADP1650_Strobe_Set()
{
	GPIO_PinOutSet(ADP1650_uC_pins.strobe_port, ADP1650_uC_pins.strobe_pin);
}
static inline void ADP1650_Strobe_Clr()
{
	GPIO_PinOutClear(ADP1650_uC_pins.strobe_port, ADP1650_uC_pins.strobe_pin);
}

/*------- ADP1650 initialization -------- */

void ADP1650_Init(ADP1650_GPIO_Settings_t uC_pins)
{
	for (int i = 0; i < 10; i++)
	{
		ADP1650_GetRegister((ADP1650_Registers_t)i, &ADP1650_Reg_Stat[i]);
	}

	ADP1650_SetGPIO1(ADP1650_GPIO1_TORCH);
	ADP1650_SetGPIO2(ADP1650_GPIO2_HIGH_IMP);
	ADP1650_SetFlashTimer(ADP1650_FLASH_TIME_1600MS);

	ADP1650_SetFlashCurrent(ADP1650_FLASH_CURRENT_400mA);
	ADP1650_SetTorchCurrent(ADP1650_TORCH_CURRENT_200mA);

	ADP1650_SetInductorPeakCurrent(ADP1650_INDUCTOR_PEAK_CURRENT_1750mA);
	ADP1650_SetStrobeSense(ADP1650_STROBE_LEVEL_SENSITIVE);
	ADP1650_SetFrequencyFeedback(ADP1650_FREQUENCY_FEEDBACK_NOT_ALLOWED);
	ADP1650_SetOutputEnable(ADP1650_OUTPUT_OFF);
	ADP1650_SetStrobeMode(ADP1650_HARDWARE_STROBE_MODE);
	ADP1650_SetLedOutputMode(ADP1650_LED_MODE_STANDBY);

	ADP1650_SetDynamicOVP(ADP1650_DYNAMIC_OVP_OFF);
	ADP1650_SetSwitchingFrequency(ADP1650_SWITCHING_FREQUENCY_DISABLED);
	ADP1650_SetStrobePolarity(ADP1650_STROBE_POLARITY_HIGH);
	ADP1650_SetIndicatorLedCurrent(ADP1650_INDICATOR_LED_CURRENT_2_75_mA);
	ADP1650_SetInputDcCurrentLimit(ADP1650_INPUT_DC_CURRENT_LIMIT_1500mA);
	ADP1650_SetInputDcCurrentLimitEnable(ADP1650_INPUT_DC_CURRENT_LIMIT_DISABLE);

	/* Set uC pins */
	if (uC_pins.gpio1_state == ADP160_UC_PIN_INPUT)
	{
		GPIO_PinModeSet(uC_pins.gpio1_port, uC_pins.gpio1_pin, gpioModeInput, 1);
	}
	else if (uC_pins.gpio1_state == ADP160_UC_PIN_OUTPUT)
	{
		GPIO_PinModeSet(uC_pins.gpio1_port, uC_pins.gpio1_pin, gpioModePushPull, 1);
		ADP1650_GPIO1_Clr();
	}
	else
	{
		;
	}
	if (uC_pins.gpio2_state == ADP160_UC_PIN_INPUT)
	{
		GPIO_PinModeSet(uC_pins.gpio2_port, uC_pins.gpio2_pin, gpioModeInput, 1);
	}
	else if (uC_pins.gpio2_state == ADP160_UC_PIN_OUTPUT)
	{
		GPIO_PinModeSet(uC_pins.gpio2_port, uC_pins.gpio2_pin, gpioModePushPull, 1);
		ADP1650_GPIO2_Clr();
	}
	else
	{
		;
	}

	if (uC_pins.strobe_state == ADP160_UC_PIN_INPUT)
	{
		GPIO_PinModeSet(uC_pins.strobe_port, uC_pins.strobe_pin, gpioModeInput, 1);
	}
	else if (uC_pins.strobe_state == ADP160_UC_PIN_OUTPUT)
	{
		GPIO_PinModeSet(uC_pins.strobe_port, uC_pins.strobe_pin, gpioModePushPull, 1);
		ADP1650_Strobe_Clr();
	}
	else
	{
		;
	}

	ADP1650_uC_pins = uC_pins;
	//TODO add strobe pin
}

/*------- Torch and flash light mode on/off -------- */

void ADP1650_TorchLedOn(void)
{
	if (ADP1650_uC_pins.gpio1_state == ADP160_UC_PIN_OUTPUT)
	{
		ADP1650_SetLedOutputMode(ADP1650_LED_MODE_STANDBY);
		ADP1650_SetOutputEnable(ADP1650_OUTPUT_ON);
		ADP1650_GPIO1_Set();
	}
	else
	{
#ifdef DEBUG
		uart_sendText("GPIO1 is not set as output\n");
#else
		;
#endif
	}
}
void ADP1650_TorchLedOff(void)
{
	if (ADP1650_uC_pins.gpio1_state == ADP160_UC_PIN_OUTPUT)
	{
		ADP1650_GPIO1_Clr();
		ADP1650_SetOutputEnable(ADP1650_OUTPUT_OFF);
	}
	else
	{
#ifdef DEBUG
		uart_sendText("GPIO1 is not set as output\n");
#else
		;
#endif
	}
}

void ADP1650_FlashLedOn(void)
{
	ADP1650_SetLedOutputMode(ADP1650_LED_MODE_FLASH);
}
