#include "em_device.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_i2c.h"
#include "em_emu.h"
#include <math.h>

#include "i2c_connection.h"
#include "uart_connection.h"
#include "RTC_.h"
#include "em_rtc.h"
#include "AD5933.h"

#define SI1143_ADDR		0x02

#define REG_DATA_LENGTH_VALUE 20

enum registerMap{

	REG_DATA_LENGTH				=0,

	REG_MODULE_ID_1				=1,
	REG_MODULE_ID_2				=2,
	REG_MODULE_ID_3				=3,

	REG_I2C_ADDR				=4,

	REG_SENSORS					=5,

	REG_COMMAND_CODE			=7,		//this registers are used to send commands to the module
	REG_COMMAND_DATA_1			=8,
	REG_COMMAND_DATA_2			=9,

	REG_IMP_1					=10,
	REG_IMP_2					=11,
	REG_IMP_3					=12,

	REG_FREQ_1					=13,
	REG_FREQ_2					=14,
	REG_FREQ_3					=15,

	REG_PULSE_1					=16,
	REG_PULSE_2					=17,
	REG_PULSE_3					=18,

	REG_OX_1					=19,
	REG_OX_2					=20

};

uint8_t sensors=0;

//----------I2C_SLAVE--------
void check_slavesList(){
	if (i2c_slave_address<0x10){ //addreses lower than 0x10 are set as a default at startup for i2c line sniffing
		if(slavesListCheck[0]>=0xFFFFFFFE && slavesListCheck[1]==0xFFFFFFFF && slavesListCheck[2]==0xFFFFFFFF && slavesListCheck[3]==0xFFFFFFFF){
			//this code will be reached if i2c list is full
			i2c_slave_address = 0x16;
			i2c_registers[REG_I2C_ADDR] = i2c_slave_address;
		}
	}
}


//-------------INIT--------------
void initOscillators(void){
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);                   // enable GPIO peripheral clock
}
void initGPIO(void){
	//UART
	GPIO_PinModeSet(COM_PORT, TX_PIN, gpioModePushPull, 1); // set TX pin to push-pull output, initialize high (otherwise glitches can occur)
	GPIO_PinModeSet(COM_PORT, RX_PIN, gpioModeInput, 0);    // set RX pin as input (no filter)
}

void clockTest();
void clockTest_short() {
	long int i=0;
	for(;i<121L;++i) {
	  if(i==120L)
		  GPIO_PortOutSet(gpioPortF, 0x80);
	  if(i==60L)
		  GPIO_PortOutClear(gpioPortF, 0x80);
	}
}

void setupSi1143(){

}


void initDevices(){
	//-----AD5933----
	if(sensors&0x01){
		AD5933_init();
	}
	//-----LTC2942----
	if(sensors&0x02){
		setupSi1143();
	}
	clockTest_short();
}

uint8_t RTC_interrupt_type=0;

void RTC_IRQHandler(void){

	//disableI2cSlaveInterrupts();
	//initI2C_Master();

	i2c_registers[REG_DATA_LENGTH]=REG_DATA_LENGTH_VALUE;
	i2c_registers[REG_I2C_ADDR]   =i2c_slave_address;
	i2c_registers[REG_SENSORS]	  =sensors;
	//TODO add command to detect i2c device once again

	static uint8_t counter=0;
	i2c_registers[REG_COMMAND_CODE]=counter;
	counter++;
	if(counter==255) counter=0;

	char buff[30];

	if(RTC_interrupt_type==0){
		AD5933_initFreq();

		//change RTC time to 100ms
		RTC_CompareSet(0, RTC_COUNT_BETWEEN_WAKEUP_2);
		RTC_interrupt_type=1;

	}
	else if(RTC_interrupt_type==1){
		AD5933_startMeasurment();

		//change RTC time to 100ms
		RTC_CompareSet(0, RTC_COUNT_BETWEEN_WAKEUP_2);
		RTC_interrupt_type=2;
	}
	else if(RTC_interrupt_type==2){

		int16_t re = (int16_t)AD5933_getRealValue();
		int16_t img = (int16_t)AD5933_getImgValue();

		sprintf(buff,"r%d\ti%d\t",re,img);
		uart_sendText(buff);

		double dRe = (double)re;
		double dImg = (double)img;

		//gain calculated for 5xGain 2Vp-p configuration
		#define IMP_CAL_GAIN_1	1.005e-9

		//gain calculated for 1xGain 2Vp-p configuration (this doesnt work good for small resistance like 30k)
		#define IMP_CAL_GAIN_2	1.383e-9

		double imp = 1.0/(sqrt(dRe*dRe+dImg*dImg)*IMP_CAL_GAIN_1);

		sprintf(buff,"imp: %ld\n",(uint32_t)imp);
		uart_sendText(buff);

		AD5933_standby();

		//change RTC time to 100ms
		RTC_CompareSet(0, RTC_COUNT_BETWEEN_WAKEUP_2);
		RTC_interrupt_type=0;
	}

	check_slavesList();

	clockTest_short();clockTest_short();clockTest_short();clockTest_short();clockTest_short();clockTest_short();clockTest_short();

	//initI2C_Slave();
	//enableI2cSlaveInterrupts();

	/* Clear interrupt source */
	RTC_IntClear(RTC_IFC_COMP0);
}

void detectDevices(){

	//-----ADP5933----
	if (i2c_Detect(I2C0,AD5933_ADDR)==1){
		uart_sendText("\t\tADP5933 detected\n");
		sensors |= 0x01;
	}
	else uart_sendText("\t---\tADP5933 NOT DETECTED\t---\t\n");

	//-----SI1143----
	if (i2c_Detect(I2C0,SI1143_ADDR)==1){
		uart_sendText("\t\tSi1143 detected\n");
		sensors |= 0x02;
	}
	else uart_sendText("\t---\tSi1143 NOT DETECTED\t---\t\n");
}

//-------------MAIN-------------
int main(void)
{
  CHIP_Init();

  initOscillators();
  initGPIO();

  initUART();

  for (int i=0;i<500;i++){	//wait for the voltage to stabilize
	  clockTest_short();
  }

  i2c_slave_address=0x16;	//0x02

  for (int i=0;i<100;i++){
	  i2c_registers[i]=0;
  }

  i2c_registers[REG_DATA_LENGTH]=REG_DATA_LENGTH_VALUE;
  i2c_registers[REG_MODULE_ID_1]='H';
  i2c_registers[REG_MODULE_ID_2]='L';
  i2c_registers[REG_MODULE_ID_3]='M';
  //id: 4997699

  initI2C_Master();
  i2c_Scan(I2C0);

  detectDevices();
  uart_sendText("\nSTARTUP\n");
  initDevices();

  setupRtc();

  //initI2C_Slave();

  /* Infinite loop */
  while (1) {
	  //EMU_EnterEM2(false);
	  while(i2c_rxInProgress){
	     EMU_EnterEM1();
	  }
	   /* Forever enter EM2. The RTC or I2C will wake up the EFM32 */
	  EMU_EnterEM2(false);
  }
}
