#include "ADP8866_reg.h"

#include "..\..\ucPeripheralDrivers\i2c_connection.h"
#include "..\..\ucPeripheralDrivers\leuart_connection.h"

#ifndef TRUE
	#define TRUE	(1)
#endif
#ifndef FALSE
	#define FALSE	(0)
#endif
#if TRUE == FALSE
	#error("TRUE macro == FALSE macro");
#endif

#define ADP8866_ADDR	(0x27*2)

uint8_t ADP8866_Detect_LowLevel(void)
{
	return i2c_Detect(I2C0, ADP8866_ADDR);
}

typedef enum {
	ADP8866_MANUFACTURER_ID_REG  						= 0x00,
	ADP8866_MODE_CONTROL_REG							= 0x01,
	ADP8866_INTERRUPT_STATUS_REG						= 0x02,
	ADP8866_INTERRUPT_ENABLE_REG						= 0x03,
	ADP8866_INDEPENDENT_SINK_INT_SEL_1_REG				= 0x04,
	ADP8866_INDEPENDENT_SINK_INT_SEL_2_REG				= 0x05,
	ADP8866_CHARGE_PUMP_GAIN_REG						= 0x06,
	ADP8866_OUTPUT_LEVEL_SEL_1_REG						= 0x07,
	ADP8866_OUTPUT_LEVEL_SEL_2_REG						= 0x08,
	ADP8866_LED_POWER_SOURCE_SEL_1_REG					= 0x09,
	ADP8866_LED_POWER_SOURCE_SEL_2_REG					= 0x0A,

	ADP8866_BACKLIGHT_CONF_REG							= 0x10,
	ADP8866_BACKLIGHT_SEL_REG							= 0x11,
	ADP8866_BACKLIGHT_FADE_REG							= 0x12,
	ADP8866_BACKLIGHT_MAX_CURRENT_REG					= 0x13,

	ADP8866_INDEPENDENT_SINK_CURRENT_CTR_1_REG			= 0x1A,
	ADP8866_INDEPENDENT_SINK_CURRENT_CTR_2_REG			= 0x1B,
	ADP8866_INDEPENDENT_SINK_CURRENT_TIME_1_REG			= 0x1C,
	ADP8866_INDEPENDENT_SINK_CURRENT_TIME_2_REG			= 0x1D,
	ADP8866_INDEPENDENT_SINK_6_OFF_TIMER_REG			= 0x1E,
	ADP8866_INDEPENDENT_SINK_7_OFF_TIMER_REG			= 0x1F,
	ADP8866_INDEPENDENT_SINK_8_OFF_TIMER_REG			= 0x20,
	ADP8866_INDEPENDENT_SINK_9_OFF_TIMER_REG			= 0x21,
	ADP8866_INDEPENDENT_SINK_CURRENT_FADE_REG			= 0x22,

	ADP8866_SINK_CURRENT_LED_1_REG						= 0x23,
	ADP8866_SINK_CURRENT_LED_2_REG						= 0x24,
	ADP8866_SINK_CURRENT_LED_3_REG						= 0x25,
	ADP8866_SINK_CURRENT_LED_4_REG						= 0x26,
	ADP8866_SINK_CURRENT_LED_5_REG						= 0x27,
	ADP8866_SINK_CURRENT_LED_6_REG						= 0x28,
	ADP8866_SINK_CURRENT_LED_7_REG						= 0x29,
	ADP8866_SINK_CURRENT_LED_8_REG						= 0x2A,
	ADP8866_SINK_CURRENT_LED_9_REG						= 0x2B,

	ADP8866_HEARTBEAT_ENABLE_SEL_REG					= 0x2C,
	ADP8866_INDEPENDENT_SINK_6_HEAR_PULSES_REG			= 0x2D,
	ADP8866_INDEPENDENT_SINK_7_HEAR_PULSES_REG			= 0x2E,
	ADP8866_INDEPENDENT_SINK_8_HEAR_PULSES_REG			= 0x2F,
	ADP8866_INDEPENDENT_SINK_9_HEAR_PULSES_REG			= 0x30,

	ADP8866_INDEPENDENT_SINK_6_TIM_OFF_HEAR_PULSES_REG	= 0x31,
	ADP8866_INDEPENDENT_SINK_7_TIM_OFF_HEAR_PULSES_REG	= 0x32,
	ADP8866_INDEPENDENT_SINK_8_TIM_OFF_HEAR_PULSES_REG	= 0x33,
	ADP8866_INDEPENDENT_SINK_9_TIM_OFF_HEAR_PULSES_REG	= 0x34,

	ADP8866_HEARTBEAT_ON_TIME_REG						= 0x35,

	ADP8866_ENABLE_DELAY_TIME_SC6_REG					= 0x3C,
	ADP8866_ENABLE_DELAY_TIME_SC7_REG					= 0x3D,
	ADP8866_ENABLE_DELAY_TIME_SC8_REG					= 0x3E,
	ADP8866_ENABLE_DELAY_TIME_SC9_REG					= 0x3F
} ADP8866_Registers_t;

