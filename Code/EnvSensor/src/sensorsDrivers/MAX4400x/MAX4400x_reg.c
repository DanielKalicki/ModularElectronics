#include "MAX4400x_reg.h"
#include "..\..\ucPeripheralDrivers\i2c_connection.h"

#ifndef TRUE
	#define TRUE	(1)
#endif
#ifndef FALSE
	#define FALSE	(0)
#endif
#if TRUE == FALSE
	#error("TRUE macro == FALSE macro");
#endif

#if defined(MAX44008)
	#if MAX4400X_A0 == 1
		const uint8_t MAX4400X_ADDR=0x40*2;
	#elif MAX4400X_A0 == 0
		const uint8_t MAX4400X_ADDR=0x41*2;
	#else
		#error "MAX4400X_A0 needs to be set to 0 or 1"
	#endif
#elif defined(MAX44006)
	#if MAX4400X_A0 == 1
		const uint8_t MAX4400X_ADDR=0x44*2;
	#elif MAX4400X_A0 == 0
		const uint8_t MAX4400X_ADDR=0x45*2;
	#else
		#error "MAX4400X_A0 needs to be set to 0 or 1"
	#endif
#elif defined(MAX44005)
	const uint8_t MAX4400X_ADDR=0x48*2;
#endif

uint8_t MAX4400x_Detect_LowLevel(void)
{
	return i2c_Detect(I2C0, MAX4400X_ADDR);
}

typedef enum {
	MAX4400X_INTERRUPT_STATUS_REG  		= 0x00,

	MAX4400X_MAIN_CONF_REG				= 0x01,
	MAX4400X_AMBIENT_CONF_REG			= 0x02,
	MAX4400X_PROXIMITY_CONF_REG			= 0x03,

	MAX4400X_AMBIENT_CLEAR_HIGH_REG		= 0x04,
	MAX4400X_AMBIENT_CLEAR_LOW_REG		= 0x05,

	MAX4400X_AMBIENT_RED_HIGH_REG		= 0x06,
	MAX4400X_AMBIENT_RED_LOW_REG		= 0x07,

	MAX4400X_AMBIENT_GREEN_HIGH_REG		= 0x08,
	MAX4400X_AMBIENT_GREEN_LOW_REG		= 0x09,

	MAX4400X_AMBIENT_BLUE_HIGH_REG		= 0x0A,
	MAX4400X_AMBIENT_BLUE_LOW_REG		= 0x0B,

	MAX4400X_AMBIENT_IR_HIGH_REG		= 0x0C,
	MAX4400X_AMBIENT_IR_LOW_REG			= 0x0D,

	MAX4400X_AMBIENT_IR_COMP_HIGH_REG	= 0x0E,
	MAX4400X_AMBIENT_IR_COMP_LOW_REG	= 0x0F,

	MAX4400X_PROXIMITY_IR_HIGH_REG		= 0x10,
	MAX4400X_PROXIMITY_IR_LOW_REG		= 0x11,

	MAX4400X_TEMP_HIGH_REG				= 0x12,
	MAX4400X_TEMP_LOW_REG				= 0x13,

	MAX4400X_AMB_UP_THR_HIGH_REG		= 0x14,
	MAX4400X_AMB_UP_THR_LOW_REG			= 0x15,

	MAX4400X_AMB_LOW_THR_HIGH_REG		= 0x16,
	MAX4400X_AMB_LOW_THR_LOW_REG		= 0x17,

	MAX4400X_THR_PERSIST_TIMER_REG		= 0x18,

	MAX4400X_PROX_UP_THR_HIGH_REG		= 0x19,
	MAX4400X_PROX_UP_THR_LOW_REG		= 0x1A,

	MAX4400X_PROX_LOW_THR_HIGH_REG		= 0x1B,
	MAX4400X_PROX_LOW_THR_LOW_REG		= 0x1C,

	MAX4400X_DIG_GAIN_TRIM_CLEAR_REG	= 0x1D,
	MAX4400X_DIG_GAIN_TRIM_RED_REG		= 0x1E,
	MAX4400X_DIG_GAIN_TRIM_GREEN_REG	= 0x1F,
	MAX4400X_DIG_GAIN_TRIM_BLUE_REG		= 0x20,
	MAX4400X_DIG_GAIN_TRIM_IR_REG		= 0x21
	/* TODO ANA_IN_KEY register is not implemented */
} MAX4400x_Registers_t;

