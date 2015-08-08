/*
 * Si114x.c
 *
 *  Created on: 18-07-2015
 *      Author: terg
 */

#include "Si114x.h"
#include "Si114x\Si114x_reg.h"
#include "..\ucPeripheralDrivers\leuart_connection.h"
#ifdef DEBUG
#include <stdio.h>
#endif

#ifndef TRUE
	#define TRUE	(1)
#endif
#ifndef FALSE
	#define FALSE	(0)
#endif
#if TRUE == FALSE
	#error("TRUE macro == FALSE macro");
#endif

/* --------------------------------------------- */
/*												 */
/* 		 IC related function - higher level 	 */
/*												 */
/* --------------------------------------------- */
uint8_t Si114x_Detect(void)
{
	return (Si114x_Detect_LowLevel());
}

void Si114x_Init(Si114x_Settings_t Si114x_Settings)
{
			/*--------------------------------*/
			/*        IRQ configuration       */
			/*--------------------------------*/
	/* By setting the mode to gpioModeInput its value can be read */
	GPIO_PinModeSet(Si114x_Settings.irq_port, Si114x_Settings.irq_pin, gpioModeInput, 1);
	if ((Si114x_Settings.irq_pin & 0x01) == 0)
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
	GPIO_IntConfig(Si114x_Settings.irq_port, Si114x_Settings.irq_pin, false, true, true);


			/*--------------------------------*/
			/*      Si114x configuration      */
			/*--------------------------------*/
	Si114x_SetHwKey();

	/* turn on ALS_VIS measurement */
	Si114x_EnableChannel_t enableChannel;
	enableChannel.ALS_IR 	= FALSE;
	enableChannel.ALS_Vis 	= TRUE;
	enableChannel.AUX 		= FALSE;
	enableChannel.PS1 		= TRUE;
	enableChannel.PS2 		= TRUE;
	enableChannel.PS3 		= TRUE;
	enableChannel.UV 		= TRUE;
	Si114x_EnableChannel(enableChannel);

	/* ALS_VIS_ADC_GAIN is set to 1 by default */
	/* ALS_VIS_ADC_MISC is set to Normal Signal Range by default */

	/* Set the device to wake-up every 1ms and performs all CHILIST */
	/* Maximum ALS + UV + 3x PS measurement time = 660us */
	/* Single PS measurement takes = 155us */
	Si114x_SetMeasurmentRate(3200);

	Si114x_PsX_Led_Select_t PsXLedSelect;
	PsXLedSelect.PS1_Led = SI114X_LED1_ENABLE;
	PsXLedSelect.PS2_Led = SI114X_LED2_ENABLE;
	PsXLedSelect.PS3_Led = SI114X_LED3_ENABLE;
	Si114x_SetPsLedSelect(PsXLedSelect);

	/* The encoding is set to 0 by default */
	/* Si114x_SetPsEncoding(0,0,0); */
	/* Si114x_SetAlsEncoding(0,0); */

	/* MUX settings */
	/* TODO This should work with IR diodes, but currently i dont have any
	 * so i used RED Leds so the PS measurements are made using a visible
	 * photodiode */
	Si114x_SetPs1ADC(SI114X_PS_ADC_VISIBLE_PHOTODIODE);
	Si114x_SetPs2ADC(SI114X_PS_ADC_VISIBLE_PHOTODIODE);
	Si114x_SetPs3ADC(SI114X_PS_ADC_VISIBLE_PHOTODIODE);
	/* Si114x_SetAuxADC() This is not set, the default values is working ok */

	/* Enable interrupts */
	// Si114x_SetIntCfg(SI114X_INT_OE_ENABLE);

	/* Enable measurement interrupts */
	//Si114x_SetIrqEnable(SI114X_PS3_IE_DISABLE, SI114X_PS2_IE_DISABLE,
	//					SI114X_PS1_IE_ENABLE , SI114X_ALS_IE_DISABLE);

	/* Set the led currents */
	Si114x_SetLedCurrents(0x0A,0x0A,0x0A);

	/* 0x07 = 511 ADC Clocks = 25.55us */
	/* 0x07 is the default value */
	Si114x_SetPsAdcCounter(0x07);
	Si114x_SetAlsVisAdcCounter(0x07);
	Si114x_SetAlsIrAdcCounter(0x07);

	/* Set High gain Signal Range on PS measurement  (1b)
	 * and Normal Proximity Measurement mode         (1b) */
	Si114x_SetPsAdcMisc(1, 1);

	/* Default values set to 1 ADC clock = 25.55us */
	/* This ensures low power consumption but the detection distance is reduced */
	Si114x_SetPsAdcGain(0x00, 0);
	Si114x_SetAlsVisAdcGain(0x00);
	Si114x_SetAlsIrAdcGain(0x00);

	Si114x_SetCommand(SI114X_CMD_PS_AUTO, 0);
}

