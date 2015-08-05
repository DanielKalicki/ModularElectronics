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

#include "ucPeripheralDrivers\i2c_connection.h"
#include "ucPeripheralDrivers\RTC_.h"
#include "ucPeripheralDrivers\leuart_connection.h"

#include "sensorsDrivers\HMC5883L.h"
#include "sensorsDrivers\MPU6050.h"
#include "sensorsDrivers\inv_mpu.h"
#include "sensorsDrivers\inv_mpu_dmp_motion_driver.h"
//#include "sensorsDrivers\AS3935.h"
#include "sensorsDrivers\BMP180.h"
#include "sensorsDrivers\Si7013.h"
#include "sensorsDrivers\Si114x.h"

//This register contains information from the module
#define DEV_INT_REG_BUFFER_SIZE       	100
volatile uint8_t devInternalRegisters[DEV_INT_REG_BUFFER_SIZE];

#define sensors devInternalRegisters[5]		//5 - REG_SENSORS

#define SI7013_SENS		0x01
#define SI114x_SENS		0x02
//#define AS3935_SENS		0x04
#define BMP180_SENS		0x08
#define HMC5883L_SENS	0x10
#define MPU6050_SENS	0x20

/********** i2c slave register map **********
 * [0:9] - configuration registers
 * [10:11] - Si7013 humidity   -> 	e.g. hum=56.25%  [10]=56 [11]=25
 * [12:13] - Si7013 temperature ->	e.g. temp=25.23*C [12]=25 [13]=23
 * [14:15] - Si1142 ambient light ->  [14] -> ambient light High byte  [15] -> ambient light Low byte
 * [16]    - AS3935 number of lighting detection
 * [17:18] - BMP180 temperature if it is measured, else it should be zero ->	e.g. temp=25.23*C [17]=25 [18]=23
 * [19:21] - BMP180 pressure  -> e.g. pressure=101399.8 [19]=101 [20]=39 [21]=98
 * [22:27] - HMC5883L [22:23] - X data, [24:25] - Y data [26:27] - Z data
 * [28:33] - MPU6050 accel  [28:29] - X accel data [30:31] - Y accel data [32:33] - Z accel data
 * [34:39] - MPU6050 gyro   [34:35] - X gyro data  [36:37] - Y gyro data  [38:39] - Z gyro data
 * [40:47] - MPU pedometer  [40:43] - number of steps in binary format
 *                          [44:47] - number of walking time in binary format in miliseconds
 * [48:57] - MPU dmp events [48:57] - detected tap events
 * [58:67]  AS3935 every byte contains estimate distance from lighting for every lighting detection
 * */
enum registerMap{

	REG_DATA_LENGTH				=0,

	REG_MODULE_ID_1				=1,
	REG_MODULE_ID_2				=2,
	REG_MODULE_ID_3				=3,

	REG_I2C_ADDR				=4,

	REG_SENSORS					=5,
	REG_MPU6050_CONFIG			=REG_MPU6050_CONFIG_NUMBER,				//BIT0 ---- 0 - normal mode,   1 - low-power accel-only mode

	REG_COMMAND_CODE			=7,		//this registers are used to send commands to the module
	REG_COMMAND_DATA_1			=8,
	REG_COMMAND_DATA_2			=9,

	REG_SI7013_HUM_HIGH			=10,
	REG_SI7013_HUM_LOW			=11,

	REG_SI7013_TEMP_HIGH		=12,
	REG_SI7013_TEMP_LOW			=13,

	REG_SI114x_ALS_HIGH			=14,
	REG_SI114x_ALS_LOW			=15,

	REG_AS3935_LIGHT_DET_NUM	=16,

	REG_BMP180_TEMP_HIGH		=17,
	REG_BMP180_TEMP_LOW			=18,

	REG_BMP180_PRESS_XHIGH		=19,
	REG_BMP180_PRESS_HIGH		=20,
	REG_BMP180_PRESS_LOW		=21,

