#include "AD7147_reg.h"

/* I2C address definition */
#if AD7147_A1 == 0
	#if AD7147_A0 == 0
		#define  AD7147_ADDR (0x2C * 2)
	#elif AD7147_A0 == 1
		#define AD7147_ADDR	(0x2D * 2)
	#endif
#elif AD7147_A1 == 1
	#if AD7147_A0 == 0
		#define AD7147_ADDR (0x2E * 2)
	#elif AD7147_A0 == 1
		#define AD7147_ADDR (0x2F * 2)
	#endif
#endif

#ifndef TRUE
	#define TRUE	(1)
#endif
#ifndef FALSE
	#define FALSE	(0)
#endif
#if TRUE == FALSE
	#error("TRUE macro == FALSE macro");
#endif


uint8_t ADP7147_Detect_LowLevel(void)
{
	return i2c_Detect(I2C0, ADP7147_ADDR);
}

typedef enum{
	AD7147_PWR_CONTROL_REG						= 0x000,
	AD7147_STAGE_CAL_EN_REG						= 0x001,
	AD7147_AMB_COMP_CTRL0_REG					= 0x002,
	AD7147_AMB_COMP_CTRL1_REG					= 0x003,
	AD7147_AMB_COMP_CTRL2_REG					= 0x004,
	AD7147_STAGE_LOW_INT_ENABLE_REG 			= 0x005,
	AD7147_STAGE_HIGH_INT_ENABLE_REG 			= 0x006,
	AD7147_STAGE_COMPLETE_INT_ENABLE_REG 		= 0x007,
	AD7147_STAGE_LOW_INT_STATUS_REG 			= 0x008,
	AD7147_STAGE_HIGH_INT_STATUS_REG 			= 0x009,
	AD7147_STAGE_COMPLETE_INT_STATUS_REG 		= 0x00A,

	AD7147_CDC_RESULT_S0_REG					= 0x00B,
	AD7147_CDC_RESULT_S1_REG					= 0x00C,
	AD7147_CDC_RESULT_S2_REG					= 0x00D,
	AD7147_CDC_RESULT_S3_REG					= 0x00E,
	AD7147_CDC_RESULT_S4_REG					= 0x00F,
	AD7147_CDC_RESULT_S5_REG					= 0x010,
	AD7147_CDC_RESULT_S6_REG					= 0x011,
	AD7147_CDC_RESULT_S7_REG					= 0x012,
	AD7147_CDC_RESULT_S8_REG					= 0x013,
	AD7147_CDC_RESULT_S9_REG					= 0x014,
	AD7147_CDC_RESULT_S10_REG					= 0x015,
	AD7147_CDC_RESULT_S11_REG					= 0x016,

	AD7147_DEVICE_ID_REG						= 0x017,

	AD7147_PROXIMITY_STATUS_REG					= 0x042,
} ADP7147_Registers_t;

static void ADP7147_SetRegister(ADP7147_Registers_t reg, uint16_t val)
{
	/* This ic requires a 16b register address and 16 bit data value */
	uint8_t data[4];
	data[0] = (uint8_t)(reg >> 8);
	data[1] = (uint8_t)reg;
	data[2] = (uint8_t)(val >> 8);
	data[3] = (uint8_t)val;
	i2c_Register_Write_Block (I2C0, ADP7147_ADDR, data[0], 3, &data[1]);
}

/* Not implemented correctly yet */
/* TODO this */
#if (0)
static void ADP7147_GetRegister(ADP7147_Registers_t ADP7147_reg, uint16_t *val)
{
	i2c_RegisterGet(I2C0, ADP7147_ADDR, (uint8_t)ADP7147_reg, val);
}
#endif


#define AD7147_POWER_MODE_SHIFT			0
#define AD7147_LP_CONV_DELAY_SHIFT		2
#define AD7147_SEQUENCE_STAGE_NUM_SHIFT	4
#define AD7147_DECIMATION_SHIFT			8
#define AD7147_SW_RESET_SHIFT			10
#define AD7147_INT_POL_SHIFT			11
#define AD7147_EXT_SOURCE_SHIFT			12
#define AD7147_CDC_BIAS_SHIFT			14

