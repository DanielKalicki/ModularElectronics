#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_rtc.h"
#include "em_i2c.h"
#include "em_emu.h"
#include <stdio.h>
#include "i2c_connection.h"
#include "RTC_.h"
#include "uart_connection.h"

#include "HMC5883L.h"
#include "MPU6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

#define sensors i2c_registers[5]		//5 - REG_SENSORS

#define SI7013_SENS		0x01
#define SI1142_SENS		0x02
#define AS3935_SENS		0x04
#define BMP085_SENS		0x08
#define HMC5883L_SENS	0x10
#define MPU6050_SENS	0x20

/********** i2c slave register map **********
 * [0:9] - configuration registers
 * [10:11] - Si7013 humidity   -> 	e.g. hum=56.25%  [10]=56 [11]=25
 * [12:13] - Si7013 temperature ->	e.g. temp=25.23*C [12]=25 [13]=23
 * [14:15] - Si1142 ambient light ->  [14] -> ambient light High byte  [15] -> ambient light Low byte
 * [16]    - AS3935 number of lighting detection
 * [17:18] - BMP085 temperature if it is measured, else it should be zero ->	e.g. temp=25.23*C [17]=25 [18]=23
 * [19:21] - BMP085 pressure  -> e.g. pressure=101399.8 [19]=101 [20]=39 [21]=98
 * [22:27] - HMC5883L [22:23] - X data, [24:25] - Y data [26:27] - Z data
 * [28:33] - MPU6050 accel  [28:29] - X accel data [30:31] - Y accel data [32:33] - Z accel data
 * [34:39] - MPU6050 gyro   [34:35] - X gyro data  [36:37] - Y gyro data  [38:39] - Z gyro data
 * [40:55] - MPU pedometer  [40:47] - number of steps in binary format
 *                          [48:55] - number of walking time in binary format in miliseconds
 * [56:65] - MPU dmp events [55:54] - detected tap events
 * [66:85]  AS3935 every byte contains estimate distance from lighting for every lighting detection
 * */
enum registerMap{

	REG_DATA_LENGTH			=0,

	REG_MODULE_ID_1			=1,
	REG_MODULE_ID_2			=2,
	REG_MODULE_ID_3			=3,

	REG_SENSORS				=5,
	REG_MPU6050_CONFIG		=REG_MPU6050_CONFIG_NUMBER,				//BIT0 ---- 0 - normal mode,   1 - low-power accel-only mode

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

	CMU_ClockEnable(cmuClock_GPIO, true);                   // enable GPIO peripheral clock
	CMU_ClockEnable(cmuClock_I2C0,true);
}
void initGPIO(void){
}


//------------I2C----------------
#define SI7013_ADDR 	0x40*2
#define AS3935_ADDR 	0x03*2
#define BMP085_ADDR 	0x77*2
#define SI1142_ADDR 	0x5A*2

//------------TEST---------------
void clockTest() {
	long int i=0;
	for(;i<120001L;++i) {
	  if(i==120000L)
		  GPIO_PortOutSet(gpioPortF, 0x4);
	  if(i==60000L)
		  GPIO_PortOutClear(gpioPortF, 0x4);
	}
}
void clockTest_short() {
	long int i=0;
	for(;i<121L;++i) {
	  if(i==120L)
		  GPIO_PortOutSet(gpioPortF, 0x4);
	  if(i==60L)
		  GPIO_PortOutClear(gpioPortF, 0x4);
	}
}

