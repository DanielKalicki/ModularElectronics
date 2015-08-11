#include "ADP5063.h"
#include "..\ucPeripheralDrivers\i2c_connection.h"
#include "..\ucPeripheralDrivers\leuart_connection.h"

#define ADP5063_ADDR 0x14*2

/*             !!!				  */
/* TODO made this as a proper lib */
/*             !!!				  */

void ADP5063_Init(void)
{
	/* set VINx input current-limit to 100mA -> [3:0] 0000 */
	i2c_RegisterSet(I2C0, ADP5063_ADDR, 0x02, 0x00);
	/* Termination voltage set to 4.2V [7:2] 100011. Charging voltage limit set to 3.8V [1:0] 11 */
	i2c_RegisterSet(I2C0, ADP5063_ADDR, 0x03, 0x8F);
	/* Fast charge current set to 50mA [6:2] 00000. Trickle and weak charge current set to 5mA [1:0] 00 */
	i2c_RegisterSet(I2C0, ADP5063_ADDR, 0x04, 0x00);
	/* Recharge enabled [7] 0. Recharge voltage set to 260mV [6:5] 11. V_dead set to 2.9V [4:3] 11. Weak battery voltage set to 3.4V [2:0] 111 */
	i2c_RegisterSet(I2C0, ADP5063_ADDR, 0x05, 0xFF);
	/* Register 0x06, 0x08 and 0x10 set to default */
	/* termination current set to 12.5mA [7:5] 000. Other values are set to Default */
	i2c_RegisterSet(I2C0, ADP5063_ADDR, 0x11, 0x00);
}

uint8_t ADP5063_Detect(void)
{
	return i2c_Detect(I2C0, ADP5063_ADDR);
}

void ADP5063_PrintStatus(void)
{
	static uint8_t chargingCompleted = 0;
	uint8_t canStartCharging = 0;

	uint8_t val[2];
	//0x0B -> Charger status 1
	//0x0C -> Charger status 2
	i2c_Register_Read_Block(I2C0, ADP5063_ADDR, 0x0B, 2, val);

	/* TODO this should be store in some user accessible register */
	/*i2c_registers[REG_ADP5063_CHARGER_ST_1] = val[0];
	i2c_registers[REG_ADP5063_CHARGER_ST_2] = val[1];*/

#define VIN_OV	 	(1 << 7)
#define VIN_OK		(1 << 6)
#define V_ILIM		(1 << 5)
#define THER_LIM	(1 << 4)
#define CHDONE		(1 << 3)
#define CHARG_STAT	(0x07)

	if (val[0] & VIN_OV)
	{
		LeUart_SendText("Voltage at VINx exceeds V_VIN_OV\n");
	}
	if (val[0] & VIN_OK)
	{
		LeUart_SendText("Voltage at VINx exceeds V_VIN_OK_RISE and V_VIN_OK_FALL\n");
	}
	if (val[0] & V_ILIM)
	{
		LeUart_SendText("Current into VINx is limited by the high voltage blocking FET and the charger is not running at the full programmed I_CHG\n");
	}
	if (val[0] & THER_LIM)
	{
		LeUart_SendText("Charger is not running at the full programmed I_CHG but is limited by the die temperature\n");
	}
	if (val[0] & CHDONE)
	{
		LeUart_SendText("End of a charge cycle has been reached. This bit latches on, in that it does not reset to low when the V_RCH threshold is breached\n");
	}
	if (val[0] & CHARG_STAT)
	{
		if ((val[0] & CHARG_STAT) == 1)
		{
			LeUart_SendText("Charger is in trickle charge state\n");
		}
		else if ((val[0] & CHARG_STAT) == 2)
		{
			LeUart_SendText("Charger is in fast charge mode (CC mode)\n");
		}
		else if ((val[0] & CHARG_STAT) == 3)
		{
			LeUart_SendText("Charger is in fast charge mode (CV mode)\n");
		}
		else if ((val[0] & CHARG_STAT) == 4)
		{
			LeUart_SendText("Charge completed\n");
			if(chargingCompleted == 0)
			{
				LTC2942_ChargeHistory_ResetAccumulated();
				chargingCompleted = 1;
			}
		}
		else if ((val[0]&CHARG_STAT) == 5)
		{
			LeUart_SendText("LDO mode\n"); canStartCharging=1;
		}
		else if ((val[0]&CHARG_STAT) == 6)
		{
			LeUart_SendText("trickle or fast charge timer expired\n");
		}
		else if ((val[0]&CHARG_STAT) == 7)
		{
			LeUart_SendText("charger is set in battery detection mode\n");
		}

		if ((val[0]&CHARG_STAT) != 4) //if charge isnt completed reset the chargeCompleted variable
		{
			chargingCompleted = 0;
		}
	}
	else
	{
		LeUart_SendText("Charger is off\n");
	}

#define THR_STAT	0xE0
#define RCH_LIM		(1 << 3)
#define BATT_STAT	0x07

	if ((val[1] & THR_STAT))
	{
		LeUart_SendText("THR status: ");
	}
	if ((val[1] & THR_STAT) == 0x20)
	{
		LeUart_SendText("battery cold\n");
	}
	if ((val[1] & THR_STAT) == 0x40){
		LeUart_SendText("battery cool\n");
	}
	if ((val[1] & THR_STAT) == 0x60)
	{
		LeUart_SendText("battery warm\n");
	}
	if ((val[1] & THR_STAT) == 0x80)
	{
		LeUart_SendText("battery hot\n");
	}
	if ((val[1] & THR_STAT) == 0xE0)
	{
		LeUart_SendText("termistor OK\n");
	}

	if ((val[0]&CHARG_STAT) == 4)
	{	//recharge limit information function is active when CHARGER_STATUS = 100 (binary)
		if (val[1] & RCH_LIM)
		{
			LeUart_SendText("V_BAT_SNS < V_R_CH\n");
		}
		else
		{
			LeUart_SendText("V_BAT_SNS > V_R_CH\n");
		}
	}
	if (val[1] & BATT_STAT)
	{
		LeUart_SendText("Battery status: ");
	}
	if ((val[1] & BATT_STAT) == 1)
	{
		LeUart_SendText("No battery\n");
	}
	else if ((val[1] & BATT_STAT) == 2)
	{
		LeUart_SendText("V_BAT_SNS < V_TRK_DEAD\n");
	}
	else if ((val[1] & BATT_STAT) == 3)
	{
		LeUart_SendText("V_TRK_DEAD <= V_BAT_SNS < V_WEAK\n");
	}
	else if ((val[1] & BATT_STAT) == 4)
	{
		LeUart_SendText("V_BAT_SNS >= V_WEAK\n");
	}

	LeUart_SendText("\n\n");

	if (canStartCharging)
	{
		LeUart_SendText("\n\t\tStarting battery charging\n\n");
		ADP5063_Init();
		//Normal operation of the device. LDO is enabled. End of charge allowed. battery charging enabled
		i2c_RegisterSet(I2C0, ADP5063_ADDR, 0x07, 0x07);
	}
}

void ADP5063_StopCharging(void)
{
	i2c_RegisterSet(I2C0,ADP5063_ADDR,0x07,0x06);
}
