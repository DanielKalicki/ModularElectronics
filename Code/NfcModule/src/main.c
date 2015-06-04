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

#define RX_BUFFER_SIZE 2

void print_byte_setup(uint8_t byte);

/* SPI functions */
void CS_pin_clr(void);
void CS_pin_set(void);

/* Global variables */
const char header[] = "\n\rManuf/Device ID\n\r";
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t print_byte_array[4] = { 0x30, 0x78, 0x20, 0x20 }; // array for displaying bytes one nibble at a time

void clockTest() {
	long int i=0;
	for(;i<120001L;++i) {
	  if(i==120000L)
		  GPIO_PortOutSet(gpioPortF, 0x4);
	  if(i==60000L)
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

	//send reg addres
	USART_SpiTransfer(USART1, reg&0x1F);

	//send data
	USART_SpiTransfer(USART1, data);
}

uint8_t read_NfcController(uint8_t reg){

	uint8_t rxData=0;
	USART_SpiTransfer(USART1, (reg&0x1F)+0x20);
	rxData = USART_SpiTransfer(USART1, 0xAA);

	return rxData;
}

int main () {


	  CHIP_Init();
	  uint16_t i;

	  //initOscillators();


	  // Clear software buffer (optional)
	  for(i=0; i<RX_BUFFER_SIZE; i++) {
	    rx_buffer[i] = 0;
	  }


	  /* Setup clock tree */
	  /* Use 24MHz crystal as HF system clock source */
	  /* Run Low Energy Peripheral B system as a high speed peripheral (use CORECLCK/2 as source) */
	  //CMU_OscillatorEnable(cmuOsc_HFXO, true,true);          // enable XTAL osc and wait for it to stabilize
	  //CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);        // select HF XTAL osc as system clock source (24MHz)
	  /*if(cmuSelect_HFRCO != CMU_ClockSelectGet(cmuClock_HF)) { // make sure HFRCO is no longer the selected clock source
	    CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);      // disable HFRCO to save power
	  }*/
	  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);  // select HFCORECLK/2 as clock source to LFB
	  CMU_ClockEnable(cmuClock_CORELE, true);                  // enable the Low Energy Peripheral Interface clock

	  CMU_ClockEnable(cmuClock_HFPER, true);
	  CMU_ClockEnable(cmuClock_GPIO, true);                    // enable GPIO peripheral clock
	  CMU_ClockEnable(cmuClock_USART1, true);                  // enable USART1 peripheral clock

	  /* Configure GPIO */
	  GPIO_PinModeSet(SPI_PORTC, SPI_MISO_PIN, gpioModeInput, 0);      // configure MISO pin as input, no filter
	  GPIO_PinModeSet(SPI_PORTC, SPI_MOSI_PIN, gpioModePushPull, 1);    // configure MOSI pin as output, initialize high
	  GPIO_PinModeSet(SPI_PORTB, SPI_CS_PIN, gpioModePushPull, 1);      // configure CS pin as output, initialize high
	  GPIO_PinModeSet(SPI_PORTB, SPI_SCLK_PIN, gpioModePushPull, 0);    // configure SCLK pin as output, initialize low

	  /* Configure SPI Port (USART1 in sync mode) */
	  USART_InitSync_TypeDef spi_init = {
	    .enable = usartEnable,        // enable bidirectional data (TX and RX)
	    .refFreq = 0,                // measure source clock
	    .baudrate = 1000000,        // 12Mbps is max data rate with 24MHz clock source
	    .databits = usartDatabits8,  // 8 data bits per frame
	    .master = true,              // configure as SPI master
	    .msbf = true,                // transmit msb first
	    .clockMode = usartClockMode0, // clock idles low, data setup on rising edge, sampled on falling edge
	  };

	  uint8_t counter=0;

	  while(1){
		  USART_IntClear(USART1, 0x1FF9);                // clear interrupt flags (optional)
		  USART_InitSync(USART1, &spi_init);              // apply configuration to USART1
		  USART1->ROUTE = (0 << 8) | ( 1 << 3)| (3 << 0); // use location #3, enable TX/RX/CLK pins
		  USART_Enable(USART1, usartEnable);

		  CS_pin_clr();
		  send_NfcController(0x00, 0x00);
		  send_NfcController(0x01, 0x00);	//ISO1443A Level-4

		  uint8_t rxData = read_NfcController(counter);
		  counter++;
		  if(counter==0x12) counter=0;

		  CS_pin_set();

		  for(int i=0;i<10;i++) clockTest();

		  initUART();
		  char buff[30];
		  sprintf(buff,"R%d: %d\n",counter,rxData);
		  uart_sendText(buff);
		  for(int i=0;i<10;i++) clockTest();
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