//------------SENSORS------------
void detectSensors(){
	uart_sendText("\nDETECTING CONNECTED SENSORS\n");
	sensors = 0;

	//-----------SI7013---------
	if (i2c_Detect(I2C0, SI7013_ADDR)){
		uart_sendText("\t\tSi7013 detected\n");
		sensors |= 0x01;
	}
	else
		uart_sendText("---\t\tSi7013 NOT DETECTED\t\t---\n");

	//-----------SI1142---------
	if (i2c_Detect(I2C0, SI1142_ADDR)){
		uart_sendText("\t\tSi1142 detected\n");
		sensors |= 0x02;
	}
	else
		uart_sendText("---\t\tSi1142 NOT DETECTED\t\t---\n");

	//-----------AS3935---------
	if (i2c_Detect(I2C0, AS3935_ADDR)){
		uart_sendText("\t\tAS3935 detected\n");
		sensors |= 0x04;
	}
	else
		uart_sendText("---\t!!!\tAS3935 NOT DETECTED\t!!!\t---\n");

	//-----------BMP085---------
	if (i2c_Detect(I2C0, BMP085_ADDR)) {
		uart_sendText("\t\tBMP085 detected\n");
		sensors |= 0x08;
	}
	else
		uart_sendText("---\t\tBMP085 NOT DETECTED\t\t---\n");

	//-----------HMC5883L---------
	if (i2c_Detect(I2C0, HMC5883L_ADDR)) {
		uart_sendText("\t\tHMC5883L detected\n");
		sensors |= 0x10;
	}
	else
		uart_sendText("---\t\tHMC5883L NOT DETECTED\t\t---\n");

	//-----------MPU6050---------
	if (i2c_Detect(I2C0, MPU6050_ADDR)) {
		uart_sendText("\t\tMPU6050 detected\n");
		sensors |= 0x20;
	}
	else
		uart_sendText("---\t\tMPU6050 NOT DETECTED\t\t---\n");
}

void initSensors(){
	uart_sendText("\nINITIALIZATION: ");

	if(sensors & SI7013_SENS){
		//initialization not required
		uart_sendText(" SI7013");
	}
	if(sensors & SI1142_SENS){
		//turn on ALS_VIS measurment
		i2c_RegisterSet(I2C0, SI1142_ADDR,0x17,0x10);	//write to PARAM reg
		i2c_RegisterSet(I2C0, SI1142_ADDR,0x18,0xA1);	//write to COMAND reg

		//ALS_VIS_ADC_GAIN is set to 1 by default
		//ALS_VIS_ADC_MISC is set to Normal Signal Range by default
		uart_sendText(" SI1142");
	}
	if(sensors & AS3935_SENS){
		//initialization need to be implemented
		uart_sendText(" AS3935");
	}
	if(sensors & BMP085_SENS){
		//initialization not required
		uart_sendText(" BMP085");
	}
	if(sensors & HMC5883L_SENS){
		HMC5883L_init();
		uart_sendText(" HMC5883L");
	}
	if(sensors & MPU6050_SENS){
		MPU6050_init();

		uart_sendText(" MPU6050");
	}
	uart_sendChar('\n');
}

//-----------DEVICES------------
static int Si7013_ReadTemperature(I2C_TypeDef *i2c, uint8_t addr, uint32_t *data,
                          	  	  uint8_t command){
	I2C_TransferSeq_TypeDef    seq;
	uint8_t                    i2c_read_data[2];
	uint8_t                    i2c_write_data[1];

	seq.addr  = addr;
	seq.flags = I2C_FLAG_WRITE_READ;
	/* Select command to issue */
	i2c_write_data[0] = command;
	seq.buf[0].data   = i2c_write_data;
	seq.buf[0].len    = 1;
	/* Select location/length of data to be read */
	seq.buf[1].data = i2c_read_data;
	seq.buf[1].len  = 2;

	I2C_Status = I2C_TransferInit(i2c, &seq);
	uint32_t timeout = I2CDRV_TRANSFER_TIMEOUT;
	while (I2C_Status == i2cTransferInProgress && timeout--) {
		I2C_Status = I2C_Transfer(I2C0);
	}
	if(timeout==(uint32_t)(-1)){
		uart_sendText("\nERROR: I2C_get_timeout\n");
	}

	if (I2C_Status != i2cTransferDone)
	{
	  *data = 0;
	  return((int) I2C_Status);
	}

	*data = ((uint32_t) i2c_read_data[0] << 8) + (i2c_read_data[1] & 0xfc);

	return((int) 2);
}

