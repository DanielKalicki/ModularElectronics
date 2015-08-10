#include "MAX4400x.h"
#include "MAX4400x\MAX4400x_reg.h"
#ifdef DEBUG
#include "..\ucPeripheralDrivers\leuart_connection.h"
#endif

uint8_t MAX4400x_Detect(void)
{
	return (MAX4400x_Detect_LowLevel());
}

uint8_t MAX4400x_Init(MAX4400x_Settings_t MAX4400x_Settings)
{
	/*--------------------------------*/
	/*        IRQ configuration       */
	/*--------------------------------*/
	/* By setting the mode to gpioModeInput its value can be read */
	GPIO_PinModeSet(MAX4400x_Settings.irq_port, MAX4400x_Settings.irq_pin, gpioModeInput, 1);
	if ((MAX4400x_Settings.irq_pin & 0x01) == 0)
	{
		/* Enable GPIO_EVEN interrupt in NVIC */
		NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	}
	else
	{
		/* Enable GPIO_ODD interrupt in NVIC */
		NVIC_EnableIRQ(GPIO_ODD_IRQn);
	}
	/* Configure interrupt on falling edge by default */
	GPIO_IntConfig(MAX4400x_Settings.irq_port, MAX4400x_Settings.irq_pin, false, true, true);


		/*----------------------------------*/
		/*      MAX4400x configuration      */
		/*----------------------------------*/
}
