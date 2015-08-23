#ifndef PWM__H_
#define PWM__H_

#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"

typedef struct{
	GPIO_Port_TypeDef CC0_port;
	unsigned int CC0_pin;
	_Bool CC0_enable;

	GPIO_Port_TypeDef CC1_port;
	unsigned int CC1_pin;
	_Bool CC1_enable;

	GPIO_Port_TypeDef CC2_port;
	unsigned int CC2_pin;
	_Bool CC2_enable;

	uint8_t location;
} PWM_Settings_t;

void PWM_Init(PWM_Settings_t PWM_Settings);

typedef enum{
	PWM_CC0	= 0,
	PWM_CC1	= 1,
	PWM_CC2	= 2
} PWM_CC_Channel;

void PWM_TurnOff(PWM_CC_Channel CCx);
/* The duty cycle should be enter in miliPercents for example 90% = 90000, 12.345% = 12345 */
void PWM_SetDuty(PWM_CC_Channel ccChannel, uint32_t duty_miliPercents);
void PWM_SetPeriod(uint32_t period, uint32_t clockFreq);

#endif /* PWM__H_ */