	REG_HMC5883L_X_HIGH			=22,
	REG_HMC5883L_X_LOW			=23,
	REG_HMC5883L_Y_HIGH			=24,
	REG_HMC5883L_Y_LOW			=25,
	REG_HMC5883L_Z_HIGH			=26,
	REG_HMC5883L_Z_LOW			=27,

	REG_MPU6050_X_ACCEL_HIGH	=28,
	REG_MPU6050_X_ACCEL_LOW		=29,
	REG_MPU6050_Y_ACCEL_HIGH	=30,
	REG_MPU6050_Y_ACCEL_LOW		=31,
	REG_MPU6050_Z_ACCEL_HIGH	=32,
	REG_MPU6050_Z_ACCEL_LOW		=33,

	REG_MPU6050_X_GYRO_HIGH		=34,
	REG_MPU6050_X_GYRO_LOW		=35,
	REG_MPU6050_Y_GYRO_HIGH		=36,
	REG_MPU6050_Y_GYRO_LOW		=37,
	REG_MPU6050_Z_GYRO_HIGH		=38,
	REG_MPU6050_Z_GYRO_LOW		=39,

	REG_MPU6050_X_PEDO_COUNT_1	=40,
	REG_MPU6050_X_PEDO_COUNT_2	=41,
	REG_MPU6050_X_PEDO_COUNT_3	=42,
	REG_MPU6050_X_PEDO_COUNT_4	=43,

	REG_MPU6050_X_PEDO_TIME_1	=44,
	REG_MPU6050_X_PEDO_TIME_2	=45,
	REG_MPU6050_X_PEDO_TIME_3	=46,
	REG_MPU6050_X_PEDO_TIME_4	=47,

	REG_MPU6050_X_TAP_EVENT_1	=48,
	REG_MPU6050_X_TAP_EVENT_2	=49,
	REG_MPU6050_X_TAP_EVENT_3	=50,
	REG_MPU6050_X_TAP_EVENT_4	=51,
	REG_MPU6050_X_TAP_EVENT_5	=52,
	REG_MPU6050_X_TAP_EVENT_6	=53,
	REG_MPU6050_X_TAP_EVENT_7	=54,
	REG_MPU6050_X_TAP_EVENT_8	=55,
	REG_MPU6050_X_TAP_EVENT_9	=56,
	REG_MPU6050_X_TAP_EVENT_10	=57,

	REG_AS3935_lIGH_DIST_1		=58,
	REG_AS3935_lIGH_DIST_2,
	REG_AS3935_lIGH_DIST_3,
	REG_AS3935_lIGH_DIST_4,
	REG_AS3935_lIGH_DIST_5,
	REG_AS3935_lIGH_DIST_6,
	REG_AS3935_lIGH_DIST_7,
	REG_AS3935_lIGH_DIST_8,
	REG_AS3935_lIGH_DIST_9,
	REG_AS3935_lIGH_DIST_10,
};

#define REG_DATA_LENGTH_VALUE	68

//-------------INIT--------------
void initOscillators(void){
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);  // select HFCORECLK/2 as clock source to LFB
  CMU_ClockEnable(cmuClock_CORELE, true);                  // enable the Low Energy Peripheral Interface clock

  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_GPIO, true);                    // enable GPIO peripheral clock
}
void initGPIO(void){

}

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
	LeUart_SendText("\nDETECTING CONNECTED SENSORS\n");
	sensors = 0;

	//-----------SI7013---------
	if (Si7013_detect()){
		LeUart_SendText("\t\tSi7013 detected\n");
		sensors |= 0x01;
	}
	else
		LeUart_SendText("---\t\tSi7013 NOT DETECTED\t\t---\n");

	//-----------SI1142---------
	if (Si114x_detect()){
		LeUart_SendText("\t\tSi1142 detected\n");
		sensors |= 0x02;
	}
	else
		LeUart_SendText("---\t\tSi1142 NOT DETECTED\t\t---\n");

	//-----------BMP180---------
	if (BMP180_detect()) {
		LeUart_SendText("\t\tBMP180 detected\n");
		sensors |= 0x08;
	}
	else
		LeUart_SendText("---\t\tBMP180 NOT DETECTED\t\t---\n");

	//-----------HMC5883L---------
	if (HMC5883L_detect()) {
		LeUart_SendText("\t\tHMC5883L detected\n");
		sensors |= 0x10;
	}
	else
		LeUart_SendText("---\t\tHMC5883L NOT DETECTED\t\t---\n");

	//-----------MPU6050---------
	if (MPU6050_detect()) {
		LeUart_SendText("\t\tMPU6050 detected\n");
		sensors |= 0x20;
	}
	else
		LeUart_SendText("---\t\tMPU6050 NOT DETECTED\t\t---\n");
}

