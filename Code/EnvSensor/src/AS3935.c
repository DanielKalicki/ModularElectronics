#include "AS3935.h"
#include "ucPeriperalDrivers\i2c_connection.h"
#include "em_gpio.h"

void AS3935_init(void){
	AS3935_set_GainBoost(1);
	AS3935_set_Watchdog_And_NoiseFloor(0x02,0x07);
	//AS3935_set_MinLightingNumber_And_SpikeRejection(0x00,0x02);
	AS3935_set_TuningCaps(0x07);

	/* By setting the mode to gpioModeInput its value can be read */
	GPIO_PinModeSet(gpioPortC, 15, gpioModeInput, 1);
	/* Enable GPIO_ODD interrupt in NVIC */
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
	 /* Configure interrupt on falling edge for pins C15 */
	GPIO_IntConfig(gpioPortC, 15, true, false, true);
}

void AS3935_powerDown(){
	i2c_RegisterSet(I2C0,AS3935_ADDR,0x00,0x01);
}

void AS3935_powerUp(){
	i2c_RegisterSet(I2C0,AS3935_ADDR,0x00,0x24);
}

void AS3935_set_GainBoost(uint8_t indoor){
	if (indoor)
		i2c_RegisterSet(I2C0,AS3935_ADDR,0x00,0x24);
	else
		i2c_RegisterSet(I2C0,AS3935_ADDR,0x00,0x1C); //set to outdoor
}

void AS3935_set_Watchdog_And_NoiseFloor(uint8_t watchdog, uint8_t noiseFloorLevel){
	//defualt 010 0010 -> 0x22
	uint8_t regValue = ((noiseFloorLevel&0x07)<<4)+(watchdog&0x0F);
	i2c_RegisterSet(I2C0,AS3935_ADDR,0x01,regValue);
}

void AS3935_set_MinLightingNumber_And_SpikeRejection(uint8_t minLightNumber, uint8_t spikeRejection){
	//defualt 1(reserved) 1(clear stats) 00 0100
	uint8_t regValue = 0xC0+((minLightNumber&0x03)<<4)+(spikeRejection&0x0F);
	i2c_RegisterSet(I2C0,AS3935_ADDR,0x02,regValue);
}
void AS3935_set_TuningCaps(uint8_t tunCap){
	//defualt 0
	uint8_t regValue = (tunCap&0x0F);
	i2c_RegisterSet(I2C0,AS3935_ADDR,0x08,regValue);
}

uint8_t AS3935_read_Interrupt(){
	uint8_t regValue=0;
	i2c_RegisterGet(I2C0,AS3935_ADDR,0x03,&regValue);
	return regValue;
}

uint8_t AS3935_read_Distance(){
	uint8_t regValue=0;
	i2c_RegisterGet(I2C0,AS3935_ADDR,0x07,&regValue);
	return regValue&0x1F;
}
