#include "BMP180.h"
#include "..\ucPeripheralDrivers\i2c_connection.h"

#define BMP180_ADDR 	0x77*2

void BMP180_init(void){

}

uint8_t BMP180_detect(void){
	return i2c_Detect(I2C0, BMP180_ADDR);
}


//TODO read this on start of the device
short AC1=0x1C45;	short AC2=0xFC32;	short AC3=0xC795;
unsigned short AC4=0x81BF;				unsigned short AC5=0x606F;
unsigned short AC6=0x5CAF;				short B1=0x157A;
short B2=0x0031;	short MB=0x8000;	short MC=0xD4BD;	short MD=0x0980;
long b5;			const unsigned char OSS = 0;

short bmp180_GetTemperature(unsigned int ut)
{
	long x1, x2;

	x1 = (((long)ut - (long)AC6)*(long)AC5) >> 15;
	x2 = ((long)MC << 11)/(x1 + MD);
	b5 = x1 + x2;

	return ((b5 + 8)>>4);
}
long bmp180_GetPressure(unsigned long up)
{
	long x1, x2, x3, b3, b6, p;
	unsigned long b4, b7;

	b6 = b5 - 4000;
	// Calculate B3
	x1 = (B2 * (b6 * b6)>>12)>>11;
	x2 = (AC2 * b6)>>11;
	x3 = x1 + x2;
	b3 = (((((long)AC1)*4 + x3)<<OSS) + 2)>>2;

	// Calculate B4
	x1 = (AC3 * b6)>>13;
	x2 = (B1 * ((b6 * b6)>>12))>>16;
	x3 = ((x1 + x2) + 2)>>2;
	b4 = (AC4 * (unsigned long)(x3 + 32768))>>15;

	b7 = ((unsigned long)(up - b3) * (50000>>OSS));
	if (b7 < 0x80000000)
	p = (b7<<1)/b4;
	else
	p = (b7/b4)<<1;

	x1 = (p>>8) * (p>>8);
	x1 = (x1 * 3038)>>16;
	x2 = (-7357 * p)>>16;
	p += (x1 + x2 + 3791)>>4;

	return p;
}
void BMP180_forceTemperatureMesurement(void){
	i2c_RegisterSet(I2C0,BMP180_ADDR,0xF4,0x2E);
}
void BMP180_readTemperature(short *temp){
	uint8_t ut_H = 0;
	i2c_RegisterGet(I2C0,BMP180_ADDR,0xF6,&ut_H);
	uint8_t ut_L = 0;
	i2c_RegisterGet(I2C0,BMP180_ADDR,0xF7,&ut_L);

	unsigned int ut=(((unsigned int)ut_H)<<8)+ut_L;
	*temp=bmp180_GetTemperature(ut);
}
void BMP180_forcePressureMeasurment(void){
	i2c_RegisterSet(I2C0,BMP180_ADDR,0xF4,0x34);	//do the simples pressure measurment
}
void BMP180_readPressure(uint32_t *pressure){
	uint8_t press_H=0;
	i2c_RegisterGet(I2C0, BMP180_ADDR,0xF6, &press_H);	//TODO read block using i2c
	uint8_t press_L=0;
	i2c_RegisterGet(I2C0, BMP180_ADDR,0xF7, &press_L);

	unsigned long up= (((unsigned long)press_H)<<8)+press_L;
	*pressure = bmp180_GetPressure(up);
}