static void MAX4400x_SetRegister(MAX4400x_Registers_t MAX4400x_reg, uint8_t val)
{
	i2c_RegisterSet(I2C0, MAX4400X_ADDR, (uint8_t)MAX4400x_reg, val);
}

static void MAX4400x_GetRegister(MAX4400x_Registers_t MAX4400x_reg, uint8_t *val)
{
	i2c_RegisterGet(I2C0, MAX4400X_ADDR, (uint8_t)MAX4400x_reg, val);
}

#define MAX4400X_INT_STAT_RESET_SHIFT			4
#define MAX4400X_INT_STAT_SHDN_SHIFT			3
#define MAX4400X_INT_STAT_PWRON_SHIFT			2
#define MAX4400X_INT_STAT_PRXINTS_SHIFT			1
#define MAX4400X_INT_STAT_AMBINTS_SHIFT			0
MAX4400x_Interrupt_Status_t MAX4400x_GetInterruptStatus(void)
{
	MAX4400x_Interrupt_Status_t InterruptStatus;
	uint8_t regValue = 0x00;
	MAX4400x_GetRegister(MAX4400X_INTERRUPT_STATUS_REG, &regValue);

	if (regValue & MAX4400X_INT_STAT_RESET_SHIFT)
	{
		InterruptStatus.Reset = TRUE;
	}
	else
	{
		InterruptStatus.Reset = FALSE;
	}

	if (regValue & MAX4400X_INT_STAT_SHDN_SHIFT)
	{
		InterruptStatus.Shutdown = TRUE;
	}
	else
	{
		InterruptStatus.Shutdown = FALSE;
	}

	if (regValue & MAX4400X_INT_STAT_PWRON_SHIFT)
	{
		InterruptStatus.PowerOn = TRUE;
	}
	else
	{
		InterruptStatus.PowerOn = FALSE;
	}

	if (regValue & MAX4400X_INT_STAT_PRXINTS_SHIFT)
	{
		InterruptStatus.ProximityReceive = TRUE;
	}
	else
	{
		InterruptStatus.ProximityReceive = FALSE;
	}

	if (regValue & MAX4400X_INT_STAT_AMBINTS_SHIFT)
	{
		InterruptStatus.AmbientInterrupt = TRUE;
	}
	else
	{
		InterruptStatus.AmbientInterrupt = FALSE;
	}

	return InterruptStatus;
}

#define MAX4400X_MAIN_CONF_AMBINTE_SHIFT	0
#define MAX4400X_MAIN_CONF_PRXINTE_SHIFT	1
#define MAX4400X_MAIN_CONF_AMBSEL_SHIFT		2
#define MAX4400X_MAIN_CONF_MODE_SHIFT		4
void MAX4400x_SetMainConfiguration(MAX4400x_Main_Configuration_t MainConf)
{
	uint8_t val;
	val = ((MainConf.Ambient_Interrupt_Enable   & 0x01) << MAX4400X_MAIN_CONF_AMBINTE_SHIFT)
		+ ((MainConf.Proximity_Interrupt_Enable & 0x01) << MAX4400X_MAIN_CONF_PRXINTE_SHIFT)
		+ ((MainConf.Ambient_Interrupt_Select   & 0x03) << MAX4400X_MAIN_CONF_AMBSEL_SHIFT)
		+ ((MainConf.Mode				        & 0x07) << MAX4400X_MAIN_CONF_MODE_SHIFT);

	MAX4400x_SetRegister(MAX4400X_MAIN_CONF_REG, val);
}

