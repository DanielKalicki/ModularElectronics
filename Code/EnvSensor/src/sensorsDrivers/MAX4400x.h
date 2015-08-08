#ifndef MAX4400X_H_
#define MAX4400X_H_

uint8_t MAX4400x_Detect(void);

typedef struct
{
	GPIO_Port_TypeDef irq_port;
	unsigned int irq_pin;
} MAX4400x_Settings_t;
uint8_t MAX4400x_Init(MAX4400x_Settings_t MAX4400x_Settings);

#endif /* MAX4400X_H_ */