void Si114x_Interrupt(void)
{
#ifdef DEBUG
	LeUart_SendChar('s');
	LeUart_SendChar('i');
#endif
	Si114x_ClrIrqStatus();
}

void Si114x_ForceAmbientLightMeasurment(void)
{
	/* Force a single ALS measurement */
	Si114x_SetCommand(SI114X_CMD_ALS_FORCE, 0);
}

uint8_t Si114x_ReadAmbientLight(uint16_t *AmbientLight)
{
	uint8_t response = 0;
	response = Si114x_GetResponse();

	if((response & 0xF0) != 0)
	{
		/* ----there was an error---- */
#ifdef DEBUG
		char buff[30];
		sprintf(buff, "\n\t\t\tSI114x ALS meas Error=0x%02x\n", response);
		LeUart_SendText(buff);
#endif

		/* send to the sequencer RESET cmd */
		Si114x_SetCommand(SI114X_CMD_NOP, 0);
		return -1;
	}
	else
	{
		/* ----no error occurred---- */
		*AmbientLight = Si114x_GetAlsVisData();
	}
	return 0;
}

void Si114x_ForceUvIndexMeasurment(void)
{
	Si114x_EnableUvReading();
	/* Force a single ALS measurement */
	Si114x_SetCommand(SI114X_CMD_ALS_FORCE, 0);
}

uint8_t Si114x_ReadUvIndex(uint16_t *UvIndex)
{
	uint8_t response = 0;
	uint8_t ret = 0;
	response = Si114x_GetResponse();

	if((response & 0xF0) != 0)
	{
		/* ----there was an error---- */
#ifdef DEBUG
		char buff[30];
		sprintf(buff, "\n\t\t\tSI114x UV meas Error=0x%02x\n", response);
		LeUart_SendText(buff);
#endif

		/* send to the sequencer RESET cmd */
		Si114x_SetCommand(SI114X_CMD_NOP, 0);
		ret = -1;
	}
	else
	{
		/* ----no error occurred---- */
		*UvIndex = Si114x_GetAuxUvData();
	}

	Si114x_DisableUvReading();

	return(ret);
}

uint8_t Si114x_ReadPsMeasurments(uint16_t *PS1, uint16_t *PS2, uint16_t *PS3)
{
	uint8_t response = 0;
	uint8_t ret = 0;
	response = Si114x_GetResponse();

	if((response & 0xF0) != 0)
	{
		/* ----there was an error---- */
#ifdef DEBUG
		char buff[30];
		sprintf(buff, "\n\t\t\tSI114x UV meas Error=0x%02x\n", response);
		LeUart_SendText(buff);
#endif

		/* send to the sequencer RESET cmd */
		Si114x_SetCommand(SI114X_CMD_NOP, 0);
		ret = -1;
	}
	else
	{
		/* ----no error occurred---- */
		*PS1 = Si114x_GetPs1Data();
		*PS2 = Si114x_GetPs2Data();
		*PS3 = Si114x_GetPs3Data();
	}

	Si114x_DisableUvReading();

	return(ret);
}
