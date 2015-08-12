#ifndef ADP8866_REG_H_
#define ADP8866_REG_H_

#include <stdint.h>

/* Comments included in this code comes from the ic datasheet:
 * http://www.analog.com/media/en/technical-documentation/data-sheets/ADP8866.pdf
 */

typedef struct{
	/*
	 * Interrupt configuration.
		1 = processor interrupt deasserts for 50 us and reasserts with pending events.
		0 = processor interrupt remains asserted if the host tries to clear the interrupt while there is a pending event.
	 */
	uint8_t INT_CFG;
	/*
	  	1 = device is in normal mode.
		0 = device is in standby, only I2C is enabled.
	 */
	uint8_t NSTBY;
	/*
	 	1 = charge pump gain is automatically set to 1× every time that the BLMX (Register 0x13) is written to.
		0 = writing to BLMX (Register 13) has no unique effect on the charge pump gain.
	 */
	uint8_t ALT_GSEL;
	/*
	 	1 = the charge pump does not switch down in gain until all LEDs are off. The charge pump switches up in gain
			as needed. This feature is useful if the ADP8866 charge pump is used to drive an external load.
		0 = the charge pump automatically switches up and down in gain. This provides optimal efficiency but is not
			suitable for driving external loads (other than those connected to the ADP8866 diode drivers).
	 */
	uint8_t GDWN_DIS;
	/*
	 * Master enable for independent sinks.
		1 = enables all LED current sinks designated as independent sinks. This bit has no effect if any of the SCx_EN
			bits that are part of the independent sinks group in Register 0x1A and Register 0x1B are set.
		0 = disables all sinks designated as independent sinks. This bit has no effect if any of the SCx_EN bits that are
			part of the independent sinks group in Register 0x1A and Register 0x1B are set.
	 */
	uint8_t SIS_EN;
	/*
	 * Master enable for backlight sinks.
		1 = enables all LED current sinks designated as backlight.
		0 = disables all sinks designated as backlight
	 */
	uint8_t BL_EN;
} ADP8866_ModeControl_t;
void ADP8866_SetModeControl(ADP8866_ModeControl_t mode);

typedef struct{
	/*
	 * Independent sink off.
		1 = indicates that the controller has ramped all the independent sinks designated in Register 0x04 and
			Register 0x05 to off.
		0 = the controller has not ramped all designated independent sinks to off.
	 */
	uint8_t ISCOFF_INT;
	/*
	 * Backlight off.
		1 = indicates that the controller has faded the backlight sinks to off.
		0 = the controller has not completed fading the backlight sinks to off
	 */
	uint8_t BLOFF_INT;
	/*
	 * Short-circuit error.
		1 = a short-circuit or overload condition on VOUT or current sinks was detected.
		0 = no short-circuit or overload condition was detected.
	 */
	uint8_t SHORT_INT;
	/*
	 * Thermal shutdown.
		1 = device temperature is too high and has been shut down.
		0 = no overtemperature condition was detected.
	 */
	uint8_t TSD_INT;
	/*
	 * Overvoltage interrupt.
		1 = charge-pump output voltage has exceeded VOVP.
		0 = charge-pump output voltage has not exceeded VOVP.
	 */
	uint8_t OVP_INT;
}ADP8866_InterruptStatus_t;
void ADP8866_SetInterruptStatus(ADP8866_InterruptStatus_t status);

