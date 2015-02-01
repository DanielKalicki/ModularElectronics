#include "em_device.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_i2c.h"
#include "em_emu.h"
#include "em_rtc.h"
#include "em_cmu.h"

#include "uart_connection.h"
#include "i2c_connection.h"
#include "RTC_.h"
#include "HM11.h"

//--------Sensor module----------
enum registerMap{

	REG_SENSORS				=1,
	REG_MPU6050_CONFIG		=2,				//BIT0 ---- 0 - normal mode,   1 - low-power accel-only mode
	//TODO check ^ number

	REG_SI7013_HUM_HIGH		=10,
	REG_SI7013_HUM_LOW		=11,

	REG_SI7013_TEMP_HIGH	=12,
	REG_SI7013_TEMP_LOW		=13,

	REG_SI1142_ALS_HIGH		=14,
	REG_SI1142_ALS_LOW		=15,

	REG_AS3935_LIGHT_DET_NUM=16,

	REG_BMP085_TEMP_HIGH	=17,
	REG_BMP085_TEMP_LOW		=18,

	REG_BMP085_PRESS_XHIGH	=19,
	REG_BMP085_PRESS_HIGH	=20,
	REG_BMP085_PRESS_LOW	=21,

	REG_HMC5883L_X_HIGH		=22,
	REG_HMC5883L_X_LOW		=23,
	REG_HMC5883L_Y_HIGH		=24,
	REG_HMC5883L_Y_LOW		=25,
	REG_HMC5883L_Z_HIGH		=26,
	REG_HMC5883L_Z_LOW		=27,

	REG_MPU6050_X_ACCEL_HIGH=28,
	REG_MPU6050_X_ACCEL_LOW	=29,
	REG_MPU6050_Y_ACCEL_HIGH=30,
	REG_MPU6050_Y_ACCEL_LOW	=31,
	REG_MPU6050_Z_ACCEL_HIGH=32,
	REG_MPU6050_Z_ACCEL_LOW	=33,

	REG_MPU6050_X_GYRO_HIGH	=34,
	REG_MPU6050_X_GYRO_LOW	=35,
	REG_MPU6050_Y_GYRO_HIGH	=36,
	REG_MPU6050_Y_GYRO_LOW	=37,
	REG_MPU6050_Z_GYRO_HIGH	=38,
	REG_MPU6050_Z_GYRO_LOW	=39,

	REG_MPU6050_X_PEDO_COUNT_1	=40,
	REG_MPU6050_X_PEDO_COUNT_2	=41,
	REG_MPU6050_X_PEDO_COUNT_3	=42,
	REG_MPU6050_X_PEDO_COUNT_4	=43,
	REG_MPU6050_X_PEDO_COUNT_5	=44,
	REG_MPU6050_X_PEDO_COUNT_6	=45,
	REG_MPU6050_X_PEDO_COUNT_7	=46,
	REG_MPU6050_X_PEDO_COUNT_8	=47,

	REG_MPU6050_X_PEDO_TIME_1	=48,
	REG_MPU6050_X_PEDO_TIME_2	=49,
	REG_MPU6050_X_PEDO_TIME_3	=50,
	REG_MPU6050_X_PEDO_TIME_4	=51,
	REG_MPU6050_X_PEDO_TIME_5	=52,
	REG_MPU6050_X_PEDO_TIME_6	=53,
	REG_MPU6050_X_PEDO_TIME_7	=54,
	REG_MPU6050_X_PEDO_TIME_8	=55,

	REG_MPU6050_X_TAP_EVENT_1	=56,
	REG_MPU6050_X_TAP_EVENT_2	=57,
	REG_MPU6050_X_TAP_EVENT_3	=58,
	REG_MPU6050_X_TAP_EVENT_4	=59,
	REG_MPU6050_X_TAP_EVENT_5	=60,
	REG_MPU6050_X_TAP_EVENT_6	=61,
	REG_MPU6050_X_TAP_EVENT_7	=62,
	REG_MPU6050_X_TAP_EVENT_8	=63,
	REG_MPU6050_X_TAP_EVENT_9	=64,
	REG_MPU6050_X_TAP_EVENT_10	=65,