#define AD7147_POWER_MODE_MASK			0x03
#define AD7147_LP_CONV_DELAY_MASK		0x03
#define AD7147_SEQUENCE_STAGE_NUM_MASK	0x0F
#define AD7147_DECIMATION_MASK			0x03
#define AD7147_SW_RESET_MASK			0x01
#define AD7147_INT_POL_MASK				0x01
#define AD7147_EXT_SOURCE_MASK			0x01
#define AD7147_CDC_BIAS_MASK			0x03
void AD7147_SetPowerControl(AD7147_PowerControl_t powerCtrl)
{
	uint16_t val = 0x0000;
	val = (((uint16_t)powerCtrl.AD7147_Power_Mode_e  				& AD7147_POWER_MODE_MASK) 			<< AD7147_POWER_MODE_SHIFT)
		+ (((uint16_t)powerCtrl.AD7147_Low_Power_Conv_delay_e  		& AD7147_LP_CONV_DELAY_MASK) 		<< AD7147_LP_CONV_DELAY_SHIFT)
		+ (((uint16_t)powerCtrl.AD7147_Sequence_Stage_Number  		& AD7147_SEQUENCE_STAGE_NUM_MASK) 	<< AD7147_SEQUENCE_STAGE_NUM_SHIFT)
		+ (((uint16_t)powerCtrl.AD7147_Decimation_e  				& AD7147_DECIMATION_MASK) 			<< AD7147_DECIMATION_SHIFT)
		+ (((uint16_t)powerCtrl.AD7147_Sw_Reset_e  					& AD7147_SW_RESET_MASK) 			<< AD7147_SW_RESET_SHIFT)
		+ (((uint16_t)powerCtrl.AD7147_Int_Poling_Control_e  		& AD7147_INT_POL_MASK) 				<< AD7147_INT_POL_SHIFT)
		+ (((uint16_t)powerCtrl.AD7147_Excitation_Source_Control_e  & AD7147_EXT_SOURCE_MASK) 			<< AD7147_EXT_SOURCE_SHIFT)
		+ (((uint16_t)powerCtrl.AD7147_CDC_Bias_e  					& AD7147_CDC_BIAS_MASK) 			<< AD7147_CDC_BIAS_SHIFT);
	ADP7147_SetRegister(AD7147_PWR_CONTROL_REG, val);
}

#define AD7147_STAGE_CAL_STAGE0_CAL_EN_SHIFT	0
#define AD7147_STAGE_CAL_STAGE1_CAL_EN_SHIFT	1
#define AD7147_STAGE_CAL_STAGE2_CAL_EN_SHIFT	2
#define AD7147_STAGE_CAL_STAGE3_CAL_EN_SHIFT	3
#define AD7147_STAGE_CAL_STAGE4_CAL_EN_SHIFT	4
#define AD7147_STAGE_CAL_STAGE5_CAL_EN_SHIFT	5
#define AD7147_STAGE_CAL_STAGE6_CAL_EN_SHIFT	6
#define AD7147_STAGE_CAL_STAGE7_CAL_EN_SHIFT	7
#define AD7147_STAGE_CAL_STAGE8_CAL_EN_SHIFT	8
#define AD7147_STAGE_CAL_STAGE9_CAL_EN_SHIFT	9
#define AD7147_STAGE_CAL_STAGE10_CAL_EN_SHIFT	10
#define AD7147_STAGE_CAL_STAGE11_CAL_EN_SHIFT	11
#define AD7147_STAGE_CAL_AVG_FP_SKIP_SHIFT		12
#define AD7147_STAGE_CAL_AVG_LP_SKIP_SHIFT		14
#define AD7147_STAGE_CAL_AVG_FP_SKIP_MASK		0x03
#define AD7147_STAGE_CAL_AVG_LP_SKIP_MASK		0x03
void AD7147_SetStageCal(AD7147_StageCal_t stageCal)
{
	uint16_t val = 0x0000;
	val = (((uint16_t)stageCal.AD7147_Stage_Cal_En_s.Stage0  		& 0x01) 							 << AD7147_STAGE_CAL_STAGE0_CAL_EN_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Stage_Cal_En_s.Stage1  		& 0x01) 							 << AD7147_STAGE_CAL_STAGE1_CAL_EN_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Stage_Cal_En_s.Stage2  		& 0x01) 							 << AD7147_STAGE_CAL_STAGE2_CAL_EN_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Stage_Cal_En_s.Stage3  		& 0x01) 							 << AD7147_STAGE_CAL_STAGE3_CAL_EN_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Stage_Cal_En_s.Stage4  		& 0x01) 							 << AD7147_STAGE_CAL_STAGE4_CAL_EN_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Stage_Cal_En_s.Stage5  		& 0x01) 							 << AD7147_STAGE_CAL_STAGE5_CAL_EN_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Stage_Cal_En_s.Stage6  		& 0x01) 							 << AD7147_STAGE_CAL_STAGE6_CAL_EN_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Stage_Cal_En_s.Stage7  		& 0x01) 							 << AD7147_STAGE_CAL_STAGE7_CAL_EN_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Stage_Cal_En_s.Stage8  		& 0x01) 							 << AD7147_STAGE_CAL_STAGE8_CAL_EN_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Stage_Cal_En_s.Stage9  		& 0x01) 							 << AD7147_STAGE_CAL_STAGE9_CAL_EN_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Stage_Cal_En_s.Stage10 		& 0x01) 							 << AD7147_STAGE_CAL_STAGE10_CAL_EN_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Stage_Cal_En_s.Stage11 		& 0x01) 							 << AD7147_STAGE_CAL_STAGE11_CAL_EN_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Avg_Full_Power_Skip_Control_e & AD7147_STAGE_CAL_AVG_FP_SKIP_MASK) << AD7147_STAGE_CAL_AVG_FP_SKIP_SHIFT)
		+ (((uint16_t)stageCal.AD7147_Avg_Low_Power_Skip_Control_e  & AD7147_STAGE_CAL_AVG_LP_SKIP_MASK) << AD7147_STAGE_CAL_AVG_LP_SKIP_SHIFT);

	ADP7147_SetRegister(AD7147_STAGE_CAL_EN_REG, val);
}

