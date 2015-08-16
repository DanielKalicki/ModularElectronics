#include "em_device.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_i2c.h"
#include "em_emu.h"

#define DEBUG

/* TODO remove i2c connection lib */
#include "ucPeripheralDrivers\i2c_connection.h"
#include "ucPeripheralDrivers\leuart_connection.h"
#include "ucPeripheralDrivers\RTC_.h"
#include "em_rtc.h"

/* #include "icDrivers\ADP8866.h" */
/* #include "icDrivers\AD7147.h" */
#include "icDrivers\BD6210.h"

uint8_t g_Devices = 0;
#define ADP8866_SENS	(0x01)
#define AD7147_SENS		(0x02)
#define BD6210_SENS		(0x04)

void initOscillators(void)
{
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);  // select HFCORECLK/2 as clock source to LFB
	CMU_ClockEnable(cmuClock_CORELE, true);                  // enable the Low Energy Peripheral Interface clock

	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);                    // enable GPIO peripheral clock
}

void initGPIO(void)
{
	; /* Nothing to do */
}

void clockTest_short()
{
	long int i = 0;
	for(; i < 121L; ++i)
	{
	  if(i == 120L)
	  {
		  GPIO_PortOutSet(gpioPortF, 0x4);
	  }
	  if(i == 60L)
	  {
		  GPIO_PortOutClear(gpioPortF, 0x4);
	  }
	}
}

void initDevices(void)
{
	/* -----ADP8866---- */
	if(g_Devices & ADP8866_SENS)
	{
		//ADP8866_Init();
	}
	/* -----AD7147---- */
	if(g_Devices & AD7147_SENS)
	{
		//AD7147_Init();
	}
	/* -----BD6210---- */
	if(g_Devices & BD6210_SENS)
	{
		BD6210_Settings_t BD6210_Settings;
		BD6210_Settings.RIN_port = gpioPortF;
		BD6210_Settings.RIN_pin = 2;
		BD6210_Settings.RIN_CCx_Number = 2;
		BD6210_Settings.FIN_port = gpioPortF;
		BD6210_Settings.FIN_pin = 1;
		BD6210_Settings.FIN_CCx_Number = 1;
		BD6210_Settings.timer_location = 5;
		BD6210_Init(BD6210_Settings);
	}
	clockTest_short();
}

/* -------------------------------------------------------------*/
/*							  RTC Timer 						*/
/* -------------------------------------------------------------*/
void RTC_IRQHandler(void)
{

	static uint8_t speed = 0;
	BD6210_SetSpeed(BD6210_FORWARD_DIR, speed);
	speed += 20;
	if (speed > 80)
	{
		speed = 0;
	}

	clockTest_short();clockTest_short();clockTest_short();clockTest_short();clockTest_short();clockTest_short();clockTest_short();

	/* Clear interrupt source */
	RTC_clearInt();
}

void detectDevices()
{
	g_Devices = 0x00;

	/* -----ADP8866---- */
	/*if (ADP8866_Detect() == 1)
	{
		#ifdef DEBUG
		LeUart_SendText("\t\tADP8866 detected\n");
		#endif
		g_Devices |= ADP8866_SENS;
	}
	else
	{
		#ifdef DEBUG
		LeUart_SendText("\t---\tADP8866 NOT DETECTED\t---\t\n");
		#endif
	}*/

	/* -----AD7147---- */
	/*if (AD7147_Detect() == 1)
	{
		#ifdef DEBUG
		LeUart_SendText("\t\tAD7147 detected\n");
		#endif
		g_Devices |= AD7147_SENS;
	}
	else
	{
		#ifdef DEBUG
		LeUart_SendText("\t---\tAD7147 NOT DETECTED\t---\t\n");
		#endif
	}*/
	/* -----BD6210---- */
	if (BD6210_Detect() == 1)
	{
		#ifdef DEBUG
		LeUart_SendText("\t\tBD6210 detected\n");
		#endif
		g_Devices |= BD6210_SENS;
	}
	else
	{
		#ifdef DEBUG
		LeUart_SendText("\t---\tBD6210 NOT DETECTED\t---\t\n");
		#endif
	}
}

void initInterfaces(void)
{
#ifdef DEBUG
	/* LeUart interface initialization */
	struct LeUart_Settings leuartSettings;
	leuartSettings.leuart_com_port = gpioPortF;
	leuartSettings.leuart_tx_pin = 0;
	leuartSettings.leuart_rx_pin = 1;
	leuartSettings.leuart_port_location = 3;
	leuartSettings.leuart_baudrate = 115200;
	LeUart_Init(leuartSettings);
	LeUart_SendText("Startup\n");
#endif

	/* i2c initialization */
	struct I2C_Settings i2cSettings;
	i2cSettings.i2c_SCL_port = gpioPortD;
	i2cSettings.i2c_SCL_pin =  7;
	i2cSettings.i2c_SDA_port = gpioPortD;
	i2cSettings.i2c_SDA_pin =  6;
	i2cSettings.i2c_port_location =  1;
	i2c_InitMaster(i2cSettings);
	i2c_Scan(I2C0);
}

int main(void)
{
	CHIP_Init();

	initOscillators();
	initGPIO();

	initInterfaces();

	detectDevices();
	LeUart_SendText("\nSTARTUP\n");
	initDevices();

	/* RTC initialization */
	RTC_init();
	RTC_setTime(50);
	RTC_enableInt();

	/* Infinite loop */
	while (1)
	{
	   /* Forever enter EM2. The RTC or I2C will wake up the EFM32 */
	   //EMU_EnterEM2(false);
	}
}
