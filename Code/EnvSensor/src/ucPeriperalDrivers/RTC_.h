#ifndef __RTC__H
#define __RTC__H

#include "em_device.h"
#include "em_i2c.h"
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

#define LFRCO_FREQUENCY   32768

void RTC_init(void);
void RTC_disableInt(void);
void RTC_enableInt(void);
void RTC_setTime(uint32_t);
void RTC_clearInt(void);

#ifdef __cplusplus
}
#endif

#endif
