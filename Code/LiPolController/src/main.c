#include "em_device.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_i2c.h"
#include "em_emu.h"

#include "i2c_connection.h"
#include "uart_connection.h"
#include "RTC_.h"
#include "em_rtc.h"

#define ADP5063_ADDR 0x14*2
#define LTC2942_ADDR 0x64*2

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

uint8_t devices=0;
//------------LTC2942------------
void setupLTC2942(){
	uint8_t val=0;
	i2c_RegisterGet(I2C0,LTC2942_ADDR,0x01,&val);
	if(val!=0x3C){	//device is power down    change to 0x3A to use AL/CC pin
		i2c_RegisterSet(I2C0,LTC2942_ADDR,0x01,0x3C);	//change to 3A to set AL/CC as input change it also in the measurment force
		uart_sendText("Initializing LTC2942: Control register set to 0x3C\n");
	}
	else {
		char buff[30];
		uart_sendText("LTC2942 doesnt require initialization. ");
		sprintf(buff,"Control regiser is set to 0x%02x\n",val);
		uart_sendText (buff);
	}
}

#define CHARGE_HISTORY_BUFF_SIZE	200
uint16_t chargeHistory[CHARGE_HISTORY_BUFF_SIZE];
uint8_t i_chargeHistory=0;

void resetLTC2942charge(){
	uart_sendText("\n\n\tCharge Completed\n\tReseting the accumulated charge in LT2942\n");
	setupLTC2942();
	i2c_RegisterSet(I2C0,LTC2942_ADDR,0x02,0xFF);
	i2c_RegisterSet(I2C0,LTC2942_ADDR,0x03,0xFF);

	for (uint8_t i=0;i<CHARGE_HISTORY_BUFF_SIZE;++i){
		chargeHistory[i]=0;
	}
	i_chargeHistory=0;
}

//----------I2C_SLAVE--------
void check_slavesList(){
	if (i2c_slave_address<0x10){ //addreses lower than 0x10 are set as a default at startup for i2c line sniffing
		if(slavesListCheck[0]>=0xFFFFFFFE && slavesListCheck[1]==0xFFFFFFFF && slavesListCheck[2]==0xFFFFFFFF && slavesListCheck[3]==0xFFFFFFFF){
			//this code will be reached if i2c list is full
			i2c_slave_address = 0x14;
			i2c_registers[REG_I2C_ADDR] = i2c_slave_address;
		}
	}
}


//-------------INIT--------------
void initOscillators(void){
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);                   // enable GPIO peripheral clock
}
void initGPIO(void){
	//UART
	GPIO_PinModeSet(COM_PORT, TX_PIN, gpioModePushPull, 1); // set TX pin to push-pull output, initialize high (otherwise glitches can occur)
	GPIO_PinModeSet(COM_PORT, RX_PIN, gpioModeInput, 0);    // set RX pin as input (no filter)
}

void clockTest();
void clockTest_short() {
	long int i=0;
	for(;i<121L;++i) {
	  if(i==120L)
		  GPIO_PortOutSet(gpioPortF, 0x80);
	  if(i==60L)
		  GPIO_PortOutClear(gpioPortF, 0x80);
	}
}

void setupADP5063(){
	//set VINx input current-limit to 100mA -> [3:0] 0000
	i2c_RegisterSet(I2C0,ADP5063_ADDR,0x02,0x00);
	//Termination voltage set to 4.2V [7:2] 100011. Charging voltage limit set to 3.8V [1:0] 11
	i2c_RegisterSet(I2C0,ADP5063_ADDR,0x03,0x8F);
	//Fast charge current set to 50mA [6:2] 00000. Trickle and weak charge current set to 5mA [1:0] 00
	i2c_RegisterSet(I2C0,ADP5063_ADDR,0x04,0x00);
	//Recharge enabled [7] 0. Recharge voltage set to 260mV [6:5] 11. V_dead set to 2.9V [4:3] 11. Weak battery voltage set to 3.4V [2:0] 111
	i2c_RegisterSet(I2C0,ADP5063_ADDR,0x05,0xFF);
	//Register 0x06, 0x08 and 0x10 set to default
	//termination current set to 12.5mA [7:5] 000. Other values are set to Default
	i2c_RegisterSet(I2C0,ADP5063_ADDR,0x11,0x00);
}