#define MAX4400X_AMB_CONF_AMBPGA_SHIFT	0
#define MAX4400X_AMB_CONF_AMBTIM_SHIFT	2
#define MAX4400X_AMB_CONF_TEMPEN_SHIFT	5
#define MAX4400X_AMB_CONF_COMPEN_SHIFT	6
#define MAX4400X_AMB_CONF_TRIM_SHIFT	7
void MAX4400x_SetAmbientConfiguration(MAX4400x_Ambient_Configuration_t AmbientConfiguration)
{
	uint8_t val;
	val = ((AmbientConfiguration.AMBPGA_Gain_Setting 			 & 0x03) << MAX4400X_AMB_CONF_AMBPGA_SHIFT)
		+ ((AmbientConfiguration.AMBTIM_Integration_Time_Setting & 0x07) << MAX4400X_AMB_CONF_AMBTIM_SHIFT)
		+ ((AmbientConfiguration.Temperature_Sensor_Enabled		 & 0x01) << MAX4400X_AMB_CONF_TEMPEN_SHIFT)
		+ ((AmbientConfiguration.IR_Compensation_Enabled 		 & 0x03) << MAX4400X_AMB_CONF_COMPEN_SHIFT)
		+ ((AmbientConfiguration.Trim_Adjust_Enabled			 & 0x03) << MAX4400X_AMB_CONF_TRIM_SHIFT);
	MAX4400x_SetRegister(MAX4400X_AMBIENT_CONF_REG, val);
}

#define MAX4400X_PROX_CONF_PRXPGA_SHIFT	0
#define MAX4400X_PROX_CONF_PRXTIM_SHIFT	1
#define MAX4400X_PROX_CONF_DRV_SHIFT	4
void MAX4400x_SetProximityConfiguration(MAX4400x_Proximity_Configuration_t ProximityConfiguration)
{
	uint8_t val;
	val = ((ProximityConfiguration.PRXPGA_IR_Channel_Gain 	  & 0x01) << MAX4400X_PROX_CONF_PRXPGA_SHIFT)
		+ ((ProximityConfiguration.PRXTIM_IR_Channel_Int_Time & 0x01) << MAX4400X_PROX_CONF_PRXTIM_SHIFT)
		+ ((ProximityConfiguration.LED_Drive_Current_Setting  & 0x0F) << MAX4400X_PROX_CONF_DRV_SHIFT);
	MAX4400x_SetRegister(MAX4400X_PROXIMITY_CONF_REG, val);
}

uint16_t MAX4400x_GetAmbientClear(void)
{
	uint16_t ret;

	uint8_t regHigh;
	MAX4400x_GetRegister(MAX4400X_AMBIENT_CLEAR_HIGH_REG, &regHigh);
	uint8_t regLow;
	MAX4400x_GetRegister(MAX4400X_AMBIENT_CLEAR_LOW_REG, &regLow);

	ret = ((regHigh & 0x3F) << 8u) + regLow;

	return ret;
}

uint16_t MAX4400x_GetAmbientRed(void)
{
	uint16_t ret;

	uint8_t regHigh;
	MAX4400x_GetRegister(MAX4400X_AMBIENT_RED_HIGH_REG, &regHigh);
	uint8_t regLow;
	MAX4400x_GetRegister(MAX4400X_AMBIENT_RED_LOW_REG, &regLow);

	ret = ((regHigh & 0x3F) << 8u) + regLow;

	return ret;
}

uint16_t MAX4400x_GetAmbientGreen(void)
{
	uint16_t ret;

	uint8_t regHigh;
	MAX4400x_GetRegister(MAX4400X_AMBIENT_GREEN_HIGH_REG, &regHigh);
	uint8_t regLow;
	MAX4400x_GetRegister(MAX4400X_AMBIENT_GREEN_LOW_REG, &regLow);

	ret = ((regHigh & 0x3F) << 8u) + regLow;

	return ret;
}

uint16_t MAX4400x_GetAmbientBlue(void)
{
	uint16_t ret;

	uint8_t regHigh;
	MAX4400x_GetRegister(MAX4400X_AMBIENT_BLUE_HIGH_REG, &regHigh);
	uint8_t regLow;
	MAX4400x_GetRegister(MAX4400X_AMBIENT_BLUE_LOW_REG, &regLow);

	ret = ((regHigh & 0x3F) << 8u) + regLow;

	return ret;
}

uint16_t MAX4400x_GetAmbientIr(void)
{
	uint16_t ret;

	uint8_t regHigh;
	MAX4400x_GetRegister(MAX4400X_AMBIENT_IR_HIGH_REG, &regHigh);
	uint8_t regLow;
	MAX4400x_GetRegister(MAX4400X_AMBIENT_IR_LOW_REG, &regLow);

	ret = ((regHigh & 0x3F) << 8u) + regLow;

	return ret;
}