void initSensors()
{
	LeUart_SendText("\nINITIALIZATION: ");

	if(sensors & SI7013_SENS)
	{
		Si7013_init();
		LeUart_SendText(" SI7013");
	}
	if(sensors & SI114x_SENS)
	{
		Si114x_init();
		LeUart_SendText(" SI1142");
	}
	if(sensors & BMP180_SENS)
	{
		BMP180_init();
		LeUart_SendText(" BMP180");
	}
	if(sensors & HMC5883L_SENS)
	{
		HMC5883L_init();
		LeUart_SendText(" HMC5883L");
	}
	if(sensors & MPU6050_SENS)
	{
		MPU6050_init();
		LeUart_SendText(" MPU6050");
	}
	LeUart_SendChar('\n');
}

//------------RTC----------------

#ifdef DEBUG
void printResults(void)
{
	char buff[30];

	uint32_t pressure=(uint32_t)devInternalRegisters[REG_BMP180_PRESS_XHIGH]*1000+(uint32_t)devInternalRegisters[REG_BMP180_PRESS_HIGH]*10+(uint32_t)devInternalRegisters[REG_BMP180_PRESS_LOW];
	sprintf(buff,"\nPress:%ld ",pressure);
	LeUart_SendText(buff);

	uint32_t rhData = (uint32_t)devInternalRegisters[REG_SI7013_HUM_HIGH] * 100 + (uint32_t)devInternalRegisters[REG_SI7013_HUM_LOW];
	int32_t   tData = (int32_t)devInternalRegisters[REG_SI7013_TEMP_HIGH] * 100 + (uint32_t)devInternalRegisters[REG_SI7013_TEMP_LOW];
	sprintf(buff,"Hum:%ld Temp:%ld ",rhData,tData);
	LeUart_SendText(buff);

	uint16_t ambientLight = (((uint16_t)devInternalRegisters[REG_SI114x_ALS_HIGH])<<8) + (uint16_t)devInternalRegisters[REG_SI114x_ALS_LOW];
	sprintf(buff,"ALS:%d ",ambientLight);
	LeUart_SendText(buff);

	short temp=(short)devInternalRegisters[REG_BMP180_TEMP_HIGH]*10+(short)devInternalRegisters[REG_BMP180_TEMP_LOW];
	sprintf(buff,"Temp:%d ",temp);
	LeUart_SendText(buff);

	int16_t compass_x=((int16_t)devInternalRegisters[REG_HMC5883L_X_HIGH]<<8) + devInternalRegisters[REG_HMC5883L_X_LOW];
	int16_t compass_y=((int16_t)devInternalRegisters[REG_HMC5883L_Y_HIGH]<<8) + devInternalRegisters[REG_HMC5883L_Y_LOW];
	int16_t compass_z=((int16_t)devInternalRegisters[REG_HMC5883L_Z_HIGH]<<8) + devInternalRegisters[REG_HMC5883L_Z_LOW];
	LeUart_SendChar('\n');
	sprintf(buff,"Compass: X[%d] Y[%d] Z[%d]\n",compass_x,compass_y,compass_z);
	LeUart_SendText(buff);

	int16_t accel[3];
	accel[0]=((int16_t)devInternalRegisters[REG_MPU6050_X_ACCEL_HIGH]<<8) + devInternalRegisters[REG_MPU6050_X_ACCEL_LOW];
	accel[1]=((int16_t)devInternalRegisters[REG_MPU6050_Y_ACCEL_HIGH]<<8) + devInternalRegisters[REG_MPU6050_Y_ACCEL_LOW];
	accel[2]=((int16_t)devInternalRegisters[REG_MPU6050_Z_ACCEL_HIGH]<<8) + devInternalRegisters[REG_MPU6050_Z_ACCEL_LOW];
	sprintf(buff,"Acc: X[%d] Y[%d] Z[%d]",accel[0],accel[1],accel[2]);
	LeUart_SendText(buff);

	int16_t gyro[3];
	gyro[0]=((int16_t)devInternalRegisters[REG_MPU6050_X_GYRO_HIGH]<<8) + devInternalRegisters[REG_MPU6050_X_GYRO_LOW];
	gyro[1]=((int16_t)devInternalRegisters[REG_MPU6050_Y_GYRO_HIGH]<<8) + devInternalRegisters[REG_MPU6050_Y_GYRO_LOW];
	gyro[2]=((int16_t)devInternalRegisters[REG_MPU6050_Z_GYRO_HIGH]<<8) + devInternalRegisters[REG_MPU6050_Z_GYRO_LOW];
	sprintf(buff,"\tGyro: X[%d] Y[%d] Z[%d]\n",gyro[0],gyro[1],gyro[2]);
	LeUart_SendText(buff);

	unsigned long pedometer_count=((unsigned long)devInternalRegisters[REG_MPU6050_X_PEDO_COUNT_1]<<24UL) +
								  ((unsigned long)devInternalRegisters[REG_MPU6050_X_PEDO_COUNT_2]<<16UL) +
								  ((unsigned long)devInternalRegisters[REG_MPU6050_X_PEDO_COUNT_3]<<8UL) +
								  ((unsigned long)devInternalRegisters[REG_MPU6050_X_PEDO_COUNT_4]);
	sprintf(buff,"\t\tPedometer count: %ld",pedometer_count);
	LeUart_SendText(buff);

	unsigned long pedometer_time=((unsigned long)devInternalRegisters[REG_MPU6050_X_PEDO_TIME_1]<<24UL) +
							     ((unsigned long)devInternalRegisters[REG_MPU6050_X_PEDO_TIME_2]<<16UL) +
								 ((unsigned long)devInternalRegisters[REG_MPU6050_X_PEDO_TIME_3]<<8UL) +
								 ((unsigned long)devInternalRegisters[REG_MPU6050_X_PEDO_TIME_4]);
	sprintf(buff,"\t\tPedometer time: %ld",pedometer_time);
	LeUart_SendText(buff);
}
#endif

