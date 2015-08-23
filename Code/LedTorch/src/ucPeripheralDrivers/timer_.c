#include "timer_.h"
#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"


void Timer_Init(void)
{
    CMU_ClockEnable(cmuClock_TIMER1, true);

    /* Setup Timer Configuration for general purpose use */
    TIMER_Init_TypeDef timerGPInit =
    {
    .enable     = true,                 /* start timer upon configuration */
    .debugRun   = false,                /* run timer in debug mode */
    .prescale   = timerPrescale1,       /* set prescaler to /1 */
    .clkSel     = timerClkSelHFPerClk,  /* set clock source as HFPERCLK */
    .fallAction = timerInputActionNone, /* no action from inputs */
    .riseAction = timerInputActionNone, /* no action from inputs */
    .mode       = timerModeUp,          /* use up-count mode */
    .dmaClrAct  = false,                /* not using DMA */
    .quadModeX4 = false,                /* not using Quad Dec. mode */
    .oneShot    = false,                /* not using one shot mode */
    .sync       = false,                /* not synchronizing timer3 with other timers */
    };

    TIMER_TopSet(TIMER1, 100);           /* TODO set this */
    TIMER_CounterSet(TIMER1, 0);         /* start counter at 0 (up-count mode) */

    Timer_ClrIRQ();
    TIMER_IntEnable(TIMER1, TIMER_IF_OF); /* enable Timer1 overflow interrupt */

    TIMER_Init(TIMER1, &timerGPInit);     /* apply general purpose configuration to timer0 */
}

void Timer_EnableIRQ(void)
{
    //TIMER_Reset(TIMER1);
    NVIC_EnableIRQ(TIMER1_IRQn);
}

void Timer_DisableIRQ(void)
{
    NVIC_DisableIRQ(TIMER1_IRQn);
}

inline void Timer_ClrIRQ(void)
{
    TIMER_IntClear(TIMER1, TIMER_IF_OF);   /* clear interrupt source */
}