static void ADP8866_SetRegister(ADP8866_Registers_t ADP8866_reg, uint8_t val)
{
	i2c_RegisterSet(I2C0, ADP8866_ADDR, (uint8_t)ADP8866_reg, val);
}

static void ADP8866_GetRegister(ADP8866_Registers_t ADP8866_reg, uint8_t *val)
{
	i2c_RegisterGet(I2C0, ADP8866_ADDR, (uint8_t)ADP8866_reg, val);
}

#define ADP8866_INT_CFG_SHIFT	6
#define ADP8866_NSTBY_SHIFT		5
#define ADP8866_ALT_GSEL_SHIFT	4
#define ADP8866_GDWN_DIS_SHIFT	3
#define ADP8866_SIS_EN_SHIFT	2
#define ADP8866_BL_EN_SHIFT		0
#define ADP8866_INT_CFG_MASK	0x01
#define ADP8866_NSTBY_MASK		0x01
#define ADP8866_ALT_GSEL_MASK	0x01
#define ADP8866_GDWN_DIS_MASK	0x01
#define ADP8866_SIS_EN_MASK		0x01
#define ADP8866_BL_EN_MASK		0x01
void ADP8866_SetModeControl(ADP8866_ModeControl_t mode)
{
	uint8_t val = ((mode.INT_CFG  & ADP8866_INT_CFG_MASK)  << ADP8866_INT_CFG_SHIFT)
			    + ((mode.NSTBY    & ADP8866_NSTBY_MASK)    << ADP8866_NSTBY_SHIFT)
			    + ((mode.ALT_GSEL & ADP8866_ALT_GSEL_MASK) << ADP8866_ALT_GSEL_SHIFT)
			    + ((mode.GDWN_DIS & ADP8866_GDWN_DIS_MASK) << ADP8866_GDWN_DIS_SHIFT)
			    + ((mode.SIS_EN   & ADP8866_SIS_EN_MASK)   << ADP8866_SIS_EN_SHIFT)
			    + ((mode.BL_EN    & ADP8866_BL_EN_MASK)    << ADP8866_BL_EN_SHIFT);
	ADP8866_SetRegister(ADP8866_MODE_CONTROL_REG, val);
}

#define ADP8866_ISCOFF_INT_SHIFT	6
#define ADP8866_BLOFF_INT_SHIFT		5
#define ADP8866_SHORT_INT_SHIFT		4
#define ADP8866_TSD_INT_SHIFT		3
#define ADP8866_OVP_INT_SHIFT		2
#define ADP8866_BLOFF_INT_MASK		0x01
#define ADP8866_SHORT_INT_MASK		0x01
#define ADP8866_TSD_INT_MASK		0x01
#define ADP8866_ISCOFF_INT_MASK		0x01
#define ADP8866_OVP_INT_MASK		0x01
void ADP8866_SetInterruptStatus(ADP8866_InterruptStatus_t status)
{
	uint8_t val = ((status.BLOFF_INT  & ADP8866_BLOFF_INT_MASK)  << ADP8866_ISCOFF_INT_SHIFT)
			    + ((status.SHORT_INT  & ADP8866_SHORT_INT_MASK)  << ADP8866_BLOFF_INT_SHIFT)
			    + ((status.TSD_INT    & ADP8866_TSD_INT_MASK)    << ADP8866_SHORT_INT_SHIFT)
			    + ((status.ISCOFF_INT & ADP8866_ISCOFF_INT_MASK) << ADP8866_TSD_INT_SHIFT)
			    + ((status.OVP_INT    & ADP8866_OVP_INT_MASK)    << ADP8866_OVP_INT_SHIFT);
	ADP8866_SetRegister(ADP8866_INTERRUPT_STATUS_REG, val);
}


