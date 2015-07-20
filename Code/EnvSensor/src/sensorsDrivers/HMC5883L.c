#include "HMC5883L.h"
#include "..\ucPeriperalDrivers\i2c_connection.h"

#define HMC5883L_ADDR 	0x1E*2

//--------------Module configuration------------

			//reg 0x00
enum HMC5883L_numberOfSamplesAverage numbOfSamples 	= 	NUM_OF_SAMPLES_1;
enum HMC5883L_typicalDataOutputRate  outpRate		=	OUTP_RATE_15_HZ;
enum HMC5883L_measurementMode 	     measurementMode=	MEASURMENT_MODE_NORMAL;

			//reg 0x01
enum HMC5883L_gainConfiguration	     gainConf		=	GAIN_CONF_152mG_per_LSB;

			//reg 0x02
enum HMC5883L_highSpeedI2C		     i2cSetting		=	HIGH_SPEED_I2C_OFF;
enum	HMC5883L_operatingMode		 operatingMode  =	OPERATION_MODE_CONTONOUS;

uint8_t HMC5883L_detect(void){
	return i2c_Detect(I2C0, HMC5883L_ADDR);
}

void HMC5883L_init(void){
	uint8_t regVal =  (numbOfSamples 	& 0x03) <<5;
			regVal += (outpRate  		& 0x07) <<2;
			regVal += (measurementMode  & 0x03) <<0;
	i2c_RegisterSet(I2C0,HMC5883L_ADDR,0x00,regVal);

			regVal =  (gainConf      	& 0x07) <<5;
	i2c_RegisterSet(I2C0,HMC5883L_ADDR,0x01,regVal);

			regVal =  (i2cSetting 		& 0x01) <<7;
			regVal += (operatingMode  	& 0x03) <<0;
	i2c_RegisterSet(I2C0,HMC5883L_ADDR,0x02,regVal);
}

void HMC5883L_getCompassData(int16_t* X, int16_t* Y, int16_t* Z){
	uint8_t data[6];
	i2c_Register_Read_Block(I2C0,HMC5883L_ADDR,0x03,6,data);
	*X=(((uint16_t)data[0])<<8)+data[1];
	*Y=(((uint16_t)data[2])<<8)+data[3];
	*Z=(((uint16_t)data[4])<<8)+data[5];
}

uint32_t HMC5883L_getFieldStrengthLSB(void){

	//												HMC5883L_gainConfiguration
	//
	//000 - 0.73mG/LSb  +-0.88Ga		001 - 0.92mG/LSb  +-1.3Ga		010 - 1.22mG/LSb  +-1.9Ga		011 - 1.52mG/LSb  +-2.5Ga
	//100 - 2.27mG/LSb  +-4.0Ga			101 - 2.56mG/LSb  +-4.7Ga		110 - 3.03mG/LSb  +-5.6Ga		111 - 4.35mG/LSb  +-8.1Ga

	switch (gainConf){
		case GAIN_CONF_073mG_per_LSB:
			return 73;
		case GAIN_CONF_092mG_per_LSB:
			return 92;
		case GAIN_CONF_122mG_per_LSB:
			return 122;
		case GAIN_CONF_152mG_per_LSB:
			return 152;
		case GAIN_CONF_227mG_per_LSB:
			return 227;
		case GAIN_CONF_256mG_per_LSB:
			return 256;
		case GAIN_CONF_303mG_per_LSB:
			return 303;
		case GAIN_CONF_435mG_per_LSB:
			return 111;
		default:
			return 0;
	}
}
