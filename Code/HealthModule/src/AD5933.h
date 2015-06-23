#ifndef AD5933_H_
#define AD5933_H_

#include <stdint.h>

#define AD5933_ADDR		0x1A

enum AD5933_REG{
	AD5933_REG_CONF_1			= 0x80,
	AD5933_REG_CONF_2			= 0x81,

	AD5933_REG_START_FREQ_1 	= 0x82,
	AD5933_REG_START_FREQ_2 	= 0x83,
	AD5933_REG_START_FREQ_3 	= 0x84,

	AD5933_REG_FREQ_INC_1 		= 0x85,
	AD5933_REG_FREQ_INC_2 		= 0x86,
	AD5933_REG_FREQ_INC_3 		= 0x87,

	AD5933_REG_INC_NUM_1	 	= 0x88,
	AD5933_REG_INC_NUM_2	 	= 0x89,

	AD5933_REG_SET_TIME_NUM_1	 = 0x8A,
	AD5933_REG_SET_TIME_NUM_2	 = 0x8B,

	AD5933_REG_STATUS_1		 	= 0x8F,

	AD5933_REG_TEMP_1	 		= 0x92,
	AD5933_REG_TEMP_2	 		= 0x93,

	AD5933_REG_REAL_DATA_1	 	= 0x94,
	AD5933_REG_REAL_DATA_2	 	= 0x95,

	AD5933_REG_IMG_DATA_1	 	= 0x96,
	AD5933_REG_IMG_DATA_2	 	= 0x97
};

//---------------------CONFIG REG-----------------
enum AD5933_CONFIG_OPERATION{
	INIT_START_FREQ  	= 1,
	START_FREQ_SWEEP 	= 2,
	INC_FREQ			= 3,
	REPEAT_FREQ			= 4,
	MEAS_TEMP			= 9,
	POWER_DOWN			= 10,
	STANDBY				= 11
};
enum AD5933_CONFIG_VOUT_RANGE{
	VOUT_2V		= 0,
	VOUT_200mV	= 1,
	VOUT_400mV	= 2,
	VOUT_1V		= 3
};
enum AD5933_CONFIG_PGA_GAIN{
	PGA_5X_GAIN	= 0,
	PGA_1X_GAIN	= 1
};
enum AD5933_CONFIG_CLOCK{
	EXT_CLK		= 1,
	INT_CLK		= 0
};

//------global variable containing AD5933 state----------
struct AD5933_config_str{
	enum AD5933_CONFIG_OPERATION operationMode;
	enum AD5933_CONFIG_VOUT_RANGE VoutRange;
	enum AD5933_CONFIG_PGA_GAIN pgaGain;
	enum AD5933_CONFIG_CLOCK clockSett;
} AD5933_state;

//------------Set functions----------------
void AD5933_init();
void AD5933_update_config();
void AD5933_setStartFrequency(uint32_t freq);
void AD5933_setSettlingTime(uint16_t cycles, uint8_t multiplier);
void AD5933_setIncNumber(uint16_t incNumber);
void AD5933_setFrequencyInc(uint32_t freq);

//-----------Measurment functions----------
void AD5933_initFreq();
void AD5933_startMeasurment();
void AD5933_standby();

//------------Get functions---------------
uint16_t AD5933_getRealValue();
uint16_t AD5933_getImgValue();

#endif /* AD5933_H_ */
