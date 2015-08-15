
#include "FPGMMOPA6C.h"
#include "..\ucPeripheralDrivers\uart_connection.h"

static FPGMMOPA6C_Settings_t gpsPinsSettings;

void FPGMMOPA6C_Init(FPGMMOPA6C_Settings_t gpsSettings)
{
	GPIO_PinModeSet(gpsSettings.fix3d_port, gpsSettings.fix3d_pin, gpioModeInput, 1);
	GPIO_PinModeSet(gpsSettings.pps_port, gpsSettings.pps_pin, gpioModeInput, 1);
	gpsPinsSettings = gpsSettings;
}

FPGMMOPA6C_Fix_Result_t FPGMMOPA6C_GetFix()
{
	/* The GPS module outputs 1s high and 1s low pulses to indicate that
	 * the module is not fixed.
	 * Continuous low level signal indicates that the module is fixed. */

	/* TODO This function is not implemented correctly, the pulse detection
	 * is quite tricky and the fix can be read from the data so this function
	 * is left unimplemented */
	return FIX_OK;
}


uint8_t Uart_RxValidate_GpsMessage(void)
{
	if (	   ('K' == rxBuff.data[rxBuff.wrI-8])
			&& (',' == rxBuff.data[rxBuff.wrI-7])
			&& ('N' == rxBuff.data[rxBuff.wrI-6])
			&& ('*' == rxBuff.data[rxBuff.wrI-5])
			&& ('\r' == rxBuff.data[rxBuff.wrI-2])
			&& ('\n' == rxBuff.data[rxBuff.wrI-1])
	   )
	{
		return UART_RX_VALIDATE_RESULT_FINISHED;
	}
	else
	{
		return UART_RX_VALIDATE_RESULT_NOT_FINISHED;
	}
}
