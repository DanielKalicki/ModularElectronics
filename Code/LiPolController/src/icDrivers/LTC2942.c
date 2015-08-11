#include "LTC2942.h"
#include "..\ucPeripheralDrivers\i2c_connection.h"
#include "..\ucPeripheralDrivers\leuart_connection.h"

#define LTC2942_ADDR 0x64*2

#define  LTC2942_CHARGE_HISTORY_BUFF_SIZE	200
uint16_t LTC2942_ChargeHistory[LTC2942_CHARGE_HISTORY_BUFF_SIZE];
uint8_t  LTC2942_i_ChargeHistory = 0;

/*             !!!				  */
/* TODO made this as a proper lib */
/*             !!!				  */

void LTC2942_Init(void)
{
	uint8_t val = 0;
	i2c_RegisterGet(I2C0, LTC2942_ADDR, 0x01, &val);
	if(val != 0x3C)//device is power down    change to 0x3A to use AL/CC pin
	{
		i2c_RegisterSet(I2C0,LTC2942_ADDR, 0x01, 0x3C);	//change to 3A to set AL/CC as input change it also in the measurment force
#ifdef DEBUG
		LeUart_SendText("Initializing LTC2942: Control register set to 0x3C\n");
#endif
	}
#ifdef DEBUG
	else {
		char buff[30];
		LeUart_SendText("LTC2942 doesn't require initialization. ");
		sprintf(buff, "Control register is set to 0x%02x\n", val);
		LeUart_SendText(buff);
	}
#endif
	//reset the LTC2942 charge accumulated setting TODO delete this in the future
	//resetLTC2942charge();
}

uint8_t LTC2942_Detect(void)
{
	return i2c_Detect(I2C0, LTC2942_ADDR);
}

void LTC2942_ChargeHistory_ResetAccumulated(void)
{
#ifdef DEBUG
	LeUart_SendText("\n\n\tCharge Completed\n\tReseting the accumulated charge in LT2942\n");
#endif
	LTC2942_Init();
	i2c_RegisterSet(I2C0, LTC2942_ADDR, 0x02, 0xFF);
	i2c_RegisterSet(I2C0, LTC2942_ADDR, 0x03, 0xFF);

	for (uint8_t i = 0; i < LTC2942_CHARGE_HISTORY_BUFF_SIZE; ++i)
	{
		LTC2942_ChargeHistory[i] = 0;
	}
	LTC2942_i_ChargeHistory =  0;
}

void LTC2942_ChargeHistory_AddNewSample(uint16_t charge)
{
	LTC2942_ChargeHistory[LTC2942_i_ChargeHistory] = charge;
	LTC2942_i_ChargeHistory++;
	if (LTC2942_CHARGE_HISTORY_BUFF_SIZE == LTC2942_i_ChargeHistory)
	{
		LTC2942_i_ChargeHistory = 0;
	}
}

uint16_t LTC2942_ChargeHistory_GetOldestSample(void)
{
	return  LTC2942_ChargeHistory[LTC2942_i_ChargeHistory];
}

uint16_t LTC2942_ChargeHistory_GetBufferSize(void)
{
	return  LTC2942_CHARGE_HISTORY_BUFF_SIZE;
}

uint8_t LTC2942_ChargeHistory_IsFull(void)
{
	/* if next sample in history == 0 that means that the table isn't fill yet */
	return (LTC2942_ChargeHistory[LTC2942_i_ChargeHistory] != 0);
}


uint16_t LTC2942_GetCharger(void)
{
	uint8_t val[2];
	i2c_Register_Read_Block(I2C0,LTC2942_ADDR,0x02,2,val);
	return (((uint16_t)val[0]) << 8) + val[1];
}
uint16_t LTC2942_GetVoltage(void)
{
	uint8_t val[2];
	i2c_Register_Read_Block(I2C0,LTC2942_ADDR,0x08,2,val);
	return (((uint16_t)val[0]) << 8) + val[1];
}

uint16_t LTC2942_GetTemperature(void)
{
	uint8_t val[2];
	i2c_Register_Read_Block(I2C0, LTC2942_ADDR, 0x0C, 2, val);
	return (((uint16_t)val[0]) << 8) + val[1];
}

void LTC2942_ForceSingleVoltageMeasurement(void)
{
	i2c_RegisterSet(I2C0, LTC2942_ADDR, 0x01, 0xBC);
}

void LTC2942_ForceSingleTemperatureMeasurement(void)
{
	i2c_RegisterSet(I2C0,LTC2942_ADDR,0x01,0x7C);
}