#define AD7147_FF_SKIP_CNT_SHIFT				0
#define AD7147_FP_PROXIMITY_CNT_SHIFT			4
#define AD7147_LP_PROXIMITY_CNT_SHIFT			8
#define AD7147_PWR_DOWN_TIMEOUT_SHIFT			12
#define AD7147_FORCED_CAL_SHIFT					14
#define AD7147_CONV_RESET_SHIFT					15
#define AD7147_PROXIMITY_RECAL_LVL_SHIFT		0
#define AD7147_PROXIMITY_DETECTION_RATE_SHIFT	8
#define AD7147_SLOW_FILTER_UPDATE_LVL_SHIFT		14
#define AD7147_FP_PROXIMITY_RECAL_SHIFT			0
#define AD7147_LP_PROXIMITY_RECAL_SHIFT			10

#define AD7147_FF_SKIP_CNT_MASK					0x000F
#define AD7147_FP_PROXIMITY_CNT_MASK			0x00F0
#define AD7147_LP_PROXIMITY_CNT_MASK			0x0F00
#define AD7147_PWR_DOWN_TIMEOUT_MASK			0x3000
#define AD7147_FORCED_CAL_MASK					0x4000
#define AD7147_CONV_RESET_MASK					0x8000
#define AD7147_PROXIMITY_RECAL_LVL_MASK			0x00FF
#define AD7147_PROXIMITY_DETECTION_RATE_MASK	0x3F00
#define AD7147_SLOW_FILTER_UPDATE_LVL_MASK		0xC000
#define AD7147_FP_PROXIMITY_RECAL_MASK			0x03FF
#define AD7147_LP_PROXIMITY_RECAL_MASK			0xFC00
void AD7147_SetAmbientCompensationControl(AD7147_AmbCompCtrl_t ambCompCtrl)
{
	uint16_t val1;
	val1 = (((uint16_t)ambCompCtrl.AD7147_Fast_Filter_Skip_Control 				& AD7147_FF_SKIP_CNT_MASK) 				<< AD7147_FF_SKIP_CNT_SHIFT)
		 + (((uint16_t)ambCompCtrl.AD7147_Full_Power_Proximity_Disable_Period 	& AD7147_FP_PROXIMITY_CNT_MASK) 		<< AD7147_FP_PROXIMITY_CNT_SHIFT)
		 + (((uint16_t)ambCompCtrl.AD7147_Low_Power_Proximity_Disable_Period 	& AD7147_LP_PROXIMITY_CNT_MASK) 		<< AD7147_LP_PROXIMITY_CNT_SHIFT)
		 + (((uint16_t)ambCompCtrl.AD7147_Power_Down_Timeout_e 					& AD7147_PWR_DOWN_TIMEOUT_MASK) 		<< AD7147_PWR_DOWN_TIMEOUT_SHIFT)
		 + (((uint16_t)ambCompCtrl.AD7147_Forced_Calibration_e					& AD7147_FORCED_CAL_MASK) 				<< AD7147_FORCED_CAL_SHIFT)
		 + (((uint16_t)ambCompCtrl.AD7147_Conversion_Reset_e 					& AD7147_CONV_RESET_MASK) 				<< AD7147_CONV_RESET_SHIFT);

	ADP7147_SetRegister(AD7147_AMB_COMP_CTRL0_REG, val1);

	uint16_t val2;
	val1 = (((uint16_t)ambCompCtrl.AD7147_Proximity_Recalibration_Level 		& AD7147_PROXIMITY_RECAL_LVL_MASK) 		<< AD7147_PROXIMITY_RECAL_LVL_SHIFT)
		 + (((uint16_t)ambCompCtrl.AD7147_Proximity_Detection_Rate 				& AD7147_PROXIMITY_DETECTION_RATE_MASK) << AD7147_PROXIMITY_DETECTION_RATE_SHIFT)
		 + (((uint16_t)ambCompCtrl.AD7147_Slow_Filter_Update_Level 				& AD7147_SLOW_FILTER_UPDATE_LVL_MASK) 	<< AD7147_SLOW_FILTER_UPDATE_LVL_SHIFT);
	ADP7147_SetRegister(AD7147_AMB_COMP_CTRL1_REG, val2);

	uint16_t val3;
	val1 = (((uint16_t)ambCompCtrl.AD7147_Full_Power_Proximity_Recalibration 	& AD7147_FP_PROXIMITY_RECAL_MASK) 		<< AD7147_FP_PROXIMITY_RECAL_SHIFT)
		 + (((uint16_t)ambCompCtrl.AD7147_Low_Power_Proximity_Recalibration 	& AD7147_LP_PROXIMITY_RECAL_MASK) 		<< AD7147_LP_PROXIMITY_RECAL_SHIFT);
	ADP7147_SetRegister(AD7147_AMB_COMP_CTRL2_REG, val3);
}

