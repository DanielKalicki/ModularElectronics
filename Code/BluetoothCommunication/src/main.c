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

volatile int RTC_interrupt=0;

//-------------RTC--------------

uint32_t slavesList[4];
uint8_t moduleList[10];
uint8_t i_moduleList=0;

void RTC_IRQHandler(void){

	//Time for executing the BLE RX commands.
	if(RTC_interrupt==0) {
		if(rxBuff.wrI>0){
			if(rxBuff.wrI>4){
				if(rxBuff.data[1]=='W'){
					//command in the form of I2C_ADDR | Write | reg_addr | data
					i2c_RegisterSet(I2C0,rxBuff.data[0],rxBuff.data[2],rxBuff.data[3]);
				}
			}
			clearRxBuffer();
		}
	}
	//i2c scan from the master
	else if(RTC_interrupt==6){
		static uint8_t scanStart=0;
		for (uint8_t i=scanStart;i<scanStart+4;i++){
			if (i2c_Detect(I2C0,(i*2))==1){
				slavesList[i>>5]|=(1<<(i&0x1F));		//i&0x1F - i%32
				uint8_t existsInList=0;
				for (int iM=0;iM<i_moduleList;iM++){
					if (moduleList[iM]==i*2){
						existsInList=1;
						break;
					}
				}
				if (existsInList==0){
					if (i*2>=0x10){ //dont add modules with addr less than 0x10
						moduleList[i_moduleList]=i*2;
						i_moduleList++;
						if(i_moduleList==10) i_moduleList=9;
					}
				}
			}
			else slavesList[i>>5]&=~(1<<(i&0x1F));
		}
		if (scanStart<124) scanStart+=4;
		else scanStart=0;

		uart_sendChar('|');
		for (int i=0;i<4;i++){
			uart_sendChar((uint8_t)(slavesList[i]));
			uart_sendChar((uint8_t)(slavesList[i]>>8));
			uart_sendChar((uint8_t)(slavesList[i]>>16));
			uart_sendChar((uint8_t)(slavesList[i]>>24));
		}
		uart_sendChar(']');
		for (int i=0; i<10;i++) uart_sendChar((uint8_t)moduleList[i]);
		uart_sendChar('|');
	}
	//first device
	else if (RTC_interrupt==2){

		if(moduleList[0]>=0x10){
			static uint8_t len=10;
			uart_sendChar('|');

			uint8_t reg_vals[200];
			i2c_Register_Read_Block(I2C0,moduleList[0],0x00,len,reg_vals);

			for(int i=0;i<len;i++)
				uart_sendChar(reg_vals[i]);

			uart_sendChar('|');

			len=reg_vals[0];
			if(len==0) len=10;
		}
	}

	RTC_interrupt++;
	if(RTC_interrupt>9) RTC_interrupt=0;

	/* Clear interrupt source */
	RTC_IntClear(RTC_IFC_COMP0);
}

//-------------MAIN-------------
int main(void)
{
  CHIP_Init();
  initOscillators();

  for (int i=0;i<4;i++)  slavesList[i]=0;
  for (int i=0;i<10;i++) moduleList[i]=0;
  i_moduleList=0;

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
	  /*if (waitingForBleData==0)
		  EMU_EnterEM2(false);
	  else {*/
		  EMU_EnterEM1();
	  //}

  }
}
