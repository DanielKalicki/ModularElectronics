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

volatile int waitingForBleData=0;

//-------------RTC--------------
void RTC_IRQHandler(void){
	if (waitingForBleData==0){

		uint8_t data_length=0;
		i2c_RegisterGet(I2C0,0x10,0x00,&data_length);

		//sprintf(buff,"L:%d [%d]",data_length,counter);

		data_length=87; //test

		uint8_t reg_vals[200];
		i2c_Register_Read_Block(I2C0,0x10,0x00,data_length,reg_vals);

		uart_sendChar('|');

		for(int i=0;i<data_length;i++)
			uart_sendChar(reg_vals[i]);

		uart_sendChar('|');

		waitingForBleData=1;	//enter EMU1 to wait for input data from ble
		//change the RTC setup to wait for data from ble module.
		RTC_CompareSet(0, RTC_COUNT_BETWEEN_WAKEUP_RX);
	}
	else {
		if(rxBuff.wrI>0){
			/*for (int i=0;i<rxBuff.wrI;i++){
				uart_sendChar(rxBuff.data[i]);
			}*/
			if(rxBuff.wrI>3){
				if(rxBuff.data[0]=='W'){
					i2c_RegisterSet(I2C0,0x10,rxBuff.data[1],rxBuff.data[2]);
				}
			}
		}

		clearRxBuffer();

		//wait for the data to be send -> this will be removed in the future
		/*int counter=0;
		while (counter<100) {counter++;__asm("NOP");};*/

		waitingForBleData=0;
		//change the RTC setup
		RTC_CompareSet(0, RTC_COUNT_BETWEEN_WAKEUP);
	}

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

  uint8_t initHM11_fail=1;
  while(initHM11_fail){
	  initHM11_fail=initHM11();
	  testWait();
  }

  /* Setting up rtc */
  setupRtc();

  while(1){
	  if (waitingForBleData==0)
		  EMU_EnterEM2(false);
	  else {
		  EMU_EnterEM1();
	  }

  }
}