#define ADP8866_ISCOFF_IEN_SHIFT	6
#define ADP8866_BLOFF_IEN_SHIFT		5
#define ADP8866_SHORT_IEN_SHIFT		4
#define ADP8866_TSD_IEN_SHIFT		3
#define ADP8866_OVP_IEN_SHIFT		2
#define ADP8866_BLOFF_IEN_MASK		0x01
#define ADP8866_SHORT_IEN_MASK		0x01
#define ADP8866_TSD_IEN_MASK		0x01
#define ADP8866_ISCOFF_IEN_MASK		0x01
#define ADP8866_OVP_IEN_MASK		0x01
void ADP8866_SetInterruptEnable(ADP8866_InterruptEnable_t interruptEnable)
{
	uint8_t val = ((interruptEnable.BLOFF_IEN  & ADP8866_BLOFF_IEN_MASK)  << ADP8866_ISCOFF_IEN_SHIFT)
			    + ((interruptEnable.SHORT_IEN  & ADP8866_SHORT_IEN_MASK)  << ADP8866_BLOFF_IEN_SHIFT)
			    + ((interruptEnable.TSD_IEN    & ADP8866_TSD_IEN_MASK)    << ADP8866_SHORT_IEN_SHIFT)
			    + ((interruptEnable.ISCOFF_IEN & ADP8866_ISCOFF_IEN_MASK) << ADP8866_TSD_IEN_SHIFT)
			    + ((interruptEnable.OVP_IEN    & ADP8866_OVP_IEN_MASK)    << ADP8866_OVP_IEN_SHIFT);
	ADP8866_SetRegister(ADP8866_INTERRUPT_ENABLE_REG, val);
}

void ADP8866_SetIndependSinkInterruptSelection(uint16_t diodesList)
{
	uint8_t IndepSinkInt_reg1 = 0x00;
	/* If the ninth bit is set then include Diode 9 */
	if ((diodesList & 0x200) == 1)
	{
		IndepSinkInt_reg1 |= 0x01;
	}
	ADP8866_SetRegister(ADP8866_INDEPENDENT_SINK_INT_SEL_1_REG, IndepSinkInt_reg1);

	uint8_t IndepSinkInt_reg2 = 0x00;
	for (uint16_t i = 1; i < 9; i++)
	{
		if ((diodesList & (1U << i)) == 1)
		{
			IndepSinkInt_reg2 |= 1U << (i - 1);
		}
	}
	ADP8866_SetRegister(ADP8866_INDEPENDENT_SINK_INT_SEL_2_REG, IndepSinkInt_reg2);
}


#define ADP8866_GAIN_MODE_SHIFT 		0
#define ADP8866_GAIN_MODE_LIMIT_SHIFT 	2
#define ADP8866_GAIN_MODE_MASK			0x03
#define ADP8866_GAIN_MODE_LIMIT_MASK	0x01
void ADP8866_SetChargePumpGain(ADP8866_GainMode_t gainMode, ADP8866_GainModeLimit_t gainModeLimit)
{
	uint8_t val = 0x00;
	val = (((uint8_t)gainMode 	   & ADP8866_GAIN_MODE_MASK) 	   << ADP8866_GAIN_MODE_SHIFT)
		+ (((uint8_t)gainModeLimit & ADP8866_GAIN_MODE_LIMIT_MASK) << ADP8866_GAIN_MODE_LIMIT_SHIFT);
	ADP8866_SetRegister(ADP8866_CHARGE_PUMP_GAIN_REG, val);
}