typedef struct{
	/*
	 * Automated ISC off indicator.
		1 = the automated independent sink off indicator is enabled.
		0 = the automated independent sink off indicator is disabled.
	 */
	uint8_t ISCOFF_IEN;
	/*
	 * Automated backlight off indicator.
		1 = the automated backlight off indicator is enabled.
		0 = the automated backlight off indicator is disabled.
		When this bit is set, an INT is generated anytime that a backlight fade-out is over. This occurs after an automated
		fade-out or after the completion of a backlight dimming profile. This is useful to synchronize the complete turn off
		for the backlights with other devices in the application.
	 */
	uint8_t BLOFF_IEN;
	/*
	 * Short-circuit interrupt enabled. When the SHORT_INT status bit is set after an error condition, an interrupt is raised
			to the host if the SHORT_IEN flag is enabled.
		1 = the short-circuit interrupt is enabled.
		0 = the short-circuit interrupt is disabled (SHORT_INT flag is still asserted).
	 */
	uint8_t SHORT_IEN;
	/*
	 * Thermal shutdown interrupt enabled. When the TSD_INT status bit is set after an error condition, an interrupt is
			raised to the host if the TSD_IEN flag is enabled.
		1 = the thermal shutdown interrupt is enabled.
		0 = the thermal shutdown interrupt is disabled (TSD_INT flag is still asserted).
	 */
	uint8_t TSD_IEN;
	/*
	 * Overvoltage interrupt enabled. When the OVP_INT status bit is set after an error condition, an interrupt is raised to
			the host if the OVP_IEN flag is enabled.
		1 = the overvoltage interrupt is enabled.
		0 = the overvoltage interrupt is disabled (OVP_INT flag is still asserted)
	 */
	uint8_t OVP_IEN;
}ADP8866_InterruptEnable_t;
void ADP8866_SetInterruptEnable(ADP8866_InterruptEnable_t interruptEnable);

/* To include diode [X] in the ISCOFF_INT flag set bit [X] as 1, to exclude it set the bit [X] to 0 */
void ADP8866_SetIndependSinkInterruptSelection(uint16_t diodesList);

typedef enum{
	ADP8866_GAIN_MODE_AUTO	=	0,
	ADP8866_GAIN_MODE_1X	=	1,
	ADP8866_GAIN_MODE_1_5X	=	2,
	ADP8866_GAIN_MODE_2X	=	3
}ADP8866_GainMode_t;
void ADP8866_SetChargePumpGain(ADP8866_GainMode_t gainMode, ADP8866_GainModeLimit_t gainModeLimit);

