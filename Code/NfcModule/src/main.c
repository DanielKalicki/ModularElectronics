#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "uart_connection.h"
#include "spi.h"
#include <stdio.h>

#define SPI_PORTC     gpioPortC // USART1 (location #0) MISO and MOSI are on PORTD
#define SPI_PORTB     gpioPortB // USART1 (location #0) SS and SCLK are on PORTC
#define SPI_MISO_PIN  1  // PC1
#define SPI_MOSI_PIN  0  // PC0
#define SPI_CS_PIN    8  // PB8
#define SPI_SCLK_PIN  7  // PB7

/* SPI functions */
void CS_pin_clr(void);
void CS_pin_set(void);

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

void initOscillators(void){
	CMU_ClockEnable(cmuClock_HFPER, true);

	CMU_ClockEnable(cmuClock_GPIO, true);                   // enable GPIO peripheral clock
	CMU_ClockEnable(cmuClock_I2C0,true);

	/* Enabling clock to USART 1 and 2*/
	CMU_ClockEnable(cmuClock_USART1, true);
}

void send_NfcController(uint8_t reg, uint8_t data){

	//send reg address
	USART_SpiTransfer(USART1, reg&0x1F);
	//send data
	USART_SpiTransfer(USART1, data);
}

uint8_t read_NfcController(uint8_t reg){

	uint8_t rxData[2];

	rxData[0] = USART_SpiTransfer(USART1, (reg&0x1F)+0x20);
	rxData[1] = USART_SpiTransfer(USART1, 0x00);

	return rxData[1];
}

void as3953_command(uint8_t cmd){
	USART_SpiTransfer(USART1,0xC0|cmd);
}

void as3953_fifo_init(uint16_t bits){
	bits=bits&0x1FFF;
	if(bits & 0x07) bits+=8;

	CS_pin_clr(); clockTest_short();
		as3953_command(0xC4);	//clear fifo command
	CS_pin_set(); clockTest_short();

	CS_pin_clr(); clockTest_short();
		send_NfcController(0x10,(bits>>8)&0xFF);
	CS_pin_set();

	CS_pin_clr(); clockTest_short();
		send_NfcController(0x11,bits&0xFF);	//mask it TODO
	CS_pin_set();

}

void as3953_fifo_write(uint8_t* data, uint8_t len){
	USART_SpiTransfer(USART1, 0x80);	//mask it fifo write

	for (int i=0;i<len;i++){
		USART_SpiTransfer(USART1, data[i]);
	}
}

void as3953_fifo_read(uint8_t* data, uint8_t len){
	USART_SpiTransfer(USART1, 0xBF);
	for (int i=0;i<len;i++){
		data[i]=USART_SpiTransfer(USART1,0x00);
	}
}

void as3953_eeprom_write(uint8_t word, uint8_t* data, uint8_t len){

	//TODO protect agains writting to lock bits.

	USART_SpiTransfer(USART1, 0x40);//mask it eeprom write
	USART_SpiTransfer(USART1, ((word & 0x1F) << 1));

	for (int i=0;i<len;i++){
		USART_SpiTransfer(USART1, data[i]);
	}
}

uint8_t as3953_eeprom_read(uint8_t addr, uint8_t* data, uint8_t len){
	if(addr>0x1F) 			return 1;
	//if (!len || len % 4) 	return 1;

	USART_SpiTransfer(USART1, 0x7F);	//mask it
	USART_SpiTransfer(USART1, addr<<1);

	for (int i=0;i<len;i++){
		data[i] = USART_SpiTransfer(USART1, 0x00);
	}
	return 0;
}

USART_InitSync_TypeDef spi_init;

void enable_SPI(){
	USART_IntClear(USART1, 0x1FF9);                // clear interrupt flags (optional)
	USART_InitSync(USART1, &spi_init);              // apply configuration to USART1
	USART1->ROUTE = (0 << 8) | ( 1 << 3)| (3 << 0); // use location #3, enable TX/RX/CLK pins
	USART_Enable(USART1, usartEnable);
}