void forceRhMeasurment(){
	I2C_TransferSeq_TypeDef    seq;
	uint8_t                    i2c_write_data[1];

	seq.addr  = SI7013_ADDR;
	seq.flags = I2C_FLAG_WRITE;
	/* Select command to issue */
	i2c_write_data[0] = 0xF5;			//Measure Relative Humidity, No Hold Master Mode
	seq.buf[0].data   = i2c_write_data;
	seq.buf[0].len    = 1;

	I2C_Status = I2C_TransferInit(I2C0, &seq);
	uint32_t timeout = I2CDRV_TRANSFER_TIMEOUT;
	while (I2C_Status == i2cTransferInProgress && timeout--) {
		I2C_Status = I2C_Transfer(I2C0);
	}
	if(timeout==(uint32_t)(-1)){
		uart_sendText("\nERROR: I2C_get_timeout\n");
	}
}
int readHumidityAndTemperature(uint32_t *rhData, int32_t *tData){
	 I2C_TransferSeq_TypeDef seq;
	 uint8_t data[2];

	 seq.addr = SI7013_ADDR;
	 seq.flags = I2C_FLAG_READ;

	 seq.buf[0].data = data;
	 seq.buf[0].len  = 2;

	 /* Do a polled transfer */
	 I2C_Status = I2C_TransferInit(I2C0, &seq);
	 uint32_t timeout = I2CDRV_TRANSFER_TIMEOUT;
	 while (I2C_Status == i2cTransferInProgress && timeout--)
	 {
	    /* Enter EM1 while waiting for I2C interrupt */
	    //EMU_EnterEM1();
	    /* Could do a timeout function here. */
		I2C_Status = I2C_Transfer(I2C0);
	 }
	 if(timeout==(uint32_t)(-1)){
		 uart_sendText("\nERROR: I2C_get_timeout\n");
	 }

	 if (I2C_Status != i2cTransferDone)
	 {
	    return((int)I2C_Status);
	 }

	 *rhData = ((((((uint16_t)data[0])<<8)+data[1]) * 15625L) >> 13) - 6000;

	 Si7013_ReadTemperature(I2C0,SI7013_ADDR,tData,0xE0);
	 *tData = (((*tData) * 21965L) >> 13) - 46850; /* convert to milli-degC */

	 return ((int) 1);
}

void forceAmbientLightMeasurment(){
	//Force a single ALS measurment
	i2c_RegisterSet(I2C0,SI1142_ADDR,0x18,0x06);	//write to COMAND reg
}
int readAmbientLight(uint16_t *ambientLight){
	uint8_t response=0;
	i2c_RegisterGet(I2C0, SI1142_ADDR, 0x20, &response);

	if((response&0xF0) != 0){
		//----there were an error----
		char buff[30];
		sprintf(buff,"SI1142 Error=%d\n",response);
		uart_sendText(buff);

		i2c_RegisterSet(I2C0,SI1142_ADDR,0x18,0x01);//send to the sequencer RESET cmd
		return -1;
	}
	else{
		//----no error occured----
		uint8_t ALS_VIS_l=0;
		i2c_RegisterGet(I2C0, SI1142_ADDR,0x22, &ALS_VIS_l);	//TODO read block using i2c
		uint8_t ALS_VIS_h=12;
		i2c_RegisterGet(I2C0, SI1142_ADDR,0x23, &ALS_VIS_h);

		*ambientLight=(((uint16_t)ALS_VIS_h)<<8) + ALS_VIS_l;
	}
	return 0;
}

//TODO read this on start of the device
short AC1=0x1C45;	short AC2=0xFC32;	short AC3=0xC795;
unsigned short AC4=0x81BF;				unsigned short AC5=0x606F;
unsigned short AC6=0x5CAF;				short B1=0x157A;
short B2=0x0031;	short MB=0x8000;	short MC=0xD4BD;	short MD=0x0980;
long b5;			const unsigned char OSS = 0;

short bmp085GetTemperature(unsigned int ut)
{
	long x1, x2;

	x1 = (((long)ut - (long)AC6)*(long)AC5) >> 15;
	x2 = ((long)MC << 11)/(x1 + MD);
	b5 = x1 + x2;

	return ((b5 + 8)>>4);
}
long bmp085GetPressure(unsigned long up)
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
void forceTemperatureMesurementBMP085(){
	i2c_RegisterSet(I2C0,BMP085_ADDR,0xF4,0x2E);
}
void readTemperatureBMP085(short *temp){
	uint8_t ut_H = 0;
	i2c_RegisterGet(I2C0,BMP085_ADDR,0xF6,&ut_H);
	uint8_t ut_L = 0;
	i2c_RegisterGet(I2C0,BMP085_ADDR,0xF7,&ut_L);

	unsigned int ut=(((unsigned int)ut_H)<<8)+ut_L;
	*temp=bmp085GetTemperature(ut);
}
void forcePressureMeasurmentBMP085(){
	i2c_RegisterSet(I2C0,BMP085_ADDR,0xF4,0x34);	//do the simples pressure measurment
}
void readPressureBMP085(uint32_t *pressure){
	uint8_t press_H=0;
	i2c_RegisterGet(I2C0, BMP085_ADDR,0xF6, &press_H);	//TODO read block using i2c
	uint8_t press_L=0;
	i2c_RegisterGet(I2C0, BMP085_ADDR,0xF7, &press_L);

	unsigned long up= (((unsigned long)press_H)<<8)+press_L;
	*pressure = bmp085GetPressure(up);
}