typedef enum{
	ADP_MAX_CURR_RANGE_31_3_mA	= 0,
	ADP_MAX_CURR_RANGE_27_8_mA	= 1,
	ADP_MAX_CURR_RANGE_25_0_mA	= 2,
	ADP_MAX_CURR_RANGE_22_7_mA	= 3,
	ADP_MAX_CURR_RANGE_20_8_mA	= 4,
	ADP_MAX_CURR_RANGE_19_2_mA	= 5,
	ADP_MAX_CURR_RANGE_17_9_mA	= 6,
	ADP_MAX_CURR_RANGE_16_7_mA	= 7,
	ADP_MAX_CURR_RANGE_15_6_mA	= 8,
	ADP_MAX_CURR_RANGE_14_7_mA	= 9,
	ADP_MAX_CURR_RANGE_13_9_mA	= 10,
	ADP_MAX_CURR_RANGE_13_2_mA	= 11,
	ADP_MAX_CURR_RANGE_12_5_mA	= 12,
	ADP_MAX_CURR_RANGE_11_9_mA	= 13,
	ADP_MAX_CURR_RANGE_11_4_mA	= 14,
	ADP_MAX_CURR_RANGE_10_9_mA	= 15,
	ADP_MAX_CURR_RANGE_10_4_mA	= 16,
	ADP_MAX_CURR_RANGE_10_0_mA	= 17,
	ADP_MAX_CURR_RANGE_9_62_mA	= 18,
	ADP_MAX_CURR_RANGE_9_26_mA	= 19,
	ADP_MAX_CURR_RANGE_8_93_mA	= 20,
	ADP_MAX_CURR_RANGE_8_62_mA	= 21,
	ADP_MAX_CURR_RANGE_8_33_mA	= 22,
	ADP_MAX_CURR_RANGE_8_06_mA	= 23,
	ADP_MAX_CURR_RANGE_7_81_mA	= 24,
	ADP_MAX_CURR_RANGE_7_58_mA	= 25,
	ADP_MAX_CURR_RANGE_7_35_mA	= 26,
	ADP_MAX_CURR_RANGE_7_14_mA	= 27,
	ADP_MAX_CURR_RANGE_6_94_mA	= 28,
	ADP_MAX_CURR_RANGE_6_76_mA	= 29,
	ADP_MAX_CURR_RANGE_6_58_mA	= 30,
	ADP_MAX_CURR_RANGE_6_41_mA	= 31,
	ADP_MAX_CURR_RANGE_6_25_mA	= 32,
	ADP_MAX_CURR_RANGE_6_10_mA	= 33,
	ADP_MAX_CURR_RANGE_5_95_mA	= 34,
	ADP_MAX_CURR_RANGE_5_81_mA	= 35,
	ADP_MAX_CURR_RANGE_5_68_mA	= 36,
	ADP_MAX_CURR_RANGE_5_56_mA	= 37,
	ADP_MAX_CURR_RANGE_5_43_mA	= 38,
	ADP_MAX_CURR_RANGE_5_32_mA	= 39,
	ADP_MAX_CURR_RANGE_5_21_mA	= 40,
	ADP_MAX_CURR_RANGE_5_10_mA	= 41,
	ADP_MAX_CURR_RANGE_5_00_mA	= 42,
	ADP_MAX_CURR_RANGE_4_90_mA	= 43,
	ADP_MAX_CURR_RANGE_4_81_mA	= 44,
	ADP_MAX_CURR_RANGE_4_72_mA	= 45,
	ADP_MAX_CURR_RANGE_4_63_mA	= 46,
	ADP_MAX_CURR_RANGE_4_55_mA	= 47,
	ADP_MAX_CURR_RANGE_4_46_mA	= 48,
	ADP_MAX_CURR_RANGE_4_39_mA	= 49,
	ADP_MAX_CURR_RANGE_4_31_mA	= 50,
	ADP_MAX_CURR_RANGE_4_24_mA	= 51,
	ADP_MAX_CURR_RANGE_4_17_mA	= 52,
	ADP_MAX_CURR_RANGE_4_10_mA	= 53,
	ADP_MAX_CURR_RANGE_4_03_mA	= 54,
	ADP_MAX_CURR_RANGE_3_97_mA	= 55,
	ADP_MAX_CURR_RANGE_3_91_mA	= 56,
	ADP_MAX_CURR_RANGE_3_85_mA	= 57,
	ADP_MAX_CURR_RANGE_3_79_mA	= 58,
	ADP_MAX_CURR_RANGE_3_73_mA	= 59,
	ADP_MAX_CURR_RANGE_3_68_mA	= 60,
	ADP_MAX_CURR_RANGE_3_62_mA	= 61,
	ADP_MAX_CURR_RANGE_3_57_mA	= 62,
	ADP_MAX_CURR_RANGE_PWM		= 63
} ADP8866_MaxCurrentRange_t;
/* To control diode [X] with LEVEL_SET bits set bit [X] as 1,
 * to set diode [X] to normal mode (25mA full-scale current) set the bit [X] to 0 */
void ADP8866_SetOutputLevel(ADP8866_DiodeLevelMode_t diodeMode, ADP8866_MaxCurrentRange_t maxCurrentRange);

/* To power LED diode [X] from battery set bit [X] as 1,
 * to power LED diode [X] from the charge pump set the bit [X] to 0 */
void ADP8866_SetLedPowerSource(uint16_t diodesList);

typedef enum{
	ADP8866_BACKLIGHT_FADE				= 0,
	ADP8866_BACKLIGHT_INSTANT_TRANSITION = 1
} ADP8866_Backlight_respond_to_change_t;
typedef enum{
	ADP8866_TRANSF_SQUARE_LAW_LINEAR_TIME_STEPS = 0,
	ADP8866_TRANSF_SQUARE_LAW_NONLINEAR_TIME_STEPS_CUBIC10 = 2,
	ADP8866_RANSF_SQUARE_LAW_NONLINEAR_TIME_STEPS_CUBIC11 = 3,
} ADP8866_Transfer_Law_Function_t;

/* To select LED diode [X] as part of the independent sink group set bit [X] as 1,
 * to select LED diode [X] as part of the backlight group set the bit [X] to 0 */
