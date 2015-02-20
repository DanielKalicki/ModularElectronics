#ifndef __RTC__H
#define __RTC__H

#include "em_device.h"
#include "em_i2c.h"
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

#define LFRCO_FREQUENCY             	32768
#define WAKEUP_INTERVAL_MS            	490
#define RTC_COUNT_BETWEEN_WAKEUP    	(((LFRCO_FREQUENCY * WAKEUP_INTERVAL_MS) / 1000)-1)
#define WAKEUP_INTERVAL_MS_RX          	500
#define RTC_COUNT_BETWEEN_WAKEUP_RX    	(((LFRCO_FREQUENCY * WAKEUP_INTERVAL_MS_RX) / 1000)-1)


void setupRtc(void);

#ifdef __cplusplus
}
#endif

#endif
