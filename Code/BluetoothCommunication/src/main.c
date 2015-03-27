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

#define REGISTER_SIZE 50
volatile uint8_t registers[REGISTER_SIZE];

enum registerMap{
	REG_DATA_LENGTH				=0,

	REG_MODULE_ID_1				=1,
	REG_MODULE_ID_2				=2,
	REG_MODULE_ID_3				=3,
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

uint32_t slavesList[4];		//binary list of detected i2c addresses
#define MODULE_LIST_SIZE 10
uint8_t moduleList[MODULE_LIST_SIZE];		//list of i2c addresses connected to the bus
uint8_t moduleList_len[MODULE_LIST_SIZE];	//list of i2c modules data lengths
uint8_t i_moduleList=0;

void scanI2cSlaves(uint8_t scanStart){
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
					if(i_moduleList==MODULE_LIST_SIZE) i_moduleList=9;
				}
			}
		}
		else slavesList[i>>5]&=~(1<<(i&0x1F));
	}
}

volatile int RTC_interrupt=0;

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
	else if(RTC_interrupt==9){

		//every RTC interrupt the ic is scanning 4 i2c addresses and adding an address to a moduleList if it exists.
		static uint8_t scanStart=0;
		scanI2cSlaves(scanStart);
		if (scanStart<124) scanStart+=4;
		else scanStart=0;

		//send the detected slave list addresses through ble
		uart_sendChar('|');
		uart_sendChar(registers[REG_DATA_LENGTH]);
		uart_sendChar(registers[REG_MODULE_ID_1]);
		uart_sendChar(registers[REG_MODULE_ID_2]);
		uart_sendChar(registers[REG_MODULE_ID_3]);
		for (int i=0;i<4;i++){
			uart_sendChar((uint8_t)(slavesList[i]));
			uart_sendChar((uint8_t)(slavesList[i]>>8));
			uart_sendChar((uint8_t)(slavesList[i]>>16));
			uart_sendChar((uint8_t)(slavesList[i]>>24));
		}

		//send module list through ble.
		for (int i=0; i<MODULE_LIST_SIZE;i++) uart_sendChar((uint8_t)moduleList[i]);
		uart_sendChar('|');
	}
	//read data from modules
	else if (RTC_interrupt==2 || RTC_interrupt==5 || RTC_interrupt==8){

		uint8_t mod_numb = (RTC_interrupt-2)/3;

		if(moduleList[mod_numb]>=0x10){
			uart_sendChar('|');

			uint8_t reg_vals[200];
			i2c_Register_Read_Block(I2C0,moduleList[mod_numb],0x00,moduleList_len[mod_numb],reg_vals);

			for(int i=0;i<moduleList_len[mod_numb];i++)
				uart_sendChar(reg_vals[i]);

			uart_sendChar('|');

			moduleList_len[mod_numb]=reg_vals[0];
			if(moduleList_len[mod_numb]==0) moduleList_len[mod_numb]=10;
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
  for (int i=0;i<MODULE_LIST_SIZE;i++) moduleList[i]=0;
  for (int i=0;i<MODULE_LIST_SIZE;i++) moduleList_len[i]=10;
  i_moduleList=0;

  registers[REG_DATA_LENGTH]=30;
  registers[REG_MODULE_ID_1]=0;
  registers[REG_MODULE_ID_2]='M';
  registers[REG_MODULE_ID_3]='C';

  initI2C();
  initUART_baud(115200);

  uint8_t initHM11_fail=1;
  while(initHM11_fail){
	  initHM11_fail=initHM11();
	  testWait();
  }

  //scan every i2c address
  for (uint8_t scanStart=0;scanStart<124;scanStart+=4){
	  scanI2cSlaves(scanStart);
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
