#ifndef AD7147_REG_H_
#define AD7147_REG_H_

/* Comments included in this file comes from the ic datasheet
 * http://www.analog.com/media/en/technical-documentation/data-sheets/AD7147.pdf */

/* I2C address setting bits */
#define AD7147_A0	0
#define AD7147_A1	0

typedef struct{
	enum{
		AD7147_POWER_MODE_FULL_POWER 		= 0,
		AD7147_POWER_MODE_FULL_SHUTDOWN 	= 1,
		AD7147_POWER_MODE_LOW_POWER 		= 2,
		AD7147_POWER_MODE_FULL_SHUTDOWN_ 	= 3
	} AD7147_Power_Mode_e;

	enum{
		AD7147_LP_CONV_DELAY_200ms		= 0,
		AD7147_LP_CONV_DELAY_400ms		= 1,
		AD7147_LP_CONV_DELAY_600ms		= 2,
		AD7147_LP_CONV_DELAY_800ms		= 3
	} AD7147_Low_Power_Conv_delay_e;

	/* Number of stages in sequence (N + 1), max 12 */
	uint8_t AD7147_Sequence_Stage_Number;

	enum{
		AD7147_DECIMATION_256		=	0,
		AD7147_DECIMATION_128		=	1,
		AD7147_DECIMATION_64		=	2,
		AD7147_DECIMATION_64_		=	3
	} AD7147_Decimation_e;

	enum{
		AD7147_SW_RESET_OFF			= 0,
		AD7147_SW_RESET_ON			= 1
	} AD7147_Sw_Reset_e;

	enum{
		AD7147_IN_POL_ACTIVE_LOW	= 0,
		AD7147_IN_POL_ACTIVE_HIGH	= 1
	} AD7147_Int_Poling_Control_e;

	enum{
		AD7147_EXT_SOURCE_ENABLE	= 0,
		AD7147_EXT_SOURCE_DISABLE	= 1
	} AD7147_Excitation_Source_Control_e;

	enum{
		AD7147_CDC_NORM_OPERATION				= 0,
		AD7147_CDC_NORM_OPERATION_PLUS_20_PERC	= 1,
		AD7147_CDC_NORM_OPERATION_PLUS_35_PERC	= 2,
		AD7147_CDC_NORM_OPERATION_PLUS_50_PERC	= 3
	} AD7147_CDC_Bias_e;
} AD7147_PowerControl_t;
void AD7147_SetPowerControl(AD7147_PowerControl_t powerCtrl);

typedef struct{
	struct {
		uint8_t Stage0	:1;
		uint8_t Stage1	:1;
		uint8_t Stage2	:1;
		uint8_t Stage3	:1;
		uint8_t Stage4	:1;
		uint8_t Stage5	:1;
		uint8_t Stage6	:1;
		uint8_t Stage7	:1;
		uint8_t Stage8	:1;
		uint8_t Stage9	:1;
		uint8_t Stage10	:1;
		uint8_t Stage11	:1;
	} AD7147_Stage_Cal_En_s;

	enum{
		AD7147_AVG_FP_SKIP_3_SAMPLES		=	0,
		AD7147_AVG_FP_SKIP_7_SAMPLES		=	1,
		AD7147_AVG_FP_SKIP_15_SAMPLES		=	2,
		AD7147_AVG_FP_SKIP_31_SAMPLES		=	3
	} AD7147_Avg_Full_Power_Skip_Control_e;

	enum{
		AD7147_AVG_LP_NO_SKIP				=	0,
		AD7147_AVG_LP_SKIP_1_SAMPLES		=	1,
		AD7147_AVG_LP_SKIP_2_SAMPLES		=	2,
		AD7147_AVG_LP_SKIP_3_SAMPLES		=	3
	} AD7147_Avg_Low_Power_Skip_Control_e;
} AD7147_StageCal_t;
void AD7147_SetStageCal(AD7147_StageCal_t stageCal);

typedef struct{
	/*Fast filter skip control (N + 1):
		0000 = no sequence of results isskipped
		0001 = one sequence of results is skipped for every one
		allowed into fast FIFO
		0010 = two sequences of results are skipped for every
		one allowed into fast FIFO
		1011 = maximum value = 12 sequences of results are
		skipped for every one allowed into fast FIFO */
	uint8_t AD7147_Fast_Filter_Skip_Control;

	/* Calibration disable period in full power mode =
	FP_PROXIMITY_CNT × 16 × time for one conversion
	sequence in full power mode */
	uint8_t AD7147_Full_Power_Proximity_Disable_Period;

	/* Calibration disable period in low power mode =
	LP_PROXIMITY_CNT × 4 × time for one conversion
	sequence in low power mode */
	uint8_t AD7147_Low_Power_Proximity_Disable_Period;

	enum{
		AD7147_PWR_DOWN_TIMEOUT_1_25_X_FP_PROXIMITY_CNT		= 0,
		AD7147_PWR_DOWN_TIMEOUT_1_50_X_FP_PROXIMITY_CNT		= 1,
		AD7147_PWR_DOWN_TIMEOUT_1_75_X_FP_PROXIMITY_CNT		= 2,
		AD7147_PWR_DOWN_TIMEOUT_2_00_X_FP_PROXIMITY_CNT		= 3
	} AD7147_Power_Down_Timeout_e;

	enum{
		AD7147_FORCED_CAL_NORMAL_OPERATION					= 0,
		AD7147_FORCED_CAL_NORMAL_RECALIBRATE_ALL_STAGES		= 1
	} AD7147_Forced_Calibration_e;

	enum{
		AD7147_CONV_RESET_NORMAL_OPERATION					= 0,
		AD7147_CONV_RESET_NORMAL_ENABLE_STAGE0				= 1
	} AD7147_Conversion_Reset_e;

	/* Proximity recalibration level; the value is multiplied by
	16 to determine actual recalibration leve */
	uint8_t AD7147_Proximity_Recalibration_Level;

	/* Proximity detection rate; the value is multiplied by 16 to
	determine actual detection rate */
	uint8_t AD7147_Proximity_Detection_Rate;

	/* Slow filter update level */
	uint8_t AD7147_Slow_Filter_Update_Level;

	/* Full power mode proximity recalibration time control */
	uint16_t AD7147_Full_Power_Proximity_Recalibration;

	/* Low power mode proximity recalibration time control */
	uint16_t AD7147_Low_Power_Proximity_Recalibration;
} AD7147_AmbCompCtrl_t;
void AD7147_SetAmbientCompensationControl(AD7147_AmbCompCtrl_t ambCompCtrl);

