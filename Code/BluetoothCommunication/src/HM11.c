#include "HM11.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "uart_connection.h"
#include <stdio.h>

void inittest(){
	//GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 0);
	//GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 0);
}

#define DEBUG

uint8_t testHM11(){
	clearRxBuffer();
	uart_sendText("AT");

	int counter=0;
	while (rxBuff.wrI<2 && counter<100000){counter++;};	//wait to receive response

	char response[5];
	memcpy( response, &rxBuff.data[0], 2 );
	response[2] = '\0';

	if(strcmp (response,"OK")==0){
		return 0;
	}
	else {
#ifdef DEBUG
		uart_sendText("\nTestFAIL\n");
#endif
		return 1;
	}
}

uint8_t setAdvertisingInterval(enum ADV_INT value){
	clearRxBuffer();
	char cValue;
	if(value < 10){
		cValue = value+'0';
	}
	else {
		cValue=(value-10)+'A';
	}
	char request[20];
	sprintf(request,"AT+ADVI%c",cValue);
	uart_sendText(request);
	uint8_t requestLen = strlen(request);

	int counter=0;
	while (rxBuff.wrI<requestLen && counter<100000){counter++;};

	char response[20];
	memcpy( response, &rxBuff.data[0],requestLen );
	response[requestLen] = '\0';

	char expectedMessage[20];
	sprintf(expectedMessage,"OK+Set:%c",cValue);

	if(strcmp (response,expectedMessage)==0){
		return 0;
	}
	else {
#ifdef DEBUG
		uart_sendText("\nAdvFAIL\n");
		/*uart_sendText(response);
		uart_sendChar('\n');
		uart_sendText(expectedMessage);
		uart_sendChar('\n');*/
#endif
		return 1;
	}

	return 0;
}

uint8_t resetModule(){
	clearRxBuffer();

	uart_sendText("AT+RESET");
	uint8_t requestLen=8;

	int counter=0;
	while (rxBuff.wrI<requestLen && counter<100000){counter++;};	//wait to receive response


	char response[20];
	memcpy( response, &rxBuff.data[0],requestLen);
	response[requestLen] = '\0';

	if(strcmp (response,"OK+RESET")==0){
		return 0;
	}
	else {
	#ifdef DEBUG
		uart_sendText("\nResetFAIL\n");
	#endif
		return 1;
	}

}

uint8_t setModuleName(char* name){
	clearRxBuffer();

	char request[30];
	sprintf(request,"AT+NAME%s",name);
	uart_sendText(request);
	uint8_t requestLen = strlen(request);

	int counter=0;
	while (rxBuff.wrI<requestLen && counter<100000){counter++;};

	char response[30];
	memcpy( response, &rxBuff.data[0], requestLen );
	response[requestLen] = '\0';

	char expectedMessage[30];
	sprintf(expectedMessage,"OK+Set:%s",name);

	if(strcmp (response,expectedMessage)==0){
		resetModule();
		return 0;
	}
	else {
#ifdef DEBUG
		uart_sendText("\nNameFAIL\n");
		/*uart_sendText(response);
		uart_sendChar('\n');
		uart_sendText(expectedMessage);
		uart_sendChar('\n');*/
#endif
		return 1;
	}
}

uint8_t setModulePower(enum MOD_POWER value){
	clearRxBuffer();
	char cValue;
	if(value < 4){
		cValue = value+'0';
	}
	else {
		return 1;
	}

	char request[20];
	sprintf(request,"AT+POWE%c",cValue);
	uart_sendText(request);
	uint8_t requestLen = strlen(request);

	int counter=0;
	while (rxBuff.wrI<requestLen && counter<100000){counter++;};

	char response[20];
	memcpy( response, &rxBuff.data[0],requestLen );
	response[requestLen] = '\0';

	char expectedMessage[20];
	sprintf(expectedMessage,"OK+Set:%c",cValue);

	if(strcmp (response,expectedMessage)==0){
		return 0;
	}
	else {
#ifdef DEBUG
		uart_sendText("\nPowFAIL\n");
		/*uart_sendText(response);
		uart_sendChar('\n');
		uart_sendText(expectedMessage);
		uart_sendChar('\n');*/
#endif
		return 1;
	}

	return 0;
}

uint8_t startModule(){
	clearRxBuffer();

	uart_sendText("AT+START");
	uint8_t requestLen=8;

	int counter=0;
	while (rxBuff.wrI<requestLen && counter<100000){counter++;};	//wait to receive response

	char response[20];
	memcpy( response, &rxBuff.data[0],requestLen);
	response[requestLen] = '\0';

	if(strcmp (response,"OK+START")==0){
		return 0;
	}
	else {
	#ifdef DEBUG
		uart_sendText("\nStartFAIL\n");
		uart_sendText(response);
		uart_sendChar('\n');
	#endif
		return 1;
	}
}

uint8_t initHM11(){
	inittest();

	if (testHM11()==1){
		return 1;	//test failed.
	}

	if(setAdvertisingInterval(ADV_INT_1022_5ms)==1){
		return 1;//ERROR
	}


	/*if(setModuleName("FitB\\o/")==1){
		return 1;//ERROR
	}*/

	if(setModulePower(POWER_MINUS_23_dbm)==1){
		return 1;//ERROR
	}

//	if(startModule()==1){
//		return 1;//ERROR
//	}

	clearRxBuffer();

	return 0;
}
