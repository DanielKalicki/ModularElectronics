#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "ucPeripheralDrivers\uart_connection.h"
#include "ucPeripheralDrivers\spi_connection.h"

#include "sensorsDrivers\AS3953.h"

#include <stdio.h>

#define SPI_PORTC     gpioPortC // USART1 (location #0) MISO and MOSI are on PORTD
#define SPI_PORTB     gpioPortB // USART1 (location #0) SS and SCLK are on PORTC
#define SPI_MISO_PIN  1  // PC1
#define SPI_MOSI_PIN  0  // PC0
#define SPI_CS_PIN    8  // PB8
#define SPI_SCLK_PIN  7  // PB7

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
	for(;i<12001L;++i) {
	  if(i==12000L)
		  GPIO_PortOutSet(gpioPortF, 0x4);
	  if(i==6000L)
		  GPIO_PortOutClear(gpioPortF, 0x4);
	}
}

void initOscillators(void)
{
	  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);  // select HFCORECLK/2 as clock source to LFB
	  CMU_ClockEnable(cmuClock_CORELE, true);                  // enable the Low Energy Peripheral Interface clock

	  CMU_ClockEnable(cmuClock_HFPER, true);
	  CMU_ClockEnable(cmuClock_GPIO, true);                    // enable GPIO peripheral clock
}

void init_uart_interface(void){
	/* UART interface initialization */
	struct UART_Settings uartSettings;
	uartSettings.uart_com_port=gpioPortD;
	uartSettings.uart_tx_pin=7;
	uartSettings.uart_rx_pin=6;
	uartSettings.uart_port_location=2;
	uartSettings.uart_speed=115200;

	uart_init(uartSettings);
}

void init_spi_interface(void){
	/* SPI interface initialization */
	struct SPI_Settings spiSettings;
	spiSettings.spi_miso_port=gpioPortC;
	spiSettings.spi_miso_pin= 1;
	spiSettings.spi_mosi_port=gpioPortC;
	spiSettings.spi_mosi_pin= 0;
	spiSettings.spi_sclk_port=gpioPortB;
	spiSettings.spi_sclk_pin= 7;
	spiSettings.spi_location=0;

	spi_init(spiSettings);
}

void as3953_init(void){

	static uint8_t counter=0;

	init_spi_interface();

	uint8_t uid[4];
	uint8_t conf[4];
	uint8_t lock[4];
	uint8_t eepromWrite=0;
	uint8_t modeReg;

	if(counter==0){

		uint8_t conf_word[] = { 0x26, 0x7E, 0x8f, 0x80 }; //{ 0x20, 0x7E, 0xE7, 0x80 } 14443-4

		spi_cs_clr(); clockTest_short();
			as3953_eeprom_read(0,uid,4);
		spi_cs_set();

		spi_cs_clr(); clockTest_short();
			as3953_eeprom_read(3,lock,4);
		spi_cs_set();

		spi_cs_clr(); clockTest_short();
			as3953_eeprom_read(2,conf,4);
		spi_cs_set();

		if(conf[0]!=conf_word[0] || conf[1]!=conf_word[1] || conf[2]!=conf_word[2] || conf[3]!=conf_word[3]){
			spi_cs_clr(); clockTest_short();
				as3953_eeprom_write(2, conf_word, 4);
			spi_cs_set(); clockTest_short();

			spi_cs_clr(); clockTest_short();
				as3953_eeprom_read(2, conf, 4);
			spi_cs_set();

			eepromWrite=1;
		}

		/*spi_cs_clr(); clockTest_short();
			send_NfcController(0x01,0x1F);
		spi_cs_set();*/

		spi_cs_clr(); clockTest_short();
			modeReg = read_NfcController(0x01);
		spi_cs_set();

	}

	//---------------
	//write fifo
	/*as3953_fifo_init(8<<3);

	uint8_t fifo[9] = {0xd1,0x01,0x05,0x54,0x74,0x65,0x73,0x74,0x31};

	spi_cs_clr(); clockTest_short();
		  as3953_fifo_write(fifo,9);
	spi_cs_set();

	spi_cs_clr(); clockTest_short();
		as3953_command(0xC8);	//transmit
	spi_cs_set();*/
	//-------------------
	//read fifo
	uint8_t fifoRead[32];
	spi_cs_clr(); clockTest_short();
		as3953_fifo_read(fifoRead, 32);
	spi_cs_set();
	//-------------------

	uint8_t regValues[0x12];
	for (int i=0;i<0x12;i++){
		spi_cs_clr(); clockTest_short();
			regValues[i] = read_NfcController(i);
		spi_cs_set();
	}

	for(int i=0;i<2;i++) clockTest();
	init_uart_interface();
	char buff[30];

	if (counter==0) {
		sprintf(buff,"UID: 0x%02x 0x%02x 0x%02x 0x%02x\n",uid[0],uid[1],uid[2],uid[3]);
		uart_sendText(buff);
		for(int i=0;i<2;i++) clockTest();

		sprintf(buff,"Conf: 0x%02x 0x%02x 0x%02x 0x%02x\n",conf[0],conf[1],conf[2],conf[3]);
		uart_sendText(buff);
		for(int i=0;i<2;i++) clockTest();

		sprintf(buff,"Lock: 0x%02x 0x%02x 0x%02x 0x%02x\n",lock[0],lock[1],lock[2],lock[3]);
		uart_sendText(buff);
		for(int i=0;i<2;i++) clockTest();

		if(eepromWrite==1){
			uart_sendText("data wrote to EEPROM\n");
			for(int i=0;i<2;i++) clockTest();
		}

		sprintf(buff,"ModeR:0x%02x\n",modeReg);
		uart_sendText(buff);
		for(int i=0;i<2;i++) clockTest();

		counter++;
	}
	for (int i=0;i<0x12;i++){
		if(regValues[i]!=0){
			sprintf(buff,"[0x%02x] 0x%02x\t",i,regValues[i]);
		}
		else{
			sprintf(buff,"[0x%02x] ____\t",i);
		}
		uart_sendText(buff);
	}

	uart_sendChar('\n');
	for (int i=0;i<32;i++){
		sprintf(buff,"0x%02x ",fifoRead[i]);
		uart_sendText(buff);
	}

	uart_sendChar('\n'); uart_sendChar('\n');
	for(int i=0;i<20;i++) clockTest();
}

