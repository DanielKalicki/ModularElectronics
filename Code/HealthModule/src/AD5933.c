#include "AD5933.h"
#include "i2c_connection.h"
#include "uart_connection.h"

#define UART_DEBUG	1

void AD5933_init(){
	//set basic AD5933 configuration
	AD5933_state.operationMode = STANDBY;
	AD5933_state.VoutRange = VOUT_2V;
	AD5933_state.pgaGain = PGA_5X_GAIN;
	AD5933_state.clockSett = INT_CLK;
	AD5933_update_config();

	//set clock input
	uint8_t regConf2 = (AD5933_state.clockSett << 3);
	i2c_RegisterSet(I2C0,AD5933_ADDR,AD5933_REG_CONF_2,regConf2);	//the clock setting cannot be changed

	//set settling time
	AD5933_setSettlingTime(511,1);

	//set start frequency
	AD5933_setStartFrequency(30000);

	//set increments number
	AD5933_setIncNumber(1);

	//set frequency
	AD5933_setFrequencyInc(10000);
}

void AD5933_update_config(){
	uint8_t regConf1 = (AD5933_state.operationMode<<4) + (AD5933_state.VoutRange<<1) + AD5933_state.pgaGain;
	i2c_RegisterSet(I2C0,AD5933_ADDR,AD5933_REG_CONF_1,regConf1);
}

void AD5933_setStartFrequency(uint32_t freq){
	uint32_t regValue = (uint32_t)((double)freq*33.554432);

	i2c_RegisterSet(I2C0,AD5933_ADDR,AD5933_REG_START_FREQ_1,(uint8_t)(regValue>>16));
	i2c_RegisterSet(I2C0,AD5933_ADDR,AD5933_REG_START_FREQ_2,(uint8_t)(regValue>>8));
	i2c_RegisterSet(I2C0,AD5933_ADDR,AD5933_REG_START_FREQ_3,(uint8_t)(regValue));
}

void AD5933_setIncNumber(uint16_t incNumber){
	uint8_t reg1 = incNumber>>8;
	i2c_RegisterSet(I2C0,AD5933_ADDR,AD5933_REG_INC_NUM_1,(uint8_t)reg1);
	i2c_RegisterSet(I2C0,AD5933_ADDR,AD5933_REG_INC_NUM_2,(uint8_t)incNumber);
}

void AD5933_setFrequencyInc(uint32_t freq){
	uint32_t regValue = (uint32_t)((double)freq*33.554432);
	i2c_RegisterSet(I2C0,AD5933_ADDR,AD5933_REG_FREQ_INC_1,(uint8_t)(regValue>>16));
	i2c_RegisterSet(I2C0,AD5933_ADDR,AD5933_REG_FREQ_INC_2,(uint8_t)(regValue>>8));
	i2c_RegisterSet(I2C0,AD5933_ADDR,AD5933_REG_FREQ_INC_3,(uint8_t)(regValue));
}

void AD5933_setSettlingTime(uint16_t cycles, uint8_t multiplier){
	if(multiplier==0 || multiplier==3) multiplier=1;		//protection against wrong settings
	else if(multiplier>4) multiplier=4;						//protection against wrong settings

	uint8_t reg1 = (cycles>>8) + (((multiplier&0x03)-1)<<1);
	i2c_RegisterSet(I2C0,AD5933_ADDR,AD5933_REG_SET_TIME_NUM_1,(uint8_t)reg1);
	i2c_RegisterSet(I2C0,AD5933_ADDR,AD5933_REG_SET_TIME_NUM_2,(uint8_t)cycles);
}

void AD5933_startMeasurment(){
	AD5933_state.operationMode = START_FREQ_SWEEP;
	AD5933_update_config(AD5933_state);
}

void AD5933_initFreq(){
	AD5933_state.operationMode = INIT_START_FREQ;
	AD5933_update_config(AD5933_state);
}

void AD5933_standby(){
	AD5933_state.operationMode = STANDBY;
	AD5933_update_config(AD5933_state);
}

uint16_t AD5933_getRealValue(){
	uint8_t reg[2];
	i2c_RegisterGet(I2C0,AD5933_ADDR,AD5933_REG_REAL_DATA_1,&reg[0]);
	i2c_RegisterGet(I2C0,AD5933_ADDR,AD5933_REG_REAL_DATA_2,&reg[1]);
	return (((uint16_t)(reg[0])<<8) + (uint16_t)reg[1]);
}

uint16_t AD5933_getImgValue(){
	uint8_t reg[2];
	i2c_RegisterGet(I2C0,AD5933_ADDR,AD5933_REG_IMG_DATA_1,&reg[0]);
	i2c_RegisterGet(I2C0,AD5933_ADDR,AD5933_REG_IMG_DATA_2,&reg[1]);
	return (((uint16_t)(reg[0])<<8) + (uint16_t)reg[1]);
}