typedef struct{
	struct {
		uint8_t Stage0	:1;
		uint8_t Stage1	:1;
		uint8_t Stage2	:1;
		uint8_t Stage3	:1;
		uint8_t Stage4	:1;
		uint8_t Stage5	:1;
		uint8_t Stage6	:1;
		uint8_t Stage7	:1;
		uint8_t Stage8	:1;
		uint8_t Stage9	:1;
		uint8_t Stage10	:1;
		uint8_t Stage11	:1;
	} AD7147_Stage_Low_Int_En_s;

	enum{
		ADP7147_GPIO_SETUP_DISABLE_GPIO				=	0,
		ADP7147_GPIO_SETUP_GPIO_INPUT				=	1,
		ADP7147_GPIO_SETUP_GPIO_ACTIVE_LOW_OUTPUT	=	2,
		ADP7147_GPIO_SETUP_GPIO_ACTIVE_HIGH_OUTPUT	=	3,
		ADP7147_GPIO_SETUP_GPIO_UNK					=	0xF0
	} ADP7147_Gpio_Setup_e;

	enum{
		ADP7147_GPIO_INPUT_CONF_TRIG_NEG_LEVEL		=	0,
		ADP7147_GPIO_INPUT_CONF_TRIG_POS_EDGE		=	1,
		ADP7147_GPIO_INPUT_CONF_TRIG_NEG_EDGE		=	2,
		ADP7147_GPIO_INPUT_CONF_TRIG_POS_LEVEL		=	3,
		ADP7147_GPIO_INPUT_CONF_UNK					=	0xF0
	} ADP7147_Gpio_Input_Configuration_e;
} AD7147_Stage_Low_Int_t;
void AD7147_SetStageLowInterruptEnable(AD7147_Stage_Low_Int_t stageLowIntEn);

AD7147_Stage_Low_Int_t AD7147_GetStageLowIntStatus(void);

typedef struct{
	struct {
		uint8_t Stage0	:1;
		uint8_t Stage1	:1;
		uint8_t Stage2	:1;
		uint8_t Stage3	:1;
		uint8_t Stage4	:1;
		uint8_t Stage5	:1;
		uint8_t Stage6	:1;
		uint8_t Stage7	:1;
		uint8_t Stage8	:1;
		uint8_t Stage9	:1;
		uint8_t Stage10	:1;
		uint8_t Stage11	:1;
	} AD7147_Stage_High_Int_En_s;
} AD7147_Stage_High_Int_t;
void AD7147_SetStageHighInterruptEnable(AD7147_Stage_High_Int_t stageHighIntEn);

AD7147_Stage_High_Int_t AD7147_GetStageHighIntStatus(void);

typedef struct{
	struct {
		uint8_t Stage0	:1;
		uint8_t Stage1	:1;
		uint8_t Stage2	:1;
		uint8_t Stage3	:1;
		uint8_t Stage4	:1;
		uint8_t Stage5	:1;
		uint8_t Stage6	:1;
		uint8_t Stage7	:1;
		uint8_t Stage8	:1;
		uint8_t Stage9	:1;
		uint8_t Stage10	:1;
		uint8_t Stage11	:1;
	} AD7147_Stage_Complete_Int_En_s;

	enum{
		ADP7147_GPIO_COMPLETE_INT_DISABLE	= 0,
		ADP7147_GPIO_COMPLETE_INT_ENABLE	= 1,
		ADP7147_GPIO_COMPLETE_INT_UNK		= 0xF0
	} AD7147_GPIO_Int_En;
} AD7147_Stage_Complete_Int_t;
void AD7147_SetStageCompleteInterruptEnable(AD7147_Stage_Complete_Int_t stageCompleteIntEn);

AD7147_Stage_Complete_Int_t AD7147_GetStageCompleteIntStatus(void);

typedef struct{
	uint8_t Stage0	:1;
	uint8_t Stage1	:1;
	uint8_t Stage2	:1;
	uint8_t Stage3	:1;
	uint8_t Stage4	:1;
	uint8_t Stage5	:1;
	uint8_t Stage6	:1;
	uint8_t Stage7	:1;
	uint8_t Stage8	:1;
	uint8_t Stage9	:1;
	uint8_t Stage10	:1;
	uint8_t Stage11	:1;
} AD7147_Proximity_Status_t;
AD7147_Proximity_Status_t AD7147_GetProximityStatus(void);


#endif /* AD7147_REG_H_ */
