#include "em_device.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_i2c.h"
#include "em_emu.h"

/* TODO remove i2c connection lib */
#include "ucPeripheralDrivers\i2c_connection.h"
#include "ucPeripheralDrivers\leuart_connection.h"
#include "ucPeripheralDrivers\RTC_.h"
#include "em_rtc.h"

#include "icDrivers\ADP5063.h"
#include "icDrivers\LTC2942.h"

#define REG_DATA_LENGTH_VALUE 22

enum registerMap{

	REG_DATA_LENGTH				=0,

	REG_MODULE_ID_1				=1,
	REG_MODULE_ID_2				=2,
	REG_MODULE_ID_3				=3,

	REG_I2C_ADDR				=4,

	REG_DEVICES					=5,

	REG_COMMAND_CODE			=7,		//this registers are used to send commands to the module
	REG_COMMAND_DATA_1			=8,
	REG_COMMAND_DATA_2			=9,

	REG_BAT_VOLT_1				=10,
	REG_BAT_VOLT_2				=11,
	REG_BAT_TEMP_1				=12,
	REG_BAT_TEMP_2				=13,
	REG_BAT_CHRG_1				=14,
	REG_BAT_CHRG_2				=15,
	REG_BAT_DISCHR_1			=16,
	REG_BAT_DISCHR_2			=17,

	REG_ADP5063_CHARGER_ST_1	=20,
	REG_ADP5063_CHARGER_ST_2	=21,

};

uint8_t g_Devices = 0;

#define ADP5063_SENS	(0x01)
#define LTC2942_SENS	(0x02)

//-------------INIT--------------
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

void clockTest();

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
	//-----ADP5063----
	if(g_Devices & ADP5063_SENS)
	{
		ADP5063_Init();
	}
	//-----LTC2942----
	if(g_Devices & LTC2942_SENS)
	{
		LTC2942_Init();
	}
	clockTest_short();
}

#define LTC2942_READ_TEMP

/* -------------------------------------------------------------*/
/*							  RTC Timer 						*/
/* -------------------------------------------------------------*/
uint8_t g_TimeInterruptType = 0;
void RTC_IRQHandler(void)
{
	char buff[30];

	g_TimeInterruptType++;
	/* every 5seconds force battery voltage and temperature measurement */
	if(g_TimeInterruptType == 5)
	{
		if(g_Devices & LTC2942_SENS)
		{
			/* force single battery voltage measurement */
			LTC2942_ForceSingleVoltageMeasurement();
		}
		g_TimeInterruptType = 0;
		if(g_Devices & ADP5063_SENS)
		{
			/* every 5 seconds we check the battery charger */
			ADP5063_PrintStatus();
		}
	}
	/* read battery voltage and temperature */
	else if (g_TimeInterruptType == 1)
	{
		if(g_Devices & LTC2942_SENS)
		{
			/* calculate battery voltage */
			uint16_t voltage_U16 = LTC2942_GetVoltage();
			uint32_t batteryVoltage=(uint32_t)60000*voltage_U16/65535;		//battery voltage in 0.1mV

			i2c_registers[REG_BAT_VOLT_1] = (uint8_t)((voltage_U16 >> 8) & 0xFF);
			i2c_registers[REG_BAT_VOLT_2] = (uint8_t)voltage_U16;

#ifdef DEBUG
			sprintf(buff,"BAT %ldmV\t",batteryVoltage/10);
			LeUart_SendText(buff);
#endif

			if(g_Devices & ADP5063_SENS)
			{
				/* if the voltage exides 4.2V stop charging */
				if(batteryVoltage / 10 > 4250)
				{
					ADP5063_StopCharging();
				}
			}

			#ifdef LTC2942_READ_TEMP
				//calculate battery module temperature
				uint16_t temperature_U16 = LTC2942_GetTemperature();
				/* battery module temperature in 0.1 C */
				uint32_t batteryTemp = ((uint32_t)6000 * temperature_U16 / 65535) - 2731;

				i2c_registers[REG_BAT_TEMP_1] = (uint8_t)((temperature_U16 >> 8) & 0xFF);
				i2c_registers[REG_BAT_TEMP_2] = (uint8_t)temperature_U16;

#ifdef DEBUG
				sprintf(buff," %ld[0.1C]\n",batteryTemp);
				LeUart_SendText(buff);
				/* force single temperature measurment */
				LTC2942_ForceSingleTemperatureMeasurement();
#endif
			#else
				#ifdef DEBUG
				LeUart_SendChar('\n');
				#endif
			#endif
		}
	}

	if(g_Devices & LTC2942_SENS){

		uint16_t chargeReg = LTC2942_GetCharger();
		uint32_t charge_U32 = (uint32_t)65535 - (uint32_t)chargeReg;

		i2c_registers[REG_BAT_CHRG_1] = (uint8_t)((chargeReg >> 8) & 0xFF);
		i2c_registers[REG_BAT_CHRG_2] = (uint8_t)chargeReg;

		uint16_t charge_U16 = (uint16_t)charge_U32;
		LTC2942_ChargeHistory_AddNewSample(charge_U16);

		if(LTC2942_ChargeHistory_IsFull())
		{
			/* assuming every measurment made in 500ms time steps -> the number 2 stands for it */
			uint32_t dischargeSpeed = (charge_U16 - LTC2942_ChargeHistory_GetOldestSample()) * 85 * 2 * 6 / LTC2942_ChargeHistory_GetBufferSize();
#ifdef DEBUG
			sprintf(buff," DS:%ld uAh/min\t", dischargeSpeed);
			LeUart_SendText(buff);
#endif
			i2c_registers[REG_BAT_DISCHR_1] = (uint8_t) (dischargeSpeed >> 8);
			i2c_registers[REG_BAT_DISCHR_2] = (uint8_t) dischargeSpeed;
			// (val16-ch[i]) * 85 is in 0.1uAh ===> [x]*2 -> is in 0.1uAh/second   ===> [x]*60 ===> 0.1uAh/min  ===> [x]/10 ===> 1uAh/min
		}

		//R_sense resistor is hard coded to 500mOhm
		//the equation is accumulated_charge * 0.085 [mAh] * 50mOhm/R_sense * M/128,   M-> prescaller setting
		charge_U32 *= 85;	//charge value in 0.1uAh
#ifdef DEBUG
		sprintf(buff, "Q:%ld uAh\n", charge_U32 / 10);
		LeUart_SendText(buff);
#endif
	}

	clockTest_short();clockTest_short();clockTest_short();clockTest_short();clockTest_short();clockTest_short();clockTest_short();

	/* Clear interrupt source */
	RTC_clearInt();
}

