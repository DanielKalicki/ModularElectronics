#include "HM11.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "../ucPeripheralDrivers/uart_connection.h"
#include "../ucPeripheralDrivers/leuart_connection.h"
#include <stdio.h>

void inittest()
{
    //GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 0);
    //GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 0);
}

#define DEBUG

uint8_t HM11_Test()
{
    Uart_ClearRxBuffer();
    Uart_SendText("AT");

    int counter = 0;
    while (rxBuff.wrI < 2 && counter < 100000)
    {
        counter++;
    };	//wait to receive response

    char response[5];
    memcpy(response, &rxBuff.data[0], 2);
    response[2] = '\0';

    if (strcmp(response, "OK") == 0)
    {
        return 0;
    }
    else
    {
#ifdef DEBUG
        LeUart_SendText("\nTestFAIL\n");
#endif
        return 1;
    }
}

uint8_t HM11_SetAdvertisingInterval(enum ADV_INT value)
{
    Uart_ClearRxBuffer();
    char cValue;
    if (value < 10)
    {
        cValue = value + '0';
    }
    else
    {
        cValue = (value - 10) + 'A';
    }
    char request[20];
    sprintf(request, "AT+ADVI%c", cValue);
    Uart_SendText(request);
    uint8_t requestLen = strlen(request);

    int counter = 0;
    while (rxBuff.wrI < requestLen && counter < 100000)
    {
        counter++;
    };

    char response[20];
    memcpy(response, &rxBuff.data[0], requestLen);
    response[requestLen] = '\0';

    char expectedMessage[20];
    sprintf(expectedMessage, "OK+Set:%c", cValue);

    if (strcmp(response, expectedMessage) == 0)
    {
        return 0;
    }
    else
    {
#ifdef DEBUG
        LeUart_SendText("\nAdvFAIL\n");
        /*uart_sendText(response);
         uart_sendChar('\n');
         uart_sendText(expectedMessage);
         uart_sendChar('\n');*/
#endif
        return 1;
    }

    return 0;
}

uint8_t HM11_ResetModule()
{
    Uart_ClearRxBuffer();

    Uart_SendText("AT+RESET");
    uint8_t requestLen = 8;

    int counter = 0;
    while (rxBuff.wrI < requestLen && counter < 100000)
    {
        counter++;
    };	//wait to receive response

    char response[20];
    memcpy(response, &rxBuff.data[0], requestLen);
    response[requestLen] = '\0';

    if (strcmp(response, "OK+RESET") == 0)
    {
        return 0;
    }
    else
    {
#ifdef DEBUG
        LeUart_SendText("\nResetFAIL\n");
#endif
        return 1;
    }

}

uint8_t HM11_SetModuleName(char* name)
{
    Uart_ClearRxBuffer();

    char request[30];
    sprintf(request, "AT+NAME%s", name);
    Uart_SendText(request);
    uint8_t requestLen = strlen(request);

    int counter = 0;
    while (rxBuff.wrI < requestLen && counter < 100000)
    {
        counter++;
    };

    char response[30];
    memcpy(response, &rxBuff.data[0], requestLen);
    response[requestLen] = '\0';

    char expectedMessage[30];
    sprintf(expectedMessage, "OK+Set:%s", name);

    if (strcmp(response, expectedMessage) == 0)
    {
        HM11_ResetModule();
        return 0;
    }
    else
    {
#ifdef DEBUG
        LeUart_SendText("\nNameFAIL\n");
        /*uart_sendText(response);
         uart_sendChar('\n');
         uart_sendText(expectedMessage);
         uart_sendChar('\n');*/
#endif
        return 1;
    }
}

uint8_t HM11_SetModulePower(enum MOD_POWER value)
{
    Uart_ClearRxBuffer();
    char cValue;
    if (value < 4)
    {
        cValue = value + '0';
    }
    else
    {
        return 1;
    }

    char request[20];
    sprintf(request, "AT+POWE%c", cValue);
    Uart_SendText(request);
    uint8_t requestLen = strlen(request);

    int counter = 0;
    while (rxBuff.wrI < requestLen && counter < 100000)
    {
        counter++;
    };

    char response[20];
    memcpy(response, &rxBuff.data[0], requestLen);
    response[requestLen] = '\0';

    char expectedMessage[20];
    sprintf(expectedMessage, "OK+Set:%c", cValue);

    if (strcmp(response, expectedMessage) == 0)
    {
        return 0;
    }
    else
    {
#ifdef DEBUG
        LeUart_SendText("\nPowFAIL\n");
        /*uart_sendText(response);
         uart_sendChar('\n');
         uart_sendText(expectedMessage);
         uart_sendChar('\n');*/
#endif
        return 1;
    }

    return 0;
}

uint8_t HM11_StartModule()
{
    Uart_ClearRxBuffer();

    Uart_SendText("AT+START");
    uint8_t requestLen = 8;

    int counter = 0;
    /*wait to receive response */
    while (rxBuff.wrI < requestLen && counter < 100000)
    {
        counter++;
    };

    char response[20];
    memcpy(response, &rxBuff.data[0], requestLen);
    response[requestLen] = '\0';

    if (strcmp(response, "OK+START") == 0)
    {
        return 0;
    }
    else
    {
#ifdef DEBUG
        LeUart_SendText("\nStartFAIL\n");
        LeUart_SendText(response);
        LeUart_SendChar('\n');
#endif
        return 1;
    }
}

uint8_t HM11_SetBaudRate(enum BAUD_RATE value)
{
    uint32_t counter = 0;
    char request[30];
    char cValue;
    if (value < 8)
    {
        cValue = value + '0';
    }
    else
    {
        return 1;
    }
    sprintf(request, "AT+BAUD%c", cValue);
    Uart_SendText(request);

    /*initUART_9600();
     while (counter<100000){counter++;};
     uart_sendText(request);

     initUART_baud(19200);
     counter=0;
     while (counter<100000){counter++;};
     uart_sendText(request);

     initUART_9600();
     while (counter<100000){counter++;};
     uart_sendText(request);*/

    return 0;
}

uint8_t HM11_Init()
{

    //if(setBaudRate(BAUD_115200)==1){
    //	return 1;
    //}

    //if (testHM11()==1){
    //	return 1;	//test failed.
    //}

    //if(setAdvertisingInterval(ADV_INT_1022_5ms)==1){
    //	return 1;//ERROR
    //}

    //if(setModuleName("DanK")==1){
    //	return 1;//ERROR
    //}

    //if(setModulePower(POWER_MINUS_23_dbm)==1){
    //	return 1;//ERROR
    //}

//	if(startModule()==1){
//		return 1;//ERROR
//	}

    Uart_ClearRxBuffer();

    return 0;
}