uint8_t time_interrupt_type=0;
void RTC_IRQHandler(void)
{
	clockTest_short();clockTest_short();clockTest_short();clockTest_short();clockTest_short();clockTest_short();clockTest_short();

	devInternalRegisters[REG_DATA_LENGTH]=REG_DATA_LENGTH_VALUE;

	if(time_interrupt_type==0)
	{
		if(sensors & SI114x_SENS)
		{
			Si114x_forceAmbientLightMeasurment();
		}
		if(sensors & BMP180_SENS)
		{
			BMP180_forceTemperatureMesurement();
		}
		if(sensors & SI7013_SENS) 	//this must be least to read
		{
			Si7013_forceRhMeasurment();
		}

		RTC_setTime(100);	//change RTC time to 100ms
		time_interrupt_type=1;

	}
	else if(time_interrupt_type==1)
	{
		if(sensors & SI7013_SENS)	//this sensor must be executed first
		{
			uint32_t rhData=0;
			int32_t   tData=0;
			if (Si7013_readHumidityAndTemperature(&rhData,&tData)!=-1){
				devInternalRegisters[REG_SI7013_HUM_HIGH]  = rhData/100;
				devInternalRegisters[REG_SI7013_HUM_LOW]   = rhData%100;
				devInternalRegisters[REG_SI7013_TEMP_HIGH] = tData/100;
				devInternalRegisters[REG_SI7013_TEMP_LOW]  = tData%100;
			}
#ifdef DEBUG
			else {
				LeUart_SendText("Error measuring temperature and humidity\n");
			}
#endif
		}

		/* Read Si114x ambient light */
		if(sensors & SI114x_SENS)
		{
			uint16_t ambientLight=0;

			if (Si114x_readAmbientLight(&ambientLight)!=-1)
			{
				devInternalRegisters[REG_SI114x_ALS_HIGH]  = ambientLight>>8;
				devInternalRegisters[REG_SI114x_ALS_LOW]   = (uint8_t)ambientLight;
			}
		}

		/* Read temperature from BMP180 */
		if(sensors & BMP180_SENS)
		{
			short temp=0;
			BMP180_readTemperature(&temp);

			devInternalRegisters[REG_BMP180_TEMP_HIGH]  = temp/10;
			devInternalRegisters[REG_BMP180_TEMP_LOW]   = temp%10;

			BMP180_forcePressureMeasurment();
		}

		/* Read magnetometer data */
		if(sensors & HMC5883L_SENS){
			int16_t X; 		int16_t Y;			int16_t Z;
			HMC5883L_getCompassData(&X,&Y,&Z);

			devInternalRegisters[REG_HMC5883L_X_HIGH]=      X>>8;
			devInternalRegisters[REG_HMC5883L_X_LOW] =(uint8_t)X;
			devInternalRegisters[REG_HMC5883L_Y_HIGH]=      Y>>8;
			devInternalRegisters[REG_HMC5883L_Y_LOW] =(uint8_t)Y;
			devInternalRegisters[REG_HMC5883L_Z_HIGH]=      Z>>8;
			devInternalRegisters[REG_HMC5883L_Z_LOW] =(uint8_t)Z;
		}

		/* Read accel and gyro data */
		if(sensors & MPU6050_SENS){
			int16_t accel[3];
			int16_t gyro[3];

			// read accelerometer data
			mpu_get_accel_reg(accel,0);
			devInternalRegisters[REG_MPU6050_X_ACCEL_HIGH]=      accel[0]>>8;
			devInternalRegisters[REG_MPU6050_X_ACCEL_LOW] =(uint8_t)accel[0];
			devInternalRegisters[REG_MPU6050_Y_ACCEL_HIGH]=      accel[1]>>8;
			devInternalRegisters[REG_MPU6050_Y_ACCEL_LOW] =(uint8_t)accel[1];
			devInternalRegisters[REG_MPU6050_Z_ACCEL_HIGH]=      accel[2]>>8;
			devInternalRegisters[REG_MPU6050_Z_ACCEL_LOW] =(uint8_t)accel[2];

			// read gyroscope data
			mpu_get_gyro_reg(gyro,0);
			devInternalRegisters[REG_MPU6050_X_GYRO_HIGH]=      gyro[0]>>8;
			devInternalRegisters[REG_MPU6050_X_GYRO_LOW] =(uint8_t)gyro[0];
			devInternalRegisters[REG_MPU6050_Y_GYRO_HIGH]=      gyro[1]>>8;
			devInternalRegisters[REG_MPU6050_Y_GYRO_LOW] =(uint8_t)gyro[1];
			devInternalRegisters[REG_MPU6050_Z_GYRO_HIGH]=      gyro[2]>>8;
			devInternalRegisters[REG_MPU6050_Z_GYRO_LOW] =(uint8_t)gyro[2];

			short sensors_mpu;
			long quat[4];
			unsigned char more;

			// check gesture interrupts
			dmp_read_fifo(gyro,accel,quat,0,&sensors_mpu,&more);

			for (int i=0;i<10;i++){
				if(i<i_tapData){
					devInternalRegisters[REG_MPU6050_X_TAP_EVENT_1+i]=tapData[i];
				}
				else {
					devInternalRegisters[REG_MPU6050_X_TAP_EVENT_1+i]=0;
				}
			}
			i_tapData=0;

			// read pedometer data
			unsigned long pedometer_count=0;
			dmp_get_pedometer_step_count(&pedometer_count);
			devInternalRegisters[REG_MPU6050_X_PEDO_COUNT_1]=(uint8_t)(pedometer_count>>24);
			devInternalRegisters[REG_MPU6050_X_PEDO_COUNT_2]=(uint8_t)(pedometer_count>>16);
			devInternalRegisters[REG_MPU6050_X_PEDO_COUNT_3]=(uint8_t)(pedometer_count>>8);
			devInternalRegisters[REG_MPU6050_X_PEDO_COUNT_4]=(uint8_t)(pedometer_count);

			unsigned long pedometer_time=0;
			dmp_get_pedometer_walk_time(&pedometer_time);
			devInternalRegisters[REG_MPU6050_X_PEDO_TIME_1]=(uint8_t)(pedometer_time>>24);
			devInternalRegisters[REG_MPU6050_X_PEDO_TIME_2]=(uint8_t)(pedometer_time>>16);
			devInternalRegisters[REG_MPU6050_X_PEDO_TIME_3]=(uint8_t)(pedometer_time>>8);
			devInternalRegisters[REG_MPU6050_X_PEDO_TIME_4]=(uint8_t)(pedometer_time);
		}

		//dont change the RTC setting
		time_interrupt_type++;
	}
	else if(time_interrupt_type==2){
		/* Read pressure */
		if(sensors & BMP180_SENS){
			uint32_t pressure=0;
			BMP180_readPressure(&pressure);

			devInternalRegisters[REG_BMP180_PRESS_XHIGH]=pressure/1000;
			devInternalRegisters[REG_BMP180_PRESS_HIGH] =(pressure%1000)/10;
			devInternalRegisters[REG_BMP180_PRESS_LOW]  =pressure%10000;
		}

		//change RTC time to 400ms
		RTC_setTime(400);
		time_interrupt_type=0;

		//after getting all the data print it
		printResults();
	}

	clockTest_short();	clockTest_short();	clockTest_short();	clockTest_short();	clockTest_short();	clockTest_short();	clockTest_short();

	RTC_clearInt();
}

