#ifndef __RTC__H
#define __RTC__H

#include "em_device.h"
#include "em_i2c.h"
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

#define LFRCO_FREQUENCY   32768

void Rtc_Init(void);
void Rtc_DisableInt(void);
void Rtc_EnableInt(void);
void Rtc_SetTime(uint32_t);
void Rtc_ClearInt(void);

#ifdef __cplusplus
}
#endif

#endif