#define ADP8866_D9LVL_SHIFT		6
#define ADP8866_LEVEL_SET_SHIFT	0
#define ADP8866_D9LVL_MASK		0x01
#define ADP8866_LEVEL_SET_MASK	0x3F
void ADP8866_SetOutputLevel(uint16_t diodesLevelSelect, ADP8866_MaxCurrentRange_t maxCurrentRange)
{
	uint8_t outputLevel_reg1 = 0x00;
	if ((diodesLevelSelect & 0x200) == 1)
	{
		outputLevel_reg1 |= (ADP8866_D9LVL_MASK << ADP8866_D9LVL_SHIFT);
	}
	outputLevel_reg1 |= (((uint8_t)maxCurrentRange & ADP8866_LEVEL_SET_MASK) << ADP8866_LEVEL_SET_SHIFT);
	ADP8866_SetRegister(ADP8866_OUTPUT_LEVEL_SEL_1_REG, val);

	uint8_t outputLevel_reg2 = 0x00;
	for (uint16_t i = 1; i < 9; i++)
	{
		if ((diodesLevelSelect & (1U << i)) == 1)
		{
			outputLevel_reg2 |= 1U << (i - 1);
		}
	}
	ADP8866_SetRegister(ADP8866_OUTPUT_LEVEL_SEL_2_REG, outputLevel_reg2);
}

void ADP8866_SetLedPowerSource(uint16_t diodesList)
{
	uint8_t LedPowerSource_reg1 = 0x00;
	/* If the ninth bit is set then include Diode 9 */
	if ((diodesList & 0x200) == 1)
	{
		LedPowerSource_reg1 |= 0x01;
	}
	ADP8866_SetRegister(ADP8866_LED_POWER_SOURCE_SEL_1_REG, LedPowerSource_reg1);

	uint8_t LedPowerSource_reg2 = 0x00;
	for (uint16_t i = 1; i < 9; i++)
	{
		if ((diodesList & (1U << i)) == 1)
		{
			LedPowerSource_reg2 |= 1U << (i - 1);
		}
	}
	ADP8866_SetRegister(ADP8866_LED_POWER_SOURCE_SEL_1_REG, LedPowerSource_reg2);
}

#define ADP8866_D9SEL_SHIFT		4
#define ADP8866_CABCFADE_SHIFT	3
#define ADP8866_BL_LAW_SHIFT	1
#define ADP8866_D9SEL_MASK		0x01
#define ADP8866_CABCFADE_MASK	0x01
#define ADP8866_BL_LAW_MASK		0x03
void ADP8866_SetBacklightConfiguration(uint16_t diodesList, ADP8866_Backlight_respond_to_change_t backlightResponse, ADP8866_Transfer_Law_Function_t backlightTransferLaw)
{
	uint8_t BacklightConfiguration_reg1 = 0x00;
	/* If the ninth bit is set then include Diode 9 */
	if ((diodesList & 0x200) == 1)
	{
		BacklightConfiguration_reg1 |= (ADP8866_D9SEL_MASK << ADP8866_D9SEL_SHIFT);
	}
	BacklightConfiguration_reg1 |= (((uint8_t)backlightResponse    & ADP8866_CABCFADE_MASK) << ADP8866_CABCFADE_SHIFT)
								 + (((uint8_t)backlightTransferLaw & ADP8866_BL_LAW_MASK)   << ADP8866_BL_LAW_SHIFT);
	ADP8866_SetRegister(ADP8866_BACKLIGHT_CONF_REG, BacklightConfiguration_reg1);

	uint8_t BacklightConfiguration_reg2 = 0x00;
	for (uint16_t i = 1; i < 9; i++)
	{
		if ((diodesList & (1U << i)) == 1)
		{
			BacklightConfiguration_reg2 |= 1U << (i - 1);
		}
	}
	ADP8866_SetRegister(ADP8866_BACKLIGHT_SEL_REG, BacklightConfiguration_reg2);
}

#define ADP8866_BL_FO_SHIFT		4
#define ADP8866_BL_FI_SHIFT		0
#define ADP8866_BL_FO_MAX		0x0F
#define ADP8866_BL_FI_MASK		0x0F
void ADP8866_SetBacklightFade(ADP8866_Backlight_Fade_Rate_t fadeOut, ADP8866_Backlight_Fade_Rate_t fadeIn)
{
	uint8_t val = 0x00;
	val = (((uint8_t)fadeOut & ADP8866_BL_FO_MAX) << ADP8866_BL_FO_SHIFT)
		+ (((uint8_t)fadeIn  & ADP8866_BL_FI_MASK) << ADP8866_BL_FI_SHIFT);

	ADP8866_SetRegister(ADP8866_BACKLIGHT_FADE_REG, val);
}