void initInterfaces(){
#ifdef DEBUG
	/* LeUart interface initialization */
	struct LeUart_Settings leuartSettings;
	leuartSettings.leuart_com_port = gpioPortF;
	leuartSettings.leuart_tx_pin = 0;
	leuartSettings.leuart_rx_pin = 1;
	leuartSettings.leuart_port_location = 3;
	leuartSettings.leuart_baudrate = 115200;
	LeUart_Init(leuartSettings);
	LeUart_SendText("Startup\n");
#endif

	/* i2c initialization */
	struct I2C_Settings i2cSettings;
	i2cSettings.i2c_SCL_port = gpioPortD;
	i2cSettings.i2c_SCL_pin =  7;
	i2cSettings.i2c_SDA_port = gpioPortD;
	i2cSettings.i2c_SDA_pin =  6;
	i2cSettings.i2c_port_location =  1;
	i2c_InitMaster(i2cSettings);
	i2c_Scan(I2C0);
}

int main(void) {

  CHIP_Init();
  initOscillators();
  initGPIO();

  initInterfaces();

  //TODO remove this later
  for (int i=0;i<DEV_INT_REG_BUFFER_SIZE;i++){
	  devInternalRegisters[i]=0;
  }
  devInternalRegisters[REG_DATA_LENGTH]=REG_DATA_LENGTH_VALUE;
  devInternalRegisters[REG_MODULE_ID_1]='E';
  devInternalRegisters[REG_MODULE_ID_2]='S';
  devInternalRegisters[REG_MODULE_ID_3]='N';

  //sensors initalization
  detectSensors();
  initSensors();

  //RTC initialization
  RTC_init();
  RTC_setTime(500);
  RTC_enableInt();

  /* Infinite loop */
  while (1) {
	  /* Forever enter EM2. The RTC or I2C will wake up the EFM32 */
	  EMU_EnterEM2(false);
  }
}