void ADP8866_SetBacklightConfiguration(uint16_t diodesList, ADP8866_Backlight_respond_to_change_t backlightResponse, ADP8866_Transfer_Law_Function_t backlightTransferLaw);

typedef enum{
	ADP8866_BACKLIGHT_FADE_DISABLE	= 0,
	ADP8866_BACKLIGHT_FADE_0_05s	= 1,
	ADP8866_BACKLIGHT_FADE_0_10s	= 2,
	ADP8866_BACKLIGHT_FADE_0_15s	= 3,
	ADP8866_BACKLIGHT_FADE_0_20s	= 4,
	ADP8866_BACKLIGHT_FADE_0_25s	= 5,
	ADP8866_BACKLIGHT_FADE_0_30s	= 6,
	ADP8866_BACKLIGHT_FADE_0_35s	= 7,
	ADP8866_BACKLIGHT_FADE_0_40s	= 8,
	ADP8866_BACKLIGHT_FADE_0_45s	= 9,
	ADP8866_BACKLIGHT_FADE_0_50s	= 10,
	ADP8866_BACKLIGHT_FADE_0_75s	= 11,
	ADP8866_BACKLIGHT_FADE_1_00s	= 12,
	ADP8866_BACKLIGHT_FADE_1_25s	= 13,
	ADP8866_BACKLIGHT_FADE_1_50s	= 14,
	ADP8866_BACKLIGHT_FADE_1_75s	= 15,
}ADP8866_Backlight_Fade_Rate_t;
void ADP8866_SetBacklightFade(ADP8866_Backlight_Fade_Rate_t fadeOut, ADP8866_Backlight_Fade_Rate_t fadeIn);

/* Because the maximum current is calculated using a square law function and is a function of Backlight Output Level
 * arguments for this c function are not enumerated the user needs to read the correct register settign from the
 * datasheet and set the uint8_t BacklightSetting to the corresponding DAC code value*/
void ADP8866_SetBacklightMaxCurrent(uint8_t BacklightSetting);

/* To enable act on LED diode [X] set bit [X] as 1,
 * to disable act on LED diode [X] set the bit [X] to 0 */
void ADP8866_SetIndependentSinkCurrentControl(uint16_t diodesList, ADP8866_Transfer_Law_Function_t transferLawFunction);

typedef enum{
	ADP8866_SC_TIME_0_00s	=	0,
	ADP8866_SC_TIME_0_05s	=	1,
	ADP8866_SC_TIME_0_10s	=	2,
	ADP8866_SC_TIME_0_15s	=	3,
	ADP8866_SC_TIME_0_20s	=	4,
	ADP8866_SC_TIME_0_25s	=	5,
	ADP8866_SC_TIME_0_30s	=	6,
	ADP8866_SC_TIME_0_35s	=	7,
	ADP8866_SC_TIME_0_40s	=	8,
	ADP8866_SC_TIME_0_45s	=	9,
	ADP8866_SC_TIME_0_50s	=	10,
	ADP8866_SC_TIME_0_55s	=	11,
	ADP8866_SC_TIME_0_60s	=	12,
	ADP8866_SC_TIME_0_65s	=	13,
	ADP8866_SC_TIME_0_70s	=	14,
	ADP8866_SC_TIME_0_75s	=	15
} ADP8866_SC_Time_t;
typedef enum{
	ADP8866_SC_OFF_TIME_DISABLE	= 0,
	ADP8866_SC_OFF_TIME_0_6s	= 1,
	ADP8866_SC_OFF_TIME_1_2s	= 2,
	ADP8866_SC_OFF_TIME_1_8s	= 3
} ADP8866_ScOffTime_t;
void ADP8866_SetIndependentSinkCurrentTime(ADP8866_SC_Time_t scTime, ADP8866_ScOffTime_t Sc5Off, ADP8866_ScOffTime_t Sc4Off,
										 ADP8866_ScOffTime_t Sc3Off, ADP8866_ScOffTime_t Sc2Off, ADP8866_ScOffTime_t Sc1Off);

#endif /* ADP8866_REG_H_ */
