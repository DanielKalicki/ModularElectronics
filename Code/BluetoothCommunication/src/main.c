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

//-------------RTC--------------
void RTC_IRQHandler(void){
	static uint16_t counter=0;

	counter++;
	if(counter>32000) counter=0;

	char text[30];
	sprintf(text,"Test: %d",counter);
	uart_sendText(text);

	//test EM1 sleep level.
}

//-------------MAIN-------------
int main(void)
{
  CHIP_Init();
  initOscillators();

  //initI2C();
  initUART();

  uint8_t initHM11_fail=1;
  while(initHM11_fail){
	  initHM11_fail=initHM11();
	  testWait();
	  uart_sendText("AT+START?");
	  testWait();
  }

  /* Setting up rtc */
  setupRtc();

  while(1){
	  //EMU_EnterEM1();
  }
}
