#include "RTC_.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_rtc.h"
#include "em_cmu.h"

void startLfxoForRtc(void)
{
  /* Starting LFRCO and waiting until it is stable */
  CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);

  /* Routing the LFRCO clock to the RTC */
  CMU_ClockSelectSet(cmuClock_LFA,cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_RTC, true);

  /* Enabling clock to the interface of the low energy modules */
  CMU_ClockEnable(cmuClock_CORELE, true);
}
void RTC_init(void)
{
  /* Configuring clocks in the Clock Management Unit (CMU) */
  startLfxoForRtc();

  RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;

  rtcInit.enable   = true;      /* Enable RTC after init has run */
  rtcInit.comp0Top = true;      /* Clear counter on compare match */
  rtcInit.debugRun = false;     /* Counter shall keep running during debug halt. */

  /* Initialize the RTC */
  RTC_Init(&rtcInit);
}

void RTC_setTime(uint32_t time_ms){
	/* Setting the compare value of the RTC */
	RTC_CompareSet(0, (((LFRCO_FREQUENCY * time_ms) / 1000)-1));
}

void RTC_disableInt(void){
	 RTC_IntDisable(RTC_IFC_COMP0);
	 NVIC_DisableIRQ(RTC_IRQn);
}
void RTC_enableInt(void){
	 RTC_IntEnable(RTC_IFC_COMP0);
	 NVIC_EnableIRQ(RTC_IRQn);
}

void RTC_clearInt(void){
	RTC_IntClear(RTC_IFC_COMP0);
}