void detectDevices()
{
	g_Devices = 0xFF; //TODO remove this - this allows disconnection of the battery when the module is working.

	//-----ADP5063----
	if (ADP5063_Detect() == 1)
	{
		#ifdef DEBUG
		LeUart_SendText("\t\tADP5063 detected\n");
		#endif
		g_Devices |= ADP5063_SENS;
	}
	else
	{
		#ifdef DEBUG
		LeUart_SendText("\t---\tADP5063 NOT DETECTED\t---\t\n");
		#endif
	}

	//-----LTC2942----
	if (LTC2942_Detect() == 1)
	{
		#ifdef DEBUG
		LeUart_SendText("\t\tLTC2942 detected\n");
		#endif
		g_Devices |= LTC2942_SENS;
	}
	else
	{
		#ifdef DEBUG
		LeUart_SendText("\t---\tLTC2942 NOT DETECTED\t---\t\n");
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
	i2cSettings.i2c_SCL_port = gpioPortC;
	i2cSettings.i2c_SCL_pin =  1;
	i2cSettings.i2c_SDA_port = gpioPortC;
	i2cSettings.i2c_SDA_pin =  0;
	i2cSettings.i2c_port_location =  4;
	i2c_InitMaster(i2cSettings);
	i2c_Scan(I2C0);
}

//-------------MAIN-------------
int main(void)
{
  CHIP_Init();

  initOscillators();
  initGPIO();

  initInterfaces();

  /* wait for the voltage to stabilize */
  /*TODO how to do this properly? */
  for (int i = 0; i <500; i++){
	  clockTest_short();
  }

  /* TODO remove this later */
  for (int i = 0;i < 100; i++)
  {
	  i2c_registers[i] = 0;
  }

  i2c_registers[REG_DATA_LENGTH] = REG_DATA_LENGTH_VALUE;
  i2c_registers[REG_MODULE_ID_1] = 'L';
  i2c_registers[REG_MODULE_ID_2] = 'B';
  i2c_registers[REG_MODULE_ID_3] = 'C';
  //id: 4997699
  /* END of remove */

  detectDevices();
  LeUart_SendText("\nSTARTUP\n");
  initDevices();

  /* RTC initialization */
  RTC_init();
  RTC_setTime(500);
  RTC_enableInt();

  /* Infinite loop */
  while (1)
  {
	   /* Forever enter EM2. The RTC or I2C will wake up the EFM32 */
	  EMU_EnterEM2(false);
  }
}
