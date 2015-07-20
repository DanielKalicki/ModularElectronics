/*
 * Si114x.c
 *
 *  Created on: 18-07-2015
 *      Author: terg
 */

#include "Si114x.h"
#include "..\ucPeriperalDrivers\i2c_connection.h"

#define SI114x_ADDR 	0x5A*2

uint8_t Si114x_detect(void){
	return i2c_Detect(I2C0, SI114x_ADDR);
}

void Si114x_init(void){
	i2c_RegisterSet(I2C0, SI114x_ADDR,0x07,0x17);

	//turn on ALS_VIS measurment
	i2c_RegisterSet(I2C0, SI114x_ADDR,0x17,0x10);	//write to PARAM reg
	i2c_RegisterSet(I2C0, SI114x_ADDR,0x18,0xA1);	//write to COMAND reg

	//ALS_VIS_ADC_GAIN is set to 1 by default
	//ALS_VIS_ADC_MISC is set to Normal Signal Range by default
}

void Si114x_forceAmbientLightMeasurment(void){
	//Force a single ALS measurment
	i2c_RegisterSet(I2C0,SI114x_ADDR,0x18,0x06);	//write to COMAND reg
}
int Si114x_readAmbientLight(uint16_t *ambientLight){
	uint8_t response=0;
	i2c_RegisterGet(I2C0, SI114x_ADDR, 0x20, &response);

	if((response&0xF0) != 0){
		//----there were an error----
		char buff[30];
		sprintf(buff,"SI114x Error=%d\n",response);
		uart_sendText(buff);

		i2c_RegisterSet(I2C0,SI114x_ADDR,0x18,0x01);//send to the sequencer RESET cmd
		return -1;
	}
	else{
		//----no error occured----
		uint8_t ALS_VIS_l=0;
		i2c_RegisterGet(I2C0, SI114x_ADDR,0x22, &ALS_VIS_l);	//TODO read block using i2c
		uint8_t ALS_VIS_h=12;
		i2c_RegisterGet(I2C0, SI114x_ADDR,0x23, &ALS_VIS_h);

		*ambientLight=(((uint16_t)ALS_VIS_h)<<8) + ALS_VIS_l;
	}
	return 0;
}
