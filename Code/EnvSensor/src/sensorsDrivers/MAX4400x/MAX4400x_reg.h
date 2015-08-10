#ifndef MAX4400X_REG_H_
#define MAX4400X_REG_H_

#include <stdint.h>

/*----------------------------------------------------*/
/*							!!!						  */
/* 			Define used sensor for i2c address 		  */
/*							!!!						  */
/*----------------------------------------------------*/
#define MAX44008
#define MAX4400X_A0		1

/* Comments included in this file comes from MAX44005 datasheet
 * http://datasheets.maximintegrated.com/en/ds/MAX44005.pdf
 */

uint8_t MAX4400x_Detect_LowLevel(void);

typedef struct{
	uint8_t Reset;
	uint8_t Shutdown;
	uint8_t PowerOn;
	uint8_t ProximityReceive;
	uint8_t AmbientInterrupt;
} MAX4400x_Interrupt_Status_t;
MAX4400x_Interrupt_Status_t MAX4400x_GetInterruptStatus(void);

typedef struct{
/*
BIT0 	OPERATION
0 		The AMBINTS bit and INT pin remain unasserted even if an ambient interrupt event has occurred. The AMBINTS bit
		is set to 0 if previously set to 1. See Table 1 for more details.
1 		Detection of ambient interrupt events is enabled. See Table 1 for more details. An ambient in
		hardware interrupt (INT pin pulled low) and set the AMBINTS bit (Register 0x00, BIT0).
*/
	uint8_t Ambient_Interrupt_Enable;
/*
BIT1 	OPERATION
0 		PRXINTS bit and INT pin remains unasserted even if a proximity interrupt event has occurred. The PRXINTS bit is set
		to 0 if previously set to 1. See Table 2 for more details.
1 		Detection of proximity interrupt events is enabled. See Table 2 for more details. A proximity interrupt can trigger a
		hardware interrupt (INT pin pulled low) and set the PRXINTS bit (Register 0x00, BIT1).
 */
	uint8_t Proximity_Interrupt_Enable;
/*
AMBSEL[1:0]		OPERATION
	00 			CLEAR channel data is used to compare with ambient interrupt thresholds and ambient timer settings.
	01 			GREEN channel data is used to compare with ambient interrupt thresholds and ambient timer settings.
	10 			IR channel data is used to compare with ambient interrupt thresholds and ambient timer settings.
	11 			TEMP channel data is used to compare with ambient interrupt thresholds and ambient timer settings.
 */
	uint8_t Ambient_Interrupt_Select;
/*MODE[2:0] OPERATING MODE 				COMMENTS
	000 	CLEAR 						CLEAR + TEMP* channel active only
	001 	CLEAR + IR 					CLEAR + TEMP* + IR channels active
	010 	CLEAR + RGB + IR 			CLEAR + TEMP* + RGB + IR channels active
	011 	CLEAR + IR + PROX 			CLEAR + TEMP* + IR + PROX channels active
										(CLEAR + TEMP* + IR + PROX interleaved)
	100 	CLEAR + RGB + IR + PROX 	CLEAR + TEMP* + RGB + IR + PROX channels active
										(CLEAR + TEMP* + RBG + IR and PROX interleaved)
	101 	PROX only 					PROX only continuous
	110 	Reserved			 		Reserved
	111 	Reserved 					Reserved
*/
	uint8_t Mode;
} MAX4400x_Main_Configuration_t;
void MAX4400x_SetMainConfiguration(MAX4400x_Main_Configuration_t MainConf);

typedef struct{
/* See the datasheet for full settings table */
	uint8_t AMBPGA_Gain_Setting;
/*
AMBTIM[2:0] INTEGRATION TIME		FULL-SCALE ADC	BIT RESOLUTION	RELATIVE LSB
				 (ms)				   (Counts)						SIZE FOR FIXED
																	AMBPGA[1:0]
000 		100 					16,384 			14				1x
001 		25 						4,096 			12 				4x
010 		6.25 					1,024 			10 				16x
011 		1.5625 					256 			8 				64x
100 		400 					16,384 			14 				1/4x
101 		Reserved 				Not applicable 	Not applicable 	Not applicable
110 		Reserved 				Not applicable 	Not applicable 	Not applicable
111 		Reserved 				Not applicable 	Not applicable 	Not applicable
 */
	uint8_t AMBTIM_Integration_Time_Setting;
/*
BIT 5 	OPERATION
0 		Disables IR compensation.
1 		Enables IR compensation. Only for MODE[2:0] = 000 mode
*/
	uint8_t IR_Compensation_Enabled;
/*
BIT 6 	OPERATION
0		Disables temperature sensor.
1 		Enables temperature sensor
*/
		uint8_t Temperature_Sensor_Enabled;
/*
BIT 7	OPERATION
0 		Use factory-programmed gains for all the channels. Ignore any bytes written to the TRIM_GAIN_GREEN[6:0], TRIM_
		GAIN_RED[6:0], TRIM_GAIN_BLUE[6:0], TRIM_GAIN_CLEAR[6:0], and TRIM_GAIN_IR[6:0] registers.
1 		Use bytes written to the TRIM_GAIN_GREEN[6:0], TRIM_GAIN_RED[6:0], TRIM_GAIN_BLUE[6:0], TRIM_GAIN_
		CLEAR[6:0], and TRIM_GAIN_IR[6:0] registers to set the gain for each channel.
*/
	uint8_t Trim_Adjust_Enabled;
} MAX4400x_Ambient_Configuration_t;
void MAX4400x_SetAmbientConfiguration(MAX4400x_Ambient_Configuration_t AmbientConfiguration);