void Test_AS3953()
{
	uint8_t uid[4];
	uint8_t conf[4];
	uint8_t lock[4];

	for (int i=0;i<2;i++){
		AS3953_Read_UID(uid);
		AS3953_Read_Lock(lock);
		AS3953_Read_Conf(conf);

		init_uart_interface();

		char buff[30];
		sprintf(buff,"\nuid: %d %d %d %d\n",uid[0],uid[1],uid[2],uid[3]);
		uart_sendText(buff);
		sprintf(buff,"conf: %d %d %d %d\n",conf[0],conf[1],conf[2],conf[3]);
		uart_sendText(buff);
		sprintf(buff,"lock: %d %d %d %d\n",lock[0],lock[1],lock[2],lock[3]);
		uart_sendText(buff);
		for (int i=0;i<20;i++) { clockTest_short(); }	//wait for the UART transmition to finished

		init_spi_interface();
	}
}

int main ()
{
	  CHIP_Init();
	  initOscillators();

	  /* UART test */
	  init_uart_interface();

	  /* Spi initialization */
	  init_spi_interface();

	  for (int i=0;i<20;i++) { clockTest_short(); }	//long wait

	  /* AS3953 initialization */
	  AS3953_Setting_t AS3953_Setting;
	  AS3953_Setting.spi_cs_port=  gpioPortB;
	  AS3953_Setting.spi_cs_pin =  8;
	  AS3953_Setting.conf_word[0]=0x26;		//	{ 0x20, 0x7E, 0xE7, 0x80 } 14443-4		??
	  AS3953_Setting.conf_word[1]=0x7E;
	  AS3953_Setting.conf_word[2]=0x8f;
	  AS3953_Setting.conf_word[3]=0x80;
	  AS3953_Init(AS3953_Setting);

	  for (int i=0;i<20;i++) { clockTest_short(); }

	  Test_AS3953();

	  while(1){

		  //as3953_init();

		  //for(int i=0;i<10;i++) clockTest();

		  /*if(counter==0){
			  uint8_t fifo[4]={0xAA,0x42,0xFF,0xC2};

			  //as3953_fifo_init(32);

			  //spi_cs_clr(); clockTest_short();
			  //	  as3953_fifo_write(fifo,4);
			  //spi_cs_set(); clockTest_short();

			  //spi_cs_clr(); clockTest_short();
			  //	as3953_command(0xC8);	//clear fifo command
			  //spi_cs_set();

			  //spi_cs_clr(); clockTest_short();
			  //	  send_NfcController(0x08, 194);
			  //spi_cs_set(); clockTest_short();
		  }*/

		  /*enable_SPI();

		  uint8_t regValues[0x12];
		  for (int i=0;i<0x12;i++){
		  		spi_cs_clr(); clockTest_short();
		  			regValues[i] = read_NfcController(i);
		  		spi_cs_set();
		  }

		  initUART();
		  char buff[30];
		  for (int i=0;i<0x12;i++){
			  sprintf(buff,"R%d=%d\t",i,regValues[i]);
			  uart_sendText(buff);
		  }
		  uart_sendChar('\n'); uart_sendChar('\n');
		  for(int i=0;i<10;i++) clockTest();*/
	  }
}