#define ADP8866_BL_MC_SHIFT		0
#define ADP8866_BL_MC_MAX		0x7F
void ADP8866_SetBacklightMaxCurrent(uint8_t BacklightSetting)
{
	uint8_t val;
	val = (BacklightSetting & 0x7F) << ADP8866_BL_MC_SHIFT;
	ADP8866_SetRegister(ADP8866_BACKLIGHT_MAX_CURRENT_REG, val);
}

#define ADP8866_D9_EN_SHIFT		2
#define ADP8866_SC_LAW_SHIFT	0
#define ADP8866_D9_EN_MASK		0x01
#define ADP8866_SC_LAW_MASK		0x03
void ADP8866_SetIndependentSinkCurrentControl(uint16_t diodesList, ADP8866_Transfer_Law_Function_t transferLawFunction)
{
	uint8_t IndependentSinkCurrent_EnableAct_reg1 = 0x00;
	/* If the ninth bit is set then include Diode 9 */
	if ((diodesList & 0x200) == 1)
	{
		IndependentSinkCurrent_EnableAct_reg1 |= (ADP8866_D9_EN_MASK << ADP8866_D9_EN_SHIFT);
	}
	IndependentSinkCurrent_EnableAct_reg1 |= (((uint8_t)transferLawFunction & ADP8866_SC_LAW_MASK)   << ADP8866_SC_LAW_SHIFT);
	ADP8866_SetRegister(ADP8866_INDEPENDENT_SINK_CURRENT_CTR_1_REG, IndependentSinkCurrent_EnableAct_reg1);

	uint8_t IndependentSinkCurrent_EnableAct_reg2 = 0x00;
	for (uint16_t i = 1; i < 9; i++)
	{
		if ((diodesList & (1U << i)) == 1)
		{
			IndependentSinkCurrent_EnableAct_reg2 |= 1U << (i - 1);
		}
	}
	ADP8866_SetRegister(ADP8866_INDEPENDENT_SINK_CURRENT_CTR_2_REG, IndependentSinkCurrent_EnableAct_reg2);
}

#define ADP8866_SCON_SHIFT		4
#define ADP8866_SC5OFF_SHIFT	0
#define ADP8866_SC4OFF_SHIFT	6
#define ADP8866_SC3OFF_SHIFT	4
#define ADP8866_SC2OFF_SHIFT	2
#define ADP8866_SC1OFF_SHIFT	0

#define ADP8866_SCON_MASK		0x0F
#define ADP8866_SC5OFF_MASK		0x03
#define ADP8866_SC4OFF_MASK		0x03
#define ADP8866_SC3OFF_MASK		0x03
#define ADP8866_SC2OFF_MASK		0x03
#define ADP8866_SC1OFF_MASK		0x03
void ADP8866_SetIndependentSinkCurrentTime(ADP8866_SC_Time_t scTime, ADP8866_ScOffTime_t Sc5Off, ADP8866_ScOffTime_t Sc4Off,
										 ADP8866_ScOffTime_t Sc3Off, ADP8866_ScOffTime_t Sc2Off, ADP8866_ScOffTime_t Sc1Off)
{
	uint8_t IndepSinkCurrTime_reg1 = 0x00;
	IndepSinkCurrTime_reg1 = (((uint8_t)scTime & ADP8866_SCON_MASK)   << ADP8866_SCON_SHIFT)
						   + (((uint8_t)Sc5Off & ADP8866_SC5OFF_MASK) << ADP8866_SC5OFF_SHIFT);
	ADP8866_SetRegister(ADP8866_INDEPENDENT_SINK_CURRENT_TIME_1_REG, IndepSinkCurrTime_reg1);

	uint8_t IndepSinkCurrTime_reg2 = 0x00;
	IndepSinkCurrTime_reg2 = (((uint8_t)Sc4Off & ADP8866_SC4OFF_MASK) << ADP8866_SC4OFF_SHIFT)
						   + (((uint8_t)Sc3Off & ADP8866_SC3OFF_MASK) << ADP8866_SC3OFF_SHIFT)
						   + (((uint8_t)Sc2Off & ADP8866_SC2OFF_MASK) << ADP8866_SC2OFF_SHIFT)
						   + (((uint8_t)Sc1Off & ADP8866_SC1OFF_MASK) << ADP8866_SC1OFF_SHIFT);
	ADP8866_SetRegister(ADP8866_INDEPENDENT_SINK_CURRENT_TIME_2_REG, IndepSinkCurrTime_reg2);
}