//------------I2C SLAVE--------------
void parse_received_message(){
	if (i2c_rxInProgress==false){
		//TODO block write
		if(i2c_rxBufferIndex>1){
			i2c_registers[i2c_rxBuffer[0]]=i2c_rxBuffer[1];
			if(i2c_rxBuffer[0]==REG_MPU6050_CONFIG){
				MPU6050_config_register();
			}
		}
		//clean the buffer
		for (int i=0;i<i2c_rxBufferIndex;i++){
			i2c_rxBuffer[i]=0;
		}
		i2c_rxBufferIndex=0;
	}
}

//------------RTC----------------
uint8_t RTC_interrupt_type=0;
void RTC_IRQHandler(void)
{
	disableI2cSlaveInterrupts();
	initI2C_Master();
	clockTest_short();
	clockTest_short();
	clockTest_short();
	clockTest_short();
	clockTest_short();
	clockTest_short();
	clockTest_short();

	parse_received_message();

	if(RTC_interrupt_type==0){

		if(sensors & SI1142_SENS){		forceAmbientLightMeasurment();			}
		if(sensors & BMP085_SENS){		forceTemperatureMesurementBMP085();		}
		if(sensors & SI7013_SENS){		forceRhMeasurment();					}	//this must be least to read

		//change RTC time to 100ms
		RTC_CompareSet(0, RTC_COUNT_BETWEEN_WAKEUP_2);
		RTC_interrupt_type=1;
	}
	else if(RTC_interrupt_type==1){
		char buff[30];
		if(sensors & SI7013_SENS){ 	//this detector must be executed first
			uint32_t rhData=0;
			int32_t   tData=0;
			if (readHumidityAndTemperature(&rhData,&tData)!=-1){
				char buff[30];
				sprintf(buff,"Hum:%ld Temp:%ld ",rhData,tData);
				uart_sendText(buff);

				i2c_registers[REG_SI7013_HUM_HIGH]  = rhData/100;
				i2c_registers[REG_SI7013_HUM_LOW]   = rhData%100;
				i2c_registers[REG_SI7013_TEMP_HIGH] = tData/100;
				i2c_registers[REG_SI7013_TEMP_LOW]  = tData%100;
			}
			else {
				uart_sendText("Error measuring temperature and humidity\n");
			}
		}
		if(sensors & SI1142_SENS){
			uint16_t ambientLight=0;

			if (readAmbientLight(&ambientLight)!=-1) {
				sprintf(buff,"ALS:%d ",ambientLight);
				uart_sendText(buff);

				i2c_registers[REG_SI1142_ALS_HIGH]  = ambientLight>>8;
				i2c_registers[REG_SI1142_ALS_LOW]   = (uint8_t)ambientLight;
			}
		}
		if(sensors & BMP085_SENS){
			short temp=0;
			readTemperatureBMP085(&temp);
			sprintf(buff,"Temp:%d ",temp);
			uart_sendText(buff);

			i2c_registers[REG_BMP085_TEMP_HIGH]  = temp/10;
			i2c_registers[REG_BMP085_TEMP_LOW]   = temp%10;

			forcePressureMeasurmentBMP085();
		}
		if(sensors & HMC5883L_SENS){
			int16_t X; 		int16_t Y;			int16_t Z;
			HMC5883L_getCompassData(&X,&Y,&Z);
			uart_sendChar('\n');
			sprintf(buff,"Compass: X[%d] Y[%d] Z[%d]\n",X,Y,Z);
			uart_sendText(buff);

			i2c_registers[REG_HMC5883L_X_HIGH]=      X>>8;
			i2c_registers[REG_HMC5883L_X_LOW] =(uint8_t)X;
			i2c_registers[REG_HMC5883L_Y_HIGH]=      Y>>8;
			i2c_registers[REG_HMC5883L_Y_LOW] =(uint8_t)Y;
			i2c_registers[REG_HMC5883L_Z_HIGH]=      Z>>8;
			i2c_registers[REG_HMC5883L_Z_LOW] =(uint8_t)Z;
		}
		if(sensors & MPU6050_SENS){
			int16_t accel[3];
			int16_t gyro[3];

			// read accelerometer data
			mpu_get_accel_reg(accel,0);
			sprintf(buff,"Acc: X[%d] Y[%d] Z[%d]",accel[0],accel[1],accel[2]);
			uart_sendText(buff);
			i2c_registers[REG_MPU6050_X_ACCEL_HIGH]=      accel[0]>>8;
			i2c_registers[REG_MPU6050_X_ACCEL_LOW] =(uint8_t)accel[0];
			i2c_registers[REG_MPU6050_Y_ACCEL_HIGH]=      accel[1]>>8;
			i2c_registers[REG_MPU6050_Y_ACCEL_LOW] =(uint8_t)accel[1];
			i2c_registers[REG_MPU6050_Z_ACCEL_HIGH]=      accel[2]>>8;
			i2c_registers[REG_MPU6050_Z_ACCEL_LOW] =(uint8_t)accel[2];

			// read gyroscope data
			mpu_get_gyro_reg(gyro,0);
			sprintf(buff,"\tGyro: X[%d] Y[%d] Z[%d]\n",gyro[0],gyro[1],gyro[2]);
			uart_sendText(buff);
			i2c_registers[REG_MPU6050_X_GYRO_HIGH]=      gyro[0]>>8;
			i2c_registers[REG_MPU6050_X_GYRO_LOW] =(uint8_t)gyro[0];
			i2c_registers[REG_MPU6050_Y_GYRO_HIGH]=      gyro[1]>>8;
			i2c_registers[REG_MPU6050_Y_GYRO_LOW] =(uint8_t)gyro[1];
			i2c_registers[REG_MPU6050_Z_GYRO_HIGH]=      gyro[2]>>8;
			i2c_registers[REG_MPU6050_Z_GYRO_LOW] =(uint8_t)gyro[2];

			short sensors;
			long quat[4];
			unsigned char more;

			// check gesture interrupts
			dmp_read_fifo(gyro,accel,quat,0,&sensors,&more);

			// read pedometer data
			unsigned long pedometer_count=0;
			dmp_get_pedometer_step_count(&pedometer_count);
			sprintf(buff,"\t\tPedometer count: %ld",pedometer_count);
			uart_sendText(buff);
			unsigned long pedometer_time=0;
			dmp_get_pedometer_walk_time(&pedometer_time);
			sprintf(buff,"  Pedometer time: %ld \n",pedometer_time);
			uart_sendText(buff);
		}

		//dont change the RTC setting
		RTC_interrupt_type++;
	}
	else if(RTC_interrupt_type==2){
		char buff[30];
		if(sensors & BMP085_SENS){
			uint32_t pressure=0;
			readPressureBMP085(&pressure);
			sprintf(buff,"Press:%ld ",pressure);
			uart_sendText(buff);

			i2c_registers[REG_BMP085_PRESS_XHIGH]=pressure/1000;
			i2c_registers[REG_BMP085_PRESS_HIGH]=(pressure%1000)/10;
			i2c_registers[REG_BMP085_PRESS_LOW]=pressure%10000;
		}
		uart_sendChar('\n');
		//change RTC time to 400ms
		RTC_CompareSet(0, RTC_COUNT_BETWEEN_WAKEUP_1);
		RTC_interrupt_type=0;
	}

	initI2C_Slave();
	enableI2cSlaveInterrupts();

	clockTest_short();
	clockTest_short();
	clockTest_short();
	clockTest_short();
	clockTest_short();
	clockTest_short();
	clockTest_short();

	RTC_IntClear(RTC_IFC_COMP0);
}

int main(void) {
  /* Chip errata */
  CHIP_Init();
  initOscillators();
  initGPIO();
  initUART();

  uart_sendText("\nSTARTUP\n");

  //clear TxBuffer
  for (int i=0;i<I2C_REG_BUFFER_SIZE;i++){
	  i2c_registers[i]=0;
  }

  i2c_registers[REG_DATA_LENGTH]=86;

  initI2C_Master();
  i2c_Scan(I2C0);

  detectSensors();
  initSensors();

  /* Setting up rtc */
  setupRtc();

  initI2C_Slave();

  /* Infinite loop */
  while (1) {
	  while(i2c_rxInProgress){
	      EMU_EnterEM1();
	  }
	  /* Forever enter EM2. The RTC or I2C will wake up the EFM32 */
	  EMU_EnterEM2(false);
  }
}