#define AD7147_STAGE_LOW_INT_STAGE0_SHIFT			0
#define AD7147_STAGE_LOW_INT_STAGE1_SHIFT			1
#define AD7147_STAGE_LOW_INT_STAGE2_SHIFT			2
#define AD7147_STAGE_LOW_INT_STAGE3_SHIFT			3
#define AD7147_STAGE_LOW_INT_STAGE4_SHIFT			4
#define AD7147_STAGE_LOW_INT_STAGE5_SHIFT			5
#define AD7147_STAGE_LOW_INT_STAGE6_SHIFT			6
#define AD7147_STAGE_LOW_INT_STAGE7_SHIFT			7
#define AD7147_STAGE_LOW_INT_STAGE8_SHIFT			8
#define AD7147_STAGE_LOW_INT_STAGE9_SHIFT			9
#define AD7147_STAGE_LOW_INT_STAGE10_SHIFT			10
#define AD7147_STAGE_LOW_INT_STAGE11_SHIFT			11
#define AD7147_STAGE_LOW_INT_ENABLE_GPIO_SETUP_SHIFT		12
#define AD7147_STAGE_LOW_INT_ENABLE_GPIO_INPUT_CONFIG_SHIFT	14
#define AD7147_STAGE_LOW_INT_ENABLE_GPIO_SETUP_MASK			0x3000
#define AD7147_STAGE_LOW_INT_ENABLE_GPIO_INPUT_CONFIG_MASK	0xC000
void AD7147_SetStageLowInterruptEnable(AD7147_Stage_Low_Int_t stageLowIntEn)
{
	uint16_t val = 0x0000;
	val = (((uint16_t)stageLowIntEn.AD7147_Stage_Low_Int_En_s.Stage0  		& 0x01) 							 					<< AD7147_STAGE_LOW_INT_STAGE0_SHIFT)
		+ (((uint16_t)stageLowIntEn.AD7147_Stage_Low_Int_En_s.Stage1  		& 0x01) 							 					<< AD7147_STAGE_LOW_INT_STAGE1_SHIFT)
		+ (((uint16_t)stageLowIntEn.AD7147_Stage_Low_Int_En_s.Stage2  		& 0x01) 							 					<< AD7147_STAGE_LOW_INT_STAGE2_SHIFT)
		+ (((uint16_t)stageLowIntEn.AD7147_Stage_Low_Int_En_s.Stage3  		& 0x01) 							 					<< AD7147_STAGE_LOW_INT_STAGE3_SHIFT)
		+ (((uint16_t)stageLowIntEn.AD7147_Stage_Low_Int_En_s.Stage4  		& 0x01) 							 					<< AD7147_STAGE_LOW_INT_STAGE4_SHIFT)
		+ (((uint16_t)stageLowIntEn.AD7147_Stage_Low_Int_En_s.Stage5  		& 0x01) 							 					<< AD7147_STAGE_LOW_INT_STAGE5_SHIFT)
		+ (((uint16_t)stageLowIntEn.AD7147_Stage_Low_Int_En_s.Stage6  		& 0x01) 							 					<< AD7147_STAGE_LOW_INT_STAGE6_SHIFT)
		+ (((uint16_t)stageLowIntEn.AD7147_Stage_Low_Int_En_s.Stage7  		& 0x01) 							 					<< AD7147_STAGE_LOW_INT_STAGE7_SHIFT)
		+ (((uint16_t)stageLowIntEn.AD7147_Stage_Low_Int_En_s.Stage8  		& 0x01) 												<< AD7147_STAGE_LOW_INT_STAGE8_SHIFT)
		+ (((uint16_t)stageLowIntEn.AD7147_Stage_Low_Int_En_s.Stage9  		& 0x01) 							 					<< AD7147_STAGE_LOW_INT_STAGE9_SHIFT)
		+ (((uint16_t)stageLowIntEn.AD7147_Stage_Low_Int_En_s.Stage10 		& 0x01) 							 					<< AD7147_STAGE_LOW_INT_STAGE10_SHIFT)
		+ (((uint16_t)stageLowIntEn.AD7147_Stage_Low_Int_En_s.Stage11 		& 0x01) 							 					<< AD7147_STAGE_LOW_INT_STAGE11_SHIFT)
		+ (((uint16_t)stageLowIntEn.ADP7147_Gpio_Setup_e 					& AD7147_STAGE_LOW_INT_ENABLE_GPIO_SETUP_MASK) 			<< AD7147_STAGE_LOW_INT_ENABLE_GPIO_SETUP_SHIFT)
		+ (((uint16_t)stageLowIntEn.ADP7147_Gpio_Input_Configuration_e  	& AD7147_STAGE_LOW_INT_ENABLE_GPIO_INPUT_CONFIG_MASK) 	<< AD7147_STAGE_LOW_INT_ENABLE_GPIO_INPUT_CONFIG_SHIFT);

	ADP7147_SetRegister(AD7147_STAGE_LOW_INT_ENABLE_REG, val);
}