uint16_t MAX4400x_GetAmbientIrComp(void)
{
	uint16_t ret;

	uint8_t regHigh;
	MAX4400x_GetRegister(MAX4400X_AMBIENT_IR_COMP_HIGH_REG, &regHigh);
	uint8_t regLow;
	MAX4400x_GetRegister(MAX4400X_AMBIENT_IR_COMP_LOW_REG, &regLow);

	ret = ((regHigh & 0x3F) << 8u) + regLow;

	return ret;
}

uint16_t MAX4400x_GetProximityIr(void)
{
	uint16_t ret;

	uint8_t regHigh;
	MAX4400x_GetRegister(MAX4400X_PROXIMITY_IR_HIGH_REG, &regHigh);
	uint8_t regLow;
	MAX4400x_GetRegister(MAX4400X_PROXIMITY_IR_LOW_REG, &regLow);

	ret = ((regHigh & 0x03) << 8u) + regLow;

	return ret;
}

uint16_t MAX4400x_GetTemperature(void)
{
	uint16_t ret;

	uint8_t regHigh;
	MAX4400x_GetRegister(MAX4400X_TEMP_HIGH_REG, &regHigh);
	uint8_t regLow;
	MAX4400x_GetRegister(MAX4400X_TEMP_LOW_REG, &regLow);

	ret = ((regHigh & 0x03) << 8u) + regLow;

	return ret;
}

uint16_t MAX4400x_GetAmbientUpThreshold(void)
{
	uint16_t ret;

	uint8_t regHigh;
	MAX4400x_GetRegister(MAX4400X_AMB_UP_THR_HIGH_REG, &regHigh);
	uint8_t regLow;
	MAX4400x_GetRegister(MAX4400X_AMB_UP_THR_LOW_REG, &regLow);

	ret = ((regHigh & 0x03) << 8u) + regLow;

	return ret;
}

void MAX4400x_SetAmbientUpThreshold(uint16_t UpThreshold)
{
	MAX4400x_SetRegister(MAX4400X_AMB_UP_THR_HIGH_REG, (uint8_t)((UpThreshold >> 8) & 0x3F));
	MAX4400x_SetRegister(MAX4400X_AMB_UP_THR_LOW_REG, (uint8_t)UpThreshold);
}

uint16_t MAX4400x_GetAmbientLowThreshold(void)
{
	uint16_t ret;

	uint8_t regHigh;
	MAX4400x_GetRegister(MAX4400X_AMB_LOW_THR_HIGH_REG, &regHigh);
	uint8_t regLow;
	MAX4400x_GetRegister(MAX4400X_AMB_LOW_THR_LOW_REG, &regLow);

	ret = ((regHigh & 0x03) << 8u) + regLow;

	return ret;
}

void MAX4400x_SetAmbientLowThreshold(uint16_t LowThreshold)
{
	MAX4400x_SetRegister(MAX4400X_AMB_LOW_THR_HIGH_REG, (uint8_t)((LowThreshold >> 8) & 0x3F));
	MAX4400x_SetRegister(MAX4400X_AMB_LOW_THR_LOW_REG, (uint8_t)LowThreshold);
}

#define MAX4400X_THR_PERSISTANT_TIMER_AMBPST_SHIFT		0
#define MAX4400X_THR_PERSISTANT_TIMER_PRXPST_SHIFT		2
void MAX4400x_SetThresholdPersistTimer(MAX4400x_Threshold_Persist_Timer_t ThresholdPersistTimer)
{
	uint8_t val;
	val = ((ThresholdPersistTimer.Proximity_Persistant_Timer & 0x03) << MAX4400X_THR_PERSISTANT_TIMER_AMBPST_SHIFT)
		+ ((ThresholdPersistTimer.Ambient_Persistant_Timer	 & 0x03) << MAX4400X_THR_PERSISTANT_TIMER_PRXPST_SHIFT);
	MAX4400x_SetRegister(MAX4400X_THR_PERSIST_TIMER_REG, val);

}