	REG_AS3935_lIGH_DIST_1	=66,
	REG_AS3935_lIGH_DIST_2,
	REG_AS3935_lIGH_DIST_3,
	REG_AS3935_lIGH_DIST_4,
	REG_AS3935_lIGH_DIST_5,
	REG_AS3935_lIGH_DIST_6,
	REG_AS3935_lIGH_DIST_7,
	REG_AS3935_lIGH_DIST_8,
	REG_AS3935_lIGH_DIST_9,
	REG_AS3935_lIGH_DIST_10,
	//ADD VALUES TO _20
};

//-------------INIT--------------
void initOscillators(void){
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);     // enable GPIO peripheral clock
}

void testWait(){
	for (int i=0;i<10;i++){
		unsigned int counter=0;
		while (counter<1000000) {counter++;__asm("NOP");};
	}
}

//-------------RTC--------------
void RTC_IRQHandler(void){
	static uint16_t counter=0;

	counter++;
	if(counter>32000) counter=0;

	char buff[40];

	sprintf(buff,"");

	if(counter%5==0){
		uint8_t ALS_H=0;	uint8_t ALS_L=0;
		i2c_RegisterGet(I2C0,0x10,REG_SI1142_ALS_HIGH,&ALS_H);	i2c_RegisterGet(I2C0,0x10,REG_SI1142_ALS_LOW ,&ALS_L);
		sprintf(buff,"L: %d.%d [%d]",ALS_H,ALS_L,counter);
	}
	else if(counter%5==1){
		uint8_t BMP_temp_H=0;	uint8_t BMP_temp_L=0;
		i2c_RegisterGet(I2C0,0x10,REG_BMP085_TEMP_HIGH,&BMP_temp_H);	i2c_RegisterGet(I2C0,0x10,REG_BMP085_TEMP_LOW ,&BMP_temp_L);
		sprintf(buff,"T:%d.%d [%d]",BMP_temp_H,BMP_temp_L,counter);
	}
	else if(counter%5==2){
		uint8_t BMP_press_XH=0;	uint8_t BMP_press_H=0; uint8_t BMP_press_L=0;
		i2c_RegisterGet(I2C0,0x10,REG_BMP085_PRESS_XHIGH,&BMP_press_XH);	i2c_RegisterGet(I2C0,0x10,REG_BMP085_PRESS_HIGH ,&BMP_press_H);	i2c_RegisterGet(I2C0,0x10,REG_BMP085_PRESS_LOW ,&BMP_press_L);
		sprintf(buff,"P:%d%d%d [%d]",BMP_press_XH,BMP_press_H,BMP_press_L,counter);
	}
	else if(counter%5==3){
		uint8_t data_length=0;
		i2c_RegisterGet(I2C0,0x10,0x00,&data_length);

		sprintf(buff,"L:%d [%d]",data_length,counter);

		data_length=87; //test

		uint8_t reg_vals[200];
		i2c_Register_Read_Block(I2C0,0x10,0x00,data_length,reg_vals);

		uart_sendText("|||");

		for(int i=0;i<data_length;i++)
			uart_sendChar(reg_vals[i]);
		uart_sendChar('\n');
	}

	//receive ack

	uart_sendText(buff);

	/* Clear interrupt source */
	RTC_IntClear(RTC_IFC_COMP0);
}

//-------------MAIN-------------
int main(void)
{
  CHIP_Init();
  initOscillators();

  initI2C();
  initUART_baud(115200);

  //initUART_9600();
  //uart_sendText("AT+BAUD0");
  //testWait();
  //uart_sendText("AT+RESET");
  //testWait(); testWait(); testWait(); testWait(); testWait();
  //initUART_baud(BAUD_19200);

  uint8_t initHM11_fail=1;
  while(initHM11_fail){
	  initHM11_fail=initHM11();
	  testWait();
  }

  /* Setting up rtc */
  setupRtc();

  while(1){
	  EMU_EnterEM1();

  }
}