#define AD7147_STAGE_HIGH_INT_STAGE0_SHIFT			0
#define AD7147_STAGE_HIGH_INT_STAGE1_SHIFT			1
#define AD7147_STAGE_HIGH_INT_STAGE2_SHIFT			2
#define AD7147_STAGE_HIGH_INT_STAGE3_SHIFT			3
#define AD7147_STAGE_HIGH_INT_STAGE4_SHIFT			4
#define AD7147_STAGE_HIGH_INT_STAGE5_SHIFT			5
#define AD7147_STAGE_HIGH_INT_STAGE6_SHIFT			6
#define AD7147_STAGE_HIGH_INT_STAGE7_SHIFT			7
#define AD7147_STAGE_HIGH_INT_STAGE8_SHIFT			8
#define AD7147_STAGE_HIGH_INT_STAGE9_SHIFT			9
#define AD7147_STAGE_HIGH_INT_STAGE10_SHIFT			10
#define AD7147_STAGE_HIGH_INT_STAGE11_SHIFT			11
void AD7147_SetStageHighInterruptEnable(AD7147_Stage_High_Int_t stageHighIntEn)
{
	uint16_t val = 0x0000;
	val = (((uint16_t)stageHighIntEn.AD7147_Stage_High_Int_En_s.Stage0  		& 0x01) 							 					<< AD7147_STAGE_HIGH_INT_STAGE0_SHIFT)
		+ (((uint16_t)stageHighIntEn.AD7147_Stage_High_Int_En_s.Stage1  		& 0x01) 							 					<< AD7147_STAGE_HIGH_INT_STAGE1_SHIFT)
		+ (((uint16_t)stageHighIntEn.AD7147_Stage_High_Int_En_s.Stage2  		& 0x01) 							 					<< AD7147_STAGE_HIGH_INT_STAGE2_SHIFT)
		+ (((uint16_t)stageHighIntEn.AD7147_Stage_High_Int_En_s.Stage3  		& 0x01) 							 					<< AD7147_STAGE_HIGH_INT_STAGE3_SHIFT)
		+ (((uint16_t)stageHighIntEn.AD7147_Stage_High_Int_En_s.Stage4  		& 0x01) 							 					<< AD7147_STAGE_HIGH_INT_STAGE4_SHIFT)
		+ (((uint16_t)stageHighIntEn.AD7147_Stage_High_Int_En_s.Stage5  		& 0x01) 							 					<< AD7147_STAGE_HIGH_INT_STAGE5_SHIFT)
		+ (((uint16_t)stageHighIntEn.AD7147_Stage_High_Int_En_s.Stage6  		& 0x01) 							 					<< AD7147_STAGE_HIGH_INT_STAGE6_SHIFT)
		+ (((uint16_t)stageHighIntEn.AD7147_Stage_High_Int_En_s.Stage7  		& 0x01) 							 					<< AD7147_STAGE_HIGH_INT_STAGE7_SHIFT)
		+ (((uint16_t)stageHighIntEn.AD7147_Stage_High_Int_En_s.Stage8  		& 0x01) 												<< AD7147_STAGE_HIGH_INT_STAGE8_SHIFT)
		+ (((uint16_t)stageHighIntEn.AD7147_Stage_High_Int_En_s.Stage9  		& 0x01) 							 					<< AD7147_STAGE_HIGH_INT_STAGE9_SHIFT)
		+ (((uint16_t)stageHighIntEn.AD7147_Stage_High_Int_En_s.Stage10 		& 0x01) 							 					<< AD7147_STAGE_HIGH_INT_STAGE10_SHIFT)
		+ (((uint16_t)stageHighIntEn.AD7147_Stage_High_Int_En_s.Stage11 		& 0x01) 							 					<< AD7147_STAGE_HIGH_INT_STAGE11_SHIFT);
	ADP7147_SetRegister(AD7147_STAGE_HIGH_INT_ENABLE_REG, val);
}