uint16_t MAX4400x_GetProximityUpThreshold(void)
{
	uint16_t ret;

	uint8_t regHigh;
	MAX4400x_GetRegister(MAX4400X_PROX_UP_THR_HIGH_REG, &regHigh);
	uint8_t regLow;
	MAX4400x_GetRegister(MAX4400X_PROX_UP_THR_LOW_REG, &regLow);

	ret = ((regHigh & 0x03) << 8u) + regLow;

	return ret;
}

void MAX4400x_SetProximityUpThreshold(uint16_t UpThreshold)
{
	MAX4400x_SetRegister(MAX4400X_PROX_UP_THR_HIGH_REG, (uint8_t)((UpThreshold >> 8) & 0x03));
	MAX4400x_SetRegister(MAX4400X_PROX_UP_THR_LOW_REG, (uint8_t)UpThreshold);
}

uint16_t MAX4400x_GetProximityLowThreshold(void)
{
	uint16_t ret;

	uint8_t regHigh;
	MAX4400x_GetRegister(MAX4400X_PROX_LOW_THR_HIGH_REG, &regHigh);
	uint8_t regLow;
	MAX4400x_GetRegister(MAX4400X_PROX_LOW_THR_LOW_REG, &regLow);

	ret = ((regHigh & 0x03) << 8u) + regLow;

	return ret;
}

void MAX4400x_SetProximityLowThreshold(uint16_t LowThreshold)
{
	MAX4400x_SetRegister(MAX4400X_PROX_LOW_THR_HIGH_REG, (uint8_t)((LowThreshold >> 8) & 0x03));
	MAX4400x_SetRegister(MAX4400X_PROX_LOW_THR_LOW_REG, (uint8_t)LowThreshold);
}

uint8_t MAX4400x_GetTrimGainClear(void)
{
	uint8_t regVal;
	MAX4400x_GetRegister(MAX4400X_DIG_GAIN_TRIM_CLEAR_REG, &regVal);
	return  (regVal & 0x7F);
}
void MAX4400x_SetTrimGainClear(uint8_t TrimGain)
{
	MAX4400x_SetRegister(MAX4400X_DIG_GAIN_TRIM_CLEAR_REG, (TrimGain & 0x7F));
}

uint8_t MAX4400x_GetTrimGainRed(void)
{
	uint8_t regVal;
	MAX4400x_GetRegister(MAX4400X_DIG_GAIN_TRIM_RED_REG, &regVal);
	return  (regVal & 0x7F);
}
void MAX4400x_SetTrimGainRed(uint8_t TrimGain)
{
	MAX4400x_SetRegister(MAX4400X_DIG_GAIN_TRIM_RED_REG, (TrimGain & 0x7F));
}

uint8_t MAX4400x_GetTrimGainGreen(void)
{
	uint8_t regVal;
	MAX4400x_GetRegister(MAX4400X_DIG_GAIN_TRIM_GREEN_REG, &regVal);
	return  (regVal & 0x7F);
}
void MAX4400x_SetTrimGainGreen(uint8_t TrimGain)
{
	MAX4400x_SetRegister(MAX4400X_DIG_GAIN_TRIM_GREEN_REG, (TrimGain & 0x7F));
}

uint8_t MAX4400x_GetTrimGainBlue(void)
{
	uint8_t regVal;
	MAX4400x_GetRegister(MAX4400X_DIG_GAIN_TRIM_BLUE_REG, &regVal);
	return  (regVal & 0x7F);
}
void MAX4400x_SetTrimGainBlue(uint8_t TrimGain)
{
	MAX4400x_SetRegister(MAX4400X_DIG_GAIN_TRIM_BLUE_REG, (TrimGain & 0x7F));
}

uint8_t MAX4400x_GetTrimGainIr(void)
{
	uint8_t regVal;
	MAX4400x_GetRegister(MAX4400X_DIG_GAIN_TRIM_IR_REG, &regVal);
	return  (regVal & 0x7F);
}
void MAX4400x_SetTrimGainIr(uint8_t TrimGain)
{
	MAX4400x_SetRegister(MAX4400X_DIG_GAIN_TRIM_IR_REG, (TrimGain & 0x7F));
}