void readStatusADP5063(){
	static uint8_t chargingCompleted=0;
	uint8_t canStartCharging=0;

	uint8_t val[2];
	//0x0B -> Charger status 1
	//0x0C -> Charger status 2
	i2c_Register_Read_Block(I2C0,ADP5063_ADDR,0x0B,2,val);

	i2c_registers[REG_ADP5063_CHARGER_ST_1]=val[0];
	i2c_registers[REG_ADP5063_CHARGER_ST_2]=val[1];

#define VIN_OV	 	1<<7
#define VIN_OK		1<<6
#define V_ILIM		1<<5
#define THER_LIM	1<<4
#define CHDONE		1<<3
#define CHARG_STAT	0x07

	if(val[0]&VIN_OV){		uart_sendText("Voltage at VINx exceeds V_VIN_OV\n");	}
	if(val[0]&VIN_OK){ 		uart_sendText("Voltage at VINx exceeds V_VIN_OK_RISE and V_VIN_OK_FALL\n");	}
	if(val[0]&V_ILIM){		uart_sendText("Current into VINx is limited by the high voltage blocking FET and the charger is not running at the full programmed I_CHG\n"); }
	if(val[0]&THER_LIM){	uart_sendText("Charger is not running at the full programmed I_CHG but is limited by the die temperature\n"); }
	if(val[0]&CHDONE){		uart_sendText("End of a charge cycle has been reached. This bit latches on, in that it does not reset to low when the V_RCH threshold is breached\n"); }
	if(val[0]&CHARG_STAT){
		if((val[0]&CHARG_STAT)==1) { 		uart_sendText("Charger is in trickle charge state\n"); }
		else if((val[0]&CHARG_STAT)==2) { 	uart_sendText("Charger is in fast charge mode (CC mode)\n"); }
		else if((val[0]&CHARG_STAT)==3) { 	uart_sendText("Charger is in fast charge mode (CV mode)\n"); }
		else if((val[0]&CHARG_STAT)==4) {
			uart_sendText("Charge completed\n");
			if(chargingCompleted==0){
				resetLTC2942charge();
				chargingCompleted=1;
			}
		}
		else if((val[0]&CHARG_STAT)==5) { 	uart_sendText("LDO mode\n"); canStartCharging=1; }
		else if((val[0]&CHARG_STAT)==6) { 	uart_sendText("trickle or fast charge timer expired\n"); }
		else if((val[0]&CHARG_STAT)==7) { 	uart_sendText("charger is set in battery detection mode\n"); }

		if((val[0]&CHARG_STAT)!=4){	//if charge isnt completed reset the chargeCompleted variable
			chargingCompleted=0;
		}
	}
	else{					uart_sendText("Charger is off\n");	}

#define THR_STAT	0xE0
#define RCH_LIM		1<<3
#define BATT_STAT	0x07

	if((val[1]&THR_STAT)){			uart_sendText("THR status: ");}
	if((val[1]&THR_STAT)==0x20){	uart_sendText("battery cold\n");}
	if((val[1]&THR_STAT)==0x40){	uart_sendText("battery cool\n");}
	if((val[1]&THR_STAT)==0x60){	uart_sendText("battery warm\n");}
	if((val[1]&THR_STAT)==0x80){	uart_sendText("battery hot\n");}
	if((val[1]&THR_STAT)==0xE0){	uart_sendText("termistor OK\n");}

	if((val[0]&CHARG_STAT)==4){	//recharge limit information function is active when CHARGER_STATUS = 100 (binary)
		if(val[1]&RCH_LIM){	uart_sendText("V_BAT_SNS < V_R_CH\n"); }
		else{				uart_sendText("V_BAT_SNS > V_R_CH\n"); }
	}
	if(val[1]&BATT_STAT){					uart_sendText("Battery status: ");}
	if((val[1]&BATT_STAT)==1){ 				uart_sendText("No battery\n");}
	else if((val[1]&BATT_STAT)==2){ 		uart_sendText("V_BAT_SNS < V_TRK_DEAD\n");}
	else if((val[1]&BATT_STAT)==3){ 		uart_sendText("V_TRK_DEAD <= V_BAT_SNS < V_WEAK\n");}
	else if((val[1]&BATT_STAT)==4){			uart_sendText("V_BAT_SNS >= V_WEAK\n");}

	uart_sendText("\n\n");

	if (canStartCharging){
		uart_sendText("\n\t\tStarting battery charging\n\n");
		setupADP5063();
		//Normal operation of the device. LDO is enabled. End of charge allowed. battery charging enabled
		i2c_RegisterSet(I2C0,ADP5063_ADDR,0x07,0x07);
	}
}

