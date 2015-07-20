#ifndef __HMC5883L_H
#define __HMC5883L_H

#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

enum HMC5883L_numberOfSamplesAverage{
	NUM_OF_SAMPLES_1=0,
	NUM_OF_SAMPLES_2,
	NUM_OF_SAMPLES_4,
	NUM_OF_SAMPLES_8
};

enum HMC5883L_typicalDataOutputRate{
	OUTP_RATE_0_75_HZ=0,
	OUTP_RATE_1_5_HZ,
	OUTP_RATE_3_HZ,
	OUTP_RATE_7_5_HZ,
	OUTP_RATE_15_HZ,
	OUTP_RATE_30_HZ,
	OUTP_RATE_75_HZ,
	OUTP_RATE_RESERVED
};

enum HMC5883L_measurementMode{
	MEASURMENT_MODE_NORMAL=0,
	MEASURMENT_MODE_POS_BIAS,
	MEASURMENT_MODE_NEG_BIAS,
	MEASURMENT_MODE_RESERVED
};

enum HMC5883L_gainConfiguration{
	GAIN_CONF_073mG_per_LSB=0,
	GAIN_CONF_092mG_per_LSB,
	GAIN_CONF_122mG_per_LSB,
	GAIN_CONF_152mG_per_LSB,
	GAIN_CONF_227mG_per_LSB,
	GAIN_CONF_256mG_per_LSB,
	GAIN_CONF_303mG_per_LSB,
	GAIN_CONF_435mG_per_LSB
};

enum HMC5883L_highSpeedI2C{
	HIGH_SPEED_I2C_OFF=0,
	HIGH_SPEED_I2C_ON
};

enum HMC5883L_operatingMode{
	OPERATION_MODE_CONTONOUS=0,
	OPERATION_MODE_SINGLE,
	OPERATION_MODE_IDLE
};

uint8_t HMC5883L_detect(void);
void HMC5883L_init(void);
void HMC5883L_getCompassData(int16_t* X, int16_t* Y, int16_t* Z);
uint32_t HMC5883L_getFieldStrengthLSB(void);		//returns the result in 0.01mG

#ifdef __cplusplus
}
#endif

#endif
