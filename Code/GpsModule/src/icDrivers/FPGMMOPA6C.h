#ifndef FPGMMOPA6C_H_
#define FPGMMOPA6C_H_

#include "em_gpio.h"

#define DEBUG

typedef struct
{
	GPIO_Port_TypeDef fix3d_port;
	unsigned int fix3d_pin;
	GPIO_Port_TypeDef pps_port;
	unsigned int pps_pin;
} FPGMMOPA6C_Settings_t;

void FPGMMOPA6C_Init(FPGMMOPA6C_Settings_t gpsSettings);

typedef enum
{
	FIX_ERROR = 0,
	FIX_OK = 1
} FPGMMOPA6C_Fix_Result_t;

FPGMMOPA6C_Fix_Result_t FPGMMOPA6C_GetFix();

uint8_t Uart_RxValidate_GpsMessage(void);

#endif /* FPGMMOPA6C_H_ */