void as3953_init(void){

	static uint8_t counter=0;

	enable_SPI();

	uint8_t uid[4];
	uint8_t conf[4];
	uint8_t lock[4];
	uint8_t eepromWrite=0;
	uint8_t modeReg;

	if(counter==0){

		uint8_t conf_word[] = { 0x26, 0x7E, 0x8f, 0x80 }; //{ 0x20, 0x7E, 0xE7, 0x80 } 14443-4

		CS_pin_clr(); clockTest_short();
			as3953_eeprom_read(0,uid,4);
		CS_pin_set();

		CS_pin_clr(); clockTest_short();
			as3953_eeprom_read(3,lock,4);
		CS_pin_set();

		CS_pin_clr(); clockTest_short();
			as3953_eeprom_read(2,conf,4);
		CS_pin_set();

		if(conf[0]!=conf_word[0] || conf[1]!=conf_word[1] || conf[2]!=conf_word[2] || conf[3]!=conf_word[3]){
			CS_pin_clr(); clockTest_short();
				as3953_eeprom_write(2, conf_word, 4);
			CS_pin_set(); clockTest_short();

			CS_pin_clr(); clockTest_short();
				as3953_eeprom_read(2,conf,4);
			CS_pin_set();

			eepromWrite=1;
		}

		/*CS_pin_clr(); clockTest_short();
			send_NfcController(0x01,0x1F);
		CS_pin_set();*/

		CS_pin_clr(); clockTest_short();
			modeReg = read_NfcController(0x01);
		CS_pin_set();

	}

	//---------------
	//write fifo
	/*as3953_fifo_init(8<<3);

	uint8_t fifo[9] = {0xd1,0x01,0x05,0x54,0x74,0x65,0x73,0x74,0x31};

	CS_pin_clr(); clockTest_short();
		  as3953_fifo_write(fifo,9);
	CS_pin_set();

	CS_pin_clr(); clockTest_short();
		as3953_command(0xC8);	//transmit
	CS_pin_set();*/
	//-------------------
	//read fifo
	uint8_t fifoRead[32];
	CS_pin_clr(); clockTest_short();
		as3953_fifo_read(fifoRead,32);
	CS_pin_set();
	//-------------------

	uint8_t regValues[0x12];
	for (int i=0;i<0x12;i++){
		CS_pin_clr(); clockTest_short();
			regValues[i] = read_NfcController(i);
		CS_pin_set();
	}

	for(int i=0;i<2;i++) clockTest();
	initUART();
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

int main () {

	  CHIP_Init();
	  uint8_t counter=0;

	  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);  // select HFCORECLK/2 as clock source to LFB
	  CMU_ClockEnable(cmuClock_CORELE, true);                  // enable the Low Energy Peripheral Interface clock

	  CMU_ClockEnable(cmuClock_HFPER, true);
	  CMU_ClockEnable(cmuClock_GPIO, true);                    // enable GPIO peripheral clock
	  CMU_ClockEnable(cmuClock_USART1, true);                  // enable USART1 peripheral clock

	  /* Configure GPIO */
	  GPIO_PinModeSet(SPI_PORTC, SPI_MISO_PIN, gpioModeInput, 1);      // configure MISO pin as input, no filter
	  GPIO_PinModeSet(SPI_PORTC, SPI_MOSI_PIN, gpioModePushPull, 1);    // configure MOSI pin as output, initialize high
	  GPIO_PinModeSet(SPI_PORTB, SPI_CS_PIN, gpioModePushPull, 1);      // configure CS pin as output, initialize high
	  GPIO_PinModeSet(SPI_PORTB, SPI_SCLK_PIN, gpioModePushPull, 0);    // configure SCLK pin as output, initialize low

	  /* Configure SPI Port (USART1 in sync mode) */
	  spi_init.enable = usartEnable;        // enable bidirectional data (TX and RX)
	  spi_init.refFreq = 0;                // measure source clock
	  spi_init.baudrate = 100000;        // 12Mbps is max data rate with 24MHz clock source
	  spi_init.databits = usartDatabits8;  // 8 data bits per frame
	  spi_init.master = true;              // configure as SPI master
	  spi_init.msbf = true;                // transmit msb first
	  spi_init.clockMode = usartClockMode1; // clock idles low, data setup on rising edge, sampled on falling edge

	  enable_SPI();
	  CS_pin_set(); for(int i=0;i<10;i++) clockTest();

	  as3953_init();

	  while(1){

		  as3953_init();

		  for(int i=0;i<10;i++) clockTest();

		  /*if(counter==0){
			  uint8_t fifo[4]={0xAA,0x42,0xFF,0xC2};

			  //as3953_fifo_init(32);

			  //CS_pin_clr(); clockTest_short();
			  //	  as3953_fifo_write(fifo,4);
			  //CS_pin_set(); clockTest_short();

			  //CS_pin_clr(); clockTest_short();
			  //	as3953_command(0xC8);	//clear fifo command
			  //CS_pin_set();

			  //CS_pin_clr(); clockTest_short();
			  //	  send_NfcController(0x08, 194);
			  //CS_pin_set(); clockTest_short();
		  }*/

		  /*enable_SPI();

		  uint8_t regValues[0x12];
		  for (int i=0;i<0x12;i++){
		  		CS_pin_clr(); clockTest_short();
		  			regValues[i] = read_NfcController(i);
		  		CS_pin_set();
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

// This function drives the CS pin low
void CS_pin_clr(void) {
  GPIO_PinOutClear(SPI_PORTB, SPI_CS_PIN);
}

// This function drives the CS pin high
void CS_pin_set(void) {
  GPIO_PinOutSet(SPI_PORTB, SPI_CS_PIN);
}
