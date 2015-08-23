#ifndef __HM_11_H
#define __HM_11_H

#include "em_device.h"
#include <stdio.h>
#include "..\ucPeripheralDrivers\uart_connection.h"


#ifdef __cplusplus
extern "C" {
#endif

uint8_t HM11_Init(void);

/*send AT command to test the connection to HM11 module. Returns 1 if connection fails and 0 if the device is present */
uint8_t HM11_Test(void);

enum ADV_INT {
	ADV_INT_100m = 0,
	ADV_INT_152_5ms,
	ADV_INT_211_25ms,
	ADV_INT_318_75ms,
	ADV_INT_417_5ms,
	ADV_INT_546_25ms,
	ADV_INT_760ms,
	ADV_INT_852_5ms,
	ADV_INT_1022_5ms,
	ADV_INT_1285ms,
	ADV_INT_2000ms,
	ADV_INT_3000ms,
	ADV_INT_4000ms,
	ADV_INT_5000ms,
	ADV_INT_6000ms,
	ADV_INT_7000ms
};
uint8_t HM11_SetAdvertisingInterval(enum ADV_INT value);

uint8_t HM11_SetModuleName(char *name);

enum MOD_POWER {
	POWER_MINUS_23_dbm = 0,
	POWER_MINUS_6_dbm,
	POWER_0_dbm,
	POWER_PLUS_6_dbm
};
uint8_t HM11_SetModulePower(enum MOD_POWER value);

enum BAUD_RATE {
	BAUD_9600 = 0,
	BAUD_19200,
	BAUD_38400,
	BAUD_57600,
	BAUD_115200,
	BAUD_4800,
	BAUD_2400,
	BAUD_1200,
	BAUD_230400
};
uint8_t HM11_SetBaudRate(enum BAUD_RATE value);

uint8_t HM11_ResetModule();

uint8_t HM11_StartModule();

#ifdef __cplusplus
}
#endif

#endif