#define AD7147_STAGE_COMPLETE_INT_STAGE0_SHIFT			0
#define AD7147_STAGE_COMPLETE_INT_STAGE1_SHIFT			1
#define AD7147_STAGE_COMPLETE_INT_STAGE2_SHIFT			2
#define AD7147_STAGE_COMPLETE_INT_STAGE3_SHIFT			3
#define AD7147_STAGE_COMPLETE_INT_STAGE4_SHIFT			4
#define AD7147_STAGE_COMPLETE_INT_STAGE5_SHIFT			5
#define AD7147_STAGE_COMPLETE_INT_STAGE6_SHIFT			6
#define AD7147_STAGE_COMPLETE_INT_STAGE7_SHIFT			7
#define AD7147_STAGE_COMPLETE_INT_STAGE8_SHIFT			8
#define AD7147_STAGE_COMPLETE_INT_STAGE9_SHIFT			9
#define AD7147_STAGE_COMPLETE_INT_STAGE10_SHIFT			10
#define AD7147_STAGE_COMPLETE_INT_STAGE11_SHIFT			11
#define AD7147_STAGE_COMPLETE_INT_ENABLE_GPIO_INT_EN_SHIFT		12
void AD7147_SetStageCompleteInterruptEnable(AD7147_Stage_Complete_Int_t stageCompleteIntEn)
{
	uint16_t val = 0x0000;
	val = (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage0  		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_STAGE0_SHIFT)
		+ (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage1  		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_STAGE1_SHIFT)
		+ (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage2  		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_STAGE2_SHIFT)
		+ (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage3  		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_STAGE3_SHIFT)
		+ (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage4  		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_STAGE4_SHIFT)
		+ (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage5  		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_STAGE5_SHIFT)
		+ (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage6  		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_STAGE6_SHIFT)
		+ (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage7  		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_STAGE7_SHIFT)
		+ (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage8  		& 0x01) 												<< AD7147_STAGE_COMPLETE_INT_STAGE8_SHIFT)
		+ (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage9  		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_STAGE9_SHIFT)
		+ (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage10 		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_STAGE10_SHIFT)
		+ (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage11 		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_STAGE11_SHIFT);
		+ (((uint16_t)stageCompleteIntEn.AD7147_Stage_Complete_Int_En_s.Stage11 		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_STAGE11_SHIFT);
		+ (((uint16_t)stageCompleteIntEn.AD7147_GPIO_Int_En						 		& 0x01) 							 					<< AD7147_STAGE_COMPLETE_INT_ENABLE_GPIO_INT_EN_SHIFT);
	ADP7147_SetRegister(AD7147_STAGE_COMPLETE_INT_ENABLE_REG, val);
}

AD7147_Stage_Low_Int_t AD7147_GetStageLowIntStatus(void)
{
	uint16_t val = 0x0000;
	AD7147_Stage_Low_Int_t ret;

	ret.AD7147_Stage_Low_Int_En_s.Stage0 	= FALSE;
	ret.AD7147_Stage_Low_Int_En_s.Stage1 	= FALSE;
	ret.AD7147_Stage_Low_Int_En_s.Stage2 	= FALSE;
	ret.AD7147_Stage_Low_Int_En_s.Stage3 	= FALSE;
	ret.AD7147_Stage_Low_Int_En_s.Stage4 	= FALSE;
	ret.AD7147_Stage_Low_Int_En_s.Stage5 	= FALSE;
	ret.AD7147_Stage_Low_Int_En_s.Stage6 	= FALSE;
	ret.AD7147_Stage_Low_Int_En_s.Stage7 	= FALSE;
	ret.AD7147_Stage_Low_Int_En_s.Stage8 	= FALSE;
	ret.AD7147_Stage_Low_Int_En_s.Stage9 	= FALSE;
	ret.AD7147_Stage_Low_Int_En_s.Stage10 	= FALSE;
	ret.AD7147_Stage_Low_Int_En_s.Stage11	= FALSE;
	ret.ADP7147_Gpio_Input_Configuration_e 	= ADP7147_GPIO_INPUT_CONF_UNK;
	ret.ADP7147_Gpio_Setup_e 				= ADP7147_GPIO_SETUP_GPIO_UNK;

	ADP7147_GetRegister(AD7147_STAGE_LOW_INT_STATUS_REG, &val);

	if (val & ((uint16_t)1 << AD7147_STAGE_LOW_INT_STAGE0_SHIFT))
	{
		ret.AD7147_Stage_Low_Int_En_s.Stage0 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_LOW_INT_STAGE1_SHIFT))
	{
		ret.AD7147_Stage_Low_Int_En_s.Stage1 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_LOW_INT_STAGE2_SHIFT))
	{
		ret.AD7147_Stage_Low_Int_En_s.Stage2 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_LOW_INT_STAGE3_SHIFT))
	{
		ret.AD7147_Stage_Low_Int_En_s.Stage3 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_LOW_INT_STAGE4_SHIFT))
	{
		ret.AD7147_Stage_Low_Int_En_s.Stage4 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_LOW_INT_STAGE5_SHIFT))
	{
		ret.AD7147_Stage_Low_Int_En_s.Stage5 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_LOW_INT_STAGE6_SHIFT))
	{
		ret.AD7147_Stage_Low_Int_En_s.Stage6 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_LOW_INT_STAGE7_SHIFT))
	{
		ret.AD7147_Stage_Low_Int_En_s.Stage7 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_LOW_INT_STAGE8_SHIFT))
	{
		ret.AD7147_Stage_Low_Int_En_s.Stage8 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_LOW_INT_STAGE9_SHIFT))
	{
		ret.AD7147_Stage_Low_Int_En_s.Stage9 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_LOW_INT_STAGE10_SHIFT))
	{
		ret.AD7147_Stage_Low_Int_En_s.Stage10 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_LOW_INT_STAGE11_SHIFT))
	{
		ret.AD7147_Stage_Low_Int_En_s.Stage11 = TRUE;
	}
}