typedef struct{
/*
BIT0 		µW/cm2 per LSB* 		FULL SCALE (µW/cm2)
0 			2 						2095
1 			16 						16,777
			*At 14-bit resolution, 100ms ADC conversion time.
 */
	uint8_t PRXPGA_IR_Channel_Gain;
/*
BIT1 ADC CONVERSION TIME (ms) 	FULL-SCALE ADC (Counts)		BIT RESOLUTION
0	 6.25 						1024 						10
1 	 1.5625 					256 						8
*/
	uint8_t PRXTIM_IR_Channel_Int_Time;
/*
DRV[3:0] 	LED CURRENT (mA) 			DRV[3:0] 		LED CURRENT (mA)
0000 		LED driver disabled 		0110 			60
0001 		10 							0111 			70
0010 		20 							1000 			80
0011 		30 							1001 			90
0100 		40 							1010 			100
0101 		50 							1011-1111 		110
*/
	uint8_t LED_Drive_Current_Setting;
}MAX4400x_Proximity_Configuration_t;
void MAX4400x_SetProximityConfiguration(MAX4400x_Proximity_Configuration_t ProximityConfiguration);

uint16_t MAX4400x_GetAmbientClear(void);
uint16_t MAX4400x_GetAmbientRed(void);
uint16_t MAX4400x_GetAmbientGreen(void);
uint16_t MAX4400x_GetAmbientBlue(void);
uint16_t MAX4400x_GetAmbientIr(void);
uint16_t MAX4400x_GetAmbientIrComp(void);

uint16_t MAX4400x_GetProximityIr(void);
uint16_t MAX4400x_GetTemperature(void);

uint16_t MAX4400x_GetAmbientUpThreshold(void);
void     MAX4400x_SetAmbientUpThreshold(uint16_t UpThreshold);
uint16_t MAX4400x_GetAmbientLowThreshold(void);
void     MAX4400x_SetAmbientLowThreshold(uint16_t LowThreshold);

typedef struct{
/*PRXPST[1:0] or AMBPST[1:0] 		NO. OF CONSECUTIVE MEASUREMENTS REQUIRED TO
											TRIGGER AN INTERRUPT
			00 											1
			01 											4
			10 											8
			11 											16
*/
	uint8_t Proximity_Persistant_Timer;
	uint8_t Ambient_Persistant_Timer;
}MAX4400x_Threshold_Persist_Timer_t;
void MAX4400x_SetThresholdPersistTimer(MAX4400x_Threshold_Persist_Timer_t ThresholdPersistTimer);

uint16_t MAX4400x_GetProximityUpThreshold(void);
void     MAX4400x_SetProximityUpThreshold(uint16_t UpThreshold);
uint16_t MAX4400x_GetProximityLowThreshold(void);
void     MAX4400x_SetProximityUpThreshold(uint16_t LowThreshold);

uint8_t MAX4400x_GetTrimGainClear(void);
void    MAX4400x_SetTrimGainClear(uint8_t TrimGain);
uint8_t MAX4400x_GetTrimGainRed(void);
void    MAX4400x_SetTrimGainRed(uint8_t TrimGain);
uint8_t MAX4400x_GetTrimGainGreen(void);
void    MAX4400x_SetTrimGainGreen(uint8_t TrimGain);
uint8_t MAX4400x_GetTrimGainBlue(void);
void    MAX4400x_SetTrimGainBlue(uint8_t TrimGain);
uint8_t MAX4400x_GetTrimGainIr(void);
void    MAX4400x_SetTrimGainIr(uint8_t TrimGain);

#endif /* MAX4400X_REG_H_ */