void initDevices(){
	//-----ADP5063----
	if(devices&0x01){
		setupADP5063();
	}
	//-----LTC2942----
	if(devices&0x02){
		setupLTC2942();
	}
	clockTest_short();
}

uint8_t RTC_interrupt_counter=0;

#define LTC2942_READ_TEMP

void RTC_IRQHandler(void){

	disableI2cSlaveInterrupts();
	initI2C_Master();

	i2c_registers[REG_DATA_LENGTH]=REG_DATA_LENGTH_VALUE;
	i2c_registers[REG_I2C_ADDR]   =i2c_slave_address;
	i2c_registers[REG_DEVICES]	  =devices;
	//TODO add command to detect i2c device once again

	static uint8_t counter=0;
	i2c_registers[REG_COMMAND_CODE]=counter;
	counter++;
	if(counter==255) counter=0;

	char buff[30];

	RTC_interrupt_counter++;
	if(RTC_interrupt_counter==5){	//every 5seconds force battery voltage and temperature measurment
		if(devices&0x02){
			i2c_RegisterSet(I2C0,LTC2942_ADDR,0x01,0xBC); //force single battery voltage measurment
		}
		RTC_interrupt_counter=0;
		if(devices&0x01){
			readStatusADP5063();	//every 5 seconds we check the battery charger
		}
	}
	else if (RTC_interrupt_counter==1){	//read battery voltage and temperature

		if(devices&0x02){
			uint8_t val[2];
			//calculate battery voltage
			i2c_Register_Read_Block(I2C0,LTC2942_ADDR,0x08,2,val);
			uint16_t val16 = ((((uint32_t)val[0])<<8)+val[1]);
			uint32_t batteryVoltage=(uint32_t)60000*val16/65535;		//battery voltage in 0.1mV

			i2c_registers[REG_BAT_VOLT_1] = val[0];
			i2c_registers[REG_BAT_VOLT_2] = val[1];

			sprintf(buff,"BAT %ldmV\t",batteryVoltage/10);
			uart_sendText(buff);

			if(devices&0x01){
				if(batteryVoltage/10 > 4250){	//if the voltage exides 4.2V stop charging
					i2c_RegisterSet(I2C0,ADP5063_ADDR,0x07,0x06);
				}
			}

			#ifdef LTC2942_READ_TEMP
				//calculate battery module temperature
				i2c_Register_Read_Block(I2C0,LTC2942_ADDR,0x0C,2,val);
				val16 = ((((uint32_t)val[0])<<8)+val[1]);
				uint32_t batteryTemp=((uint32_t)6000*val16/65535)-2731;	//batery module temperature in 0.1 C

				i2c_registers[REG_BAT_TEMP_1]=val[0];
				i2c_registers[REG_BAT_TEMP_2]=val[1];

				sprintf(buff," %ld[0.1C]\n",batteryTemp);
				uart_sendText(buff);
				i2c_RegisterSet(I2C0,LTC2942_ADDR,0x01,0x7C); //force single temperature measurment
			#else
				uart_sendChar('\n');
			#endif
		}
	}

	if(devices&0x02){
		uint8_t val[2];
		i2c_Register_Read_Block(I2C0,LTC2942_ADDR,0x02,2,val);
		uint32_t charge =  ((uint32_t)65535-((((uint32_t)val[0])<<8)+val[1]));

		i2c_registers[REG_BAT_CHRG_1]=val[0];
		i2c_registers[REG_BAT_CHRG_2]=val[1];

		uint16_t val16 = (uint16_t)charge;
		chargeHistory[i_chargeHistory]=val16;
		i_chargeHistory++;
		if(i_chargeHistory==CHARGE_HISTORY_BUFF_SIZE) {i_chargeHistory=0;}
		if(chargeHistory[i_chargeHistory]!=0){	//if this==0 that meanse that the table isnt fill yet
			uint32_t dischargeSpeed = (val16-chargeHistory[i_chargeHistory])*85*2*6/CHARGE_HISTORY_BUFF_SIZE;	//assuming every measurment made in 500ms time steps -> the number 2 stands for it
			sprintf(buff,"DS:%ld uAh/min\t",dischargeSpeed);
			i2c_registers[REG_BAT_DISCHR_1] = (uint8_t) (dischargeSpeed>>8);
			i2c_registers[REG_BAT_DISCHR_2] = (uint8_t) dischargeSpeed;
			// (val16-ch[i]) * 85 is in 0.1uAh ===> [x]*2 -> is in 0.1uAh/second   ===> [x]*60 ===> 0.1uAh/min  ===> [x]/10 ===> 1uAh/min
			uart_sendText(buff);
		}

		//R_sense resistor is hard coded to 500mOhm
		//the equation is accumulated_charge * 0.085 [mAh] * 50mOhm/R_sense * M/128,   M-> prescaller setting
		charge *= 85;	//charge value in 0.1uAh
		sprintf(buff,"Q:%ld uAh\n",charge/10);
		uart_sendText(buff);
	}

	for (int i=0;i<REG_DATA_LENGTH_VALUE;i++){
		sprintf(buff,"%d:%d\t",i,i2c_registers[i]);
		uart_sendText(buff);
	}

	check_slavesList();

	//initI2C_Slave();
	enableI2cSlaveInterrupts();

	/* Clear interrupt source */
	RTC_IntClear(RTC_IFC_COMP0);
}