AD7147_Stage_High_Int_t AD7147_GetStageHighIntStatus(void)
{
	uint16_t val = 0x0000;
	AD7147_Stage_High_Int_t ret;

	ret.AD7147_Stage_High_Int_En_s.Stage0 	= FALSE;
	ret.AD7147_Stage_High_Int_En_s.Stage1 	= FALSE;
	ret.AD7147_Stage_High_Int_En_s.Stage2 	= FALSE;
	ret.AD7147_Stage_High_Int_En_s.Stage3 	= FALSE;
	ret.AD7147_Stage_High_Int_En_s.Stage4 	= FALSE;
	ret.AD7147_Stage_High_Int_En_s.Stage5 	= FALSE;
	ret.AD7147_Stage_High_Int_En_s.Stage6 	= FALSE;
	ret.AD7147_Stage_High_Int_En_s.Stage7 	= FALSE;
	ret.AD7147_Stage_High_Int_En_s.Stage8 	= FALSE;
	ret.AD7147_Stage_High_Int_En_s.Stage9 	= FALSE;
	ret.AD7147_Stage_High_Int_En_s.Stage10 	= FALSE;
	ret.AD7147_Stage_High_Int_En_s.Stage11	= FALSE;

	ADP7147_GetRegister(AD7147_STAGE_HIGH_INT_STATUS_REG, &val);

	if (val & ((uint16_t)1 << AD7147_STAGE_HIGH_INT_STAGE0_SHIFT))
	{
		ret.AD7147_Stage_High_Int_En_s.Stage0 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_HIGH_INT_STAGE1_SHIFT))
	{
		ret.AD7147_Stage_High_Int_En_s.Stage1 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_HIGH_INT_STAGE2_SHIFT))
	{
		ret.AD7147_Stage_High_Int_En_s.Stage2 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_HIGH_INT_STAGE3_SHIFT))
	{
		ret.AD7147_Stage_High_Int_En_s.Stage3 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_HIGH_INT_STAGE4_SHIFT))
	{
		ret.AD7147_Stage_High_Int_En_s.Stage4 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_HIGH_INT_STAGE5_SHIFT))
	{
		ret.AD7147_Stage_High_Int_En_s.Stage5 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_HIGH_INT_STAGE6_SHIFT))
	{
		ret.AD7147_Stage_High_Int_En_s.Stage6 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_HIGH_INT_STAGE7_SHIFT))
	{
		ret.AD7147_Stage_High_Int_En_s.Stage7 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_HIGH_INT_STAGE8_SHIFT))
	{
		ret.AD7147_Stage_High_Int_En_s.Stage8 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_HIGH_INT_STAGE9_SHIFT))
	{
		ret.AD7147_Stage_High_Int_En_s.Stage9 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_HIGH_INT_STAGE10_SHIFT))
	{
		ret.AD7147_Stage_High_Int_En_s.Stage10 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_HIGH_INT_STAGE11_SHIFT))
	{
		ret.AD7147_Stage_High_Int_En_s.Stage11 = TRUE;
	}
}

AD7147_Stage_Complete_Int_t AD7147_GetStageCompleteIntStatus(void)
{
	uint16_t val = 0x0000;
	AD7147_Stage_Complete_Int_t ret;

	ret.AD7147_Stage_Complete_Int_En_s.Stage0 	= FALSE;
	ret.AD7147_Stage_Complete_Int_En_s.Stage1 	= FALSE;
	ret.AD7147_Stage_Complete_Int_En_s.Stage2 	= FALSE;
	ret.AD7147_Stage_Complete_Int_En_s.Stage3 	= FALSE;
	ret.AD7147_Stage_Complete_Int_En_s.Stage4 	= FALSE;
	ret.AD7147_Stage_Complete_Int_En_s.Stage5 	= FALSE;
	ret.AD7147_Stage_Complete_Int_En_s.Stage6 	= FALSE;
	ret.AD7147_Stage_Complete_Int_En_s.Stage7 	= FALSE;
	ret.AD7147_Stage_Complete_Int_En_s.Stage8 	= FALSE;
	ret.AD7147_Stage_Complete_Int_En_s.Stage9 	= FALSE;
	ret.AD7147_Stage_Complete_Int_En_s.Stage10 	= FALSE;
	ret.AD7147_Stage_Complete_Int_En_s.Stage11	= FALSE;
	ret.AD7147_Stage_Complete_Int_En_s 			= ADP7147_GPIO_COMPLETE_INT_UNK;

	ADP7147_GetRegister(AD7147_STAGE_COMPLETE_INT_STATUS_REG, &val);

	if (val & ((uint16_t)1 << AD7147_STAGE_COMPLETE_INT_STAGE0_SHIFT))
	{
		ret.AD7147_Stage_Complete_Int_En_s.Stage0 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_COMPLETE_INT_STAGE1_SHIFT))
	{
		ret.AD7147_Stage_Complete_Int_En_s.Stage1 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_COMPLETE_INT_STAGE2_SHIFT))
	{
		ret.AD7147_Stage_Complete_Int_En_s.Stage2 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_COMPLETE_INT_STAGE3_SHIFT))
	{
		ret.AD7147_Stage_Complete_Int_En_s.Stage3 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_COMPLETE_INT_STAGE4_SHIFT))
	{
		ret.AD7147_Stage_Complete_Int_En_s.Stage4 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_COMPLETE_INT_STAGE5_SHIFT))
	{
		ret.AD7147_Stage_Complete_Int_En_s.Stage5 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_COMPLETE_INT_STAGE6_SHIFT))
	{
		ret.AD7147_Stage_Complete_Int_En_s.Stage6 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_COMPLETE_INT_STAGE7_SHIFT))
	{
		ret.AD7147_Stage_Complete_Int_En_s.Stage7 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_COMPLETE_INT_STAGE8_SHIFT))
	{
		ret.AD7147_Stage_Complete_Int_En_s.Stage8 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_COMPLETE_INT_STAGE9_SHIFT))
	{
		ret.AD7147_Stage_Complete_Int_En_s.Stage9 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_COMPLETE_INT_STAGE10_SHIFT))
	{
		ret.AD7147_Stage_Complete_Int_En_s.Stage10 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_STAGE_COMPLETE_INT_STAGE11_SHIFT))
	{
		ret.AD7147_Stage_Complete_Int_En_s.Stage11 = TRUE;
	}
}

