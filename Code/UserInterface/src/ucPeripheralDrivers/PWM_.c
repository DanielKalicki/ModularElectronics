#include "PWM_.h"

#define PWM_NUMBER_OF_CC_CHANNELS	3

static uint32_t g_PWM_MainClockFrequency;
static uint32_t g_PWM_dutyCycle[PWM_NUMBER_OF_CC_CHANNELS];
static uint32_t g_PWM_Period;

void PWM_Init(PWM_Settings_t PWM_Settings)
{
	/* enable TIMER0 peripheral clock */
	CMU_ClockEnable(cmuClock_TIMER0, true);

	/* Setup Timer Channel Configuration for PWM */
	TIMER_InitCC_TypeDef TimerCCInit = {
		.eventCtrl  = timerEventEveryEdge,    /* this value will be ignored since we aren't using input capture */
		.edge       = timerEdgeNone,          /* this value will be ignored since we aren't using input capture */
		.prsSel     = timerPRSSELCh0,         /* this value will be ignored since we aren't using PRS */
		.cufoa      = timerOutputActionNone,  /* no action on underflow (up-count mode) */
		.cofoa      = timerOutputActionSet,   /* on overflow, we want the output to go high, but in PWM mode this should happen automatically */
		.cmoa       = timerOutputActionClear, /* on compare match, we want output to clear, but in PWM mode this should happen automatically */
		.mode       = timerCCModePWM,         /* set timer channel to run in PWM mode */
		.filter     = false,                  /* not using input, so don't need a filter */
		.prsInput   = false,                  /* not using PRS */
		.coist      = false,                  /* initial state for PWM is high when timer is enabled */
		.outInvert  = false,                  /* non-inverted output */
	};

	/* Setup Timer Configuration for PWM */
	TIMER_Init_TypeDef TimerPWMSetup =
	{
		.enable     = true,                 /* start timer upon configuration */
		.debugRun   = true,                 /* run timer in debug mode */
		.prescale   = timerPrescale1,       /* set prescaler to 1 */
		.clkSel     = timerClkSelHFPerClk,  /* set clock source as HFPERCLK */
		.fallAction = timerInputActionNone, /* no action from inputs */
		.riseAction = timerInputActionNone, /* no action from inputs */
		.mode       = timerModeUp,          /* use up-count mode */
		.dmaClrAct  = false,                /* not using DMA */
		.quadModeX4 = false,                /* not using Quad Dec. mode */
		.oneShot    = false,                /* not using one shot mode */
		.sync       = false,                /* not synchronizing timer3 with other timers */
	};

	/* by default */
	PWM_SetPeriod(1000ULL, 10000000ULL);

	TIMER_CounterSet(TIMER0, 0);                 /* start counter at 0 (up-count mode) */

	for (uint8_t CCx = 0; CCx < PWM_NUMBER_OF_CC_CHANNELS; CCx++)
	{
		PWM_SetDuty(CCx, 0);
		TIMER_InitCC(TIMER0, CCx, &TimerCCInit);       /* apply channel configuration to Timer0 channel 0 */
	}

	TIMER0->ROUTE = (PWM_Settings.location << _TIMER_ROUTE_LOCATION_SHIFT);
	if (PWM_Settings.CC0_enable == true)
	{
		TIMER0->ROUTE |= TIMER_ROUTE_CC0PEN;
	}
	if (PWM_Settings.CC1_enable == true)
	{
		TIMER0->ROUTE |= TIMER_ROUTE_CC1PEN;
	}
	if (PWM_Settings.CC2_enable == true)
	{
		TIMER0->ROUTE |= TIMER_ROUTE_CC2PEN;
	}

	TIMER_Init(TIMER0, &TimerPWMSetup);           /* apply PWM configuration to timer1 */
}

void PWM_TurnOff(PWM_CC_Channel CCx)
{
	TIMER_CompareSet(TIMER0, CCx, 0);
	TIMER_CompareBufSet(TIMER0, CCx, 0);
	g_PWM_dutyCycle[CCx] = 0;
}

void PWM_SetDuty(PWM_CC_Channel CCx, uint32_t duty_miliPercents)
{
	uint32_t duty = (uint32_t)(((uint64_t)g_PWM_Period * (uint64_t)duty_miliPercents) / 10000ULL);
	TIMER_CompareSet(TIMER0, CCx, 0);
	TIMER_CompareBufSet(TIMER0, CCx, duty);
	g_PWM_dutyCycle[CCx] = duty;
}

void PWM_SetPeriod(uint32_t period, uint32_t clockFreq)
{
	uint32_t TopValue = clockFreq / period;
	TIMER_TopSet(TIMER0, TopValue);
	g_PWM_MainClockFrequency = clockFreq;
	g_PWM_Period = period;
}