void detectDevices(){
	devices=0xFF;

	//-----ADP5063----
	if (i2c_Detect(I2C0,ADP5063_ADDR)==1){
		uart_sendText("\t\tADP5063 detected\n");
		devices |= 0x01;
	}
	else uart_sendText("\t---\tADP5063 NOT DETECTED\t---\t\n");

	//-----LTC2942----
	if (i2c_Detect(I2C0,LTC2942_ADDR)==1){
		uart_sendText("\t\tLTC2942 detected\n");
		devices |= 0x02;
	}
	else uart_sendText("\t---\tLTC2942 NOT DETECTED\t---\t\n");
}

//-------------MAIN-------------
int main(void)
{
  CHIP_Init();

  initOscillators();
  initGPIO();
  initUART();

  i2c_slave_address=0x02;

  for (int i=0;i<100;i++){
	  i2c_registers[i]=0;
  }

  i2c_registers[REG_DATA_LENGTH]=REG_DATA_LENGTH_VALUE;
  i2c_registers[REG_MODULE_ID_1]='L';
  i2c_registers[REG_MODULE_ID_2]='B';
  i2c_registers[REG_MODULE_ID_3]='C';
  //id: 4997699

  initI2C_Master();
  i2c_Scan(I2C0);

  detectDevices();
  uart_sendText("\nSTARTUP\n");
  initDevices();

  setupRtc();

  //initI2C_Slave();

  /* Infinite loop */
  while (1) {
	  //EMU_EnterEM2(false);
	  while(i2c_rxInProgress){
	     EMU_EnterEM1();
	  }
	   /* Forever enter EM2. The RTC or I2C will wake up the EFM32 */
	  EMU_EnterEM2(false);
  }
}