uint16_t AD7147_GetCdcResult(uint8_t stageNumb)
{
	uint16_t val = 0x0000;
	if (stageNumb > 11)
	{
		stageNumb = 11;
	}
	ADP7147_GetRegister(AD7147_CDC_RESULT_S0_REG + stageNumb, &val);
	return val;
}

#define AD7147_PROXIMITY_STATUS_STAGE0_SHIFT	0
#define AD7147_PROXIMITY_STATUS_STAGE1_SHIFT	1
#define AD7147_PROXIMITY_STATUS_STAGE2_SHIFT	2
#define AD7147_PROXIMITY_STATUS_STAGE3_SHIFT	3
#define AD7147_PROXIMITY_STATUS_STAGE4_SHIFT	4
#define AD7147_PROXIMITY_STATUS_STAGE5_SHIFT	5
#define AD7147_PROXIMITY_STATUS_STAGE6_SHIFT	6
#define AD7147_PROXIMITY_STATUS_STAGE7_SHIFT	7
#define AD7147_PROXIMITY_STATUS_STAGE8_SHIFT	8
#define AD7147_PROXIMITY_STATUS_STAGE9_SHIFT	9
#define AD7147_PROXIMITY_STATUS_STAGE10_SHIFT	10
#define AD7147_PROXIMITY_STATUS_STAGE11_SHIFT	11
AD7147_Proximity_Status_t AD7147_GetProximityStatus(void)
{
	uint16_t val = 0x0000;
	AD7147_Proximity_Status_t ret;

	ret.Stage0 	= FALSE;
	ret.Stage1 	= FALSE;
	ret.Stage2 	= FALSE;
	ret.Stage3 	= FALSE;
	ret.Stage4 	= FALSE;
	ret.Stage5 	= FALSE;
	ret.Stage6 	= FALSE;
	ret.Stage7 	= FALSE;
	ret.Stage8 	= FALSE;
	ret.Stage9 	= FALSE;
	ret.Stage10 = FALSE;
	ret.Stage11	= FALSE;

	ADP7147_GetRegister(AD7147_PROXIMITY_STATUS_REG, &val);

	if (val & ((uint16_t)1 << AD7147_PROXIMITY_STATUS_STAGE0_SHIFT))
	{
		ret.Stage0 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_PROXIMITY_STATUS_STAGE1_SHIFT))
	{
		ret.Stage1 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_PROXIMITY_STATUS_STAGE2_SHIFT))
	{
		ret.Stage2 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_PROXIMITY_STATUS_STAGE3_SHIFT))
	{
		ret.Stage3 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_PROXIMITY_STATUS_STAGE4_SHIFT))
	{
		ret.Stage4 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_PROXIMITY_STATUS_STAGE5_SHIFT))
	{
		ret.Stage5 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_PROXIMITY_STATUS_STAGE6_SHIFT))
	{
		ret.Stage6 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_PROXIMITY_STATUS_STAGE7_SHIFT))
	{
		ret.Stage7 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_PROXIMITY_STATUS_STAGE8_SHIFT))
	{
		ret.Stage8 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_PROXIMITY_STATUS_STAGE9_SHIFT))
	{
		ret.Stage9 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_PROXIMITY_STATUS_STAGE10_SHIFT))
	{
		ret.Stage10 = TRUE;
	}
	if (val & ((uint16_t)1 << AD7147_PROXIMITY_STATUS_STAGE11_SHIFT))
	{
		ret.Stage11 = TRUE;
	}
}
