/*
 * Si114x.h
 *
 *  Created on: 18-07-2015
 *      Author: terg
 */

#ifndef SI114X_REG_H_
#define SI114X_REG_H_

#include <stdint.h>

/*----------------------------------------------------*/
/*							!!!						  */
/* 			Define used sensor for i2c address 		  */
/*							!!!						  */
/*----------------------------------------------------*/
#define SI1147

uint8_t Si114x_Detect_LowLevel(void);

uint8_t Si114x_GetPartId(void);
uint8_t Si114x_GetRevId(void);
uint8_t Si114x_GetSeqId(void);

typedef enum {
	SI114X_INT_OE_DISABLE = 0,
	SI114X_INT_OE_ENABLE = 1
} Si114x_Int_Cfg_t;
void Si114x_SetIntCfg(Si114x_Int_Cfg_t IntCfg);

typedef enum {
	SI114X_ALS_IE_DISABLE = 0,
	SI114X_ALS_IE_ENABLE = 1
} Si114x_Irq_Enable_ALS_t;
typedef enum {
	SI114X_PS3_IE_DISABLE = 0,
	SI114X_PS3_IE_ENABLE = 1
} Si114x_Irq_Enable_PS3_t;
typedef enum {
	SI114X_PS2_IE_DISABLE = 0,
	SI114X_PS2_IE_ENABLE = 1
} Si114x_Irq_Enable_PS2_t;
typedef enum {
	SI114X_PS1_IE_DISABLE = 0,
	SI114X_PS1_IE_ENABLE = 1
} Si114x_Irq_Enable_PS1_t;
void Si114x_SetIrqEnable(Si114x_Irq_Enable_ALS_t IrqEnableALS,
						 Si114x_Irq_Enable_PS1_t IrqEnablePs1,
						 Si114x_Irq_Enable_PS2_t IrqEnablePs2,
						 Si114x_Irq_Enable_PS3_t IrqEnablePs3);

void Si114x_SetHwKey(void);
void Si114x_SetMeasurmentRate(uint16_t measRate);
void Si114x_SetLedCurrents(uint8_t Led1_Current, uint8_t Led2_Current, uint8_t Led3_Current);

void Si114x_EnableUvReading();
void Si114x_DisableUvReading();

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
void Si114x_SetCommand(Si114x_Command_Name_t cmd, uint8_t val);
void Si114x_SetParamWr(uint8_t val);
void Si114x_SetParamRd(uint8_t val);
uint8_t Si114x_GetResponse();

void Si114x_GetIrqStatus(uint8_t *ALS_Int, uint8_t *PS1_Int, uint8_t *PS2_Int, uint8_t *PS3_Int, uint8_t *CMD_Int);
void Si114x_ClrIrqStatus(void);

void Si114x_GetChipStat(uint8_t *Si114x_Sleep, uint8_t *Si114x_Suspend, uint8_t *Si114x_Running);
uint16_t Si114x_GetAlsVisData(void);
uint16_t Si114x_GetAlsIrData(void);
uint16_t Si114x_GetPs1Data(void);
uint16_t Si114x_GetPs2Data(void);
uint16_t Si114x_GetPs3Data(void);
uint16_t Si114x_GetAuxUvData(void);

typedef struct{
	uint8_t UV;
	uint8_t AUX;
	uint8_t ALS_IR;
	uint8_t ALS_Vis;
	uint8_t PS3;
	uint8_t PS2;
	uint8_t PS1;
}Si114x_EnableChannel_t;
void Si114x_EnableChannel(Si114x_EnableChannel_t EnableChannel);

typedef struct{
	uint8_t PS1_Led;
	uint8_t PS2_Led;
	uint8_t PS3_Led;
} Si114x_PsX_Led_Select_t;
#define SI114X_LED1_ENABLE 	 (0x01)
#define SI114X_LED2_ENABLE 	 (0x02)
#define SI114X_LED3_ENABLE 	 (0x04)
void Si114x_SetPsLedSelect(Si114x_PsX_Led_Select_t PsXLedSelect);

void Si114x_SetPsEncoding(uint8_t PS1_Align, uint8_t PS2_Align, uint8_t PS3_Align);
void Si114x_SetAlsEncoding(uint8_t ALS_IR_Align, uint8_t ALS_Vis_Align);

typedef enum{
	SI114X_PS_ADC_SMALL_IR				= 0x00,
	SI114X_PS_ADC_VISIBLE_PHOTODIODE	= 0x02,
	SI114X_PS_ADC_LARGE_IR				= 0x03,
	SI114X_PS_ADC_NO_PHOTODIODE			= 0x06,
	SI114X_PS_ADC_GND_VOLT				= 0x25,
	SI114X_PS_ADC_TEMPERATURE			= 0x65,
	SI114X_PS_ADC_VDD_VOLT				= 0x75
} Si114x_PS_Meas_ADC_t;
void Si114x_SetPs1ADC(Si114x_PS_Meas_ADC_t adc);
void Si114x_SetPs2ADC(Si114x_PS_Meas_ADC_t adc);
void Si114x_SetPs3ADC(Si114x_PS_Meas_ADC_t adc);
typedef enum{
	SI114X_AUX_ADC_SMALL_IR				= 0x00,
	SI114X_AUX_ADC_LARGE_IR				= 0x03,
	SI114X_AUX_ADC_TEMPERATURE			= 0x65,
	SI114X_AUX_ADC_VDD_VOLT				= 0x75
} Si114x_Aux_Meas_ADC_t;
void Si114x_SetAuxADC(Si114x_Aux_Meas_ADC_t adc);

void Si114x_SetPsAdcCounter(uint8_t adcRecPeriod);
void Si114x_SetAlsVisAdcCounter(uint8_t adcRecPeriod);
void Si114x_SetAlsIrAdcCounter(uint8_t adcRecPeriod);

void Si114x_SetPsAdcGain(uint8_t adcGain, uint8_t protectionDisable);
void Si114x_SetAlsVisAdcGain(uint8_t adcGain);
void Si114x_SetAlsIrAdcGain(uint8_t adcGain);

void Si114x_SetPsAdcMisc(uint8_t PsRange, uint8_t PsAdcMode);
void Si114x_SetAlsVisAdcMisc(uint8_t AlsVisRange);
void Si114x_SetAlsIrAdcMisc(uint8_t AlsIrRange);

#endif /* SI114X_REG_H_ */
