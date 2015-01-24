#include "i2c_connection.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "uart_connection.h"	//for i2c_Scan
#include <stdio.h>


void initI2C(void)
{
  CMU_ClockEnable(cmuClock_I2C0,true);


  int i;

  /* Initialize I2C driver for the device on the DK, using standard rate. */
  /* Devices on DK itself supports fast mode, */
  /* but in case some slower devices are added on */
  /* prototype board, we use standard mode. */
  //I2C_Init_TypeDef i2cInit = {true, true, 0, I2C_FREQ_FAST_MAX, i2cClockHLRFast};
  I2C_Init_TypeDef i2cInit = {true, true, 0, I2C_FREQ_STANDARD_MAX, i2cClockHLRStandard};


  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_I2C0, true);

  /* Output value must be set to 1 to not drive lines low. Set */
    /* SCL first, to ensure it is high before changing SDA. */
  GPIO_PinModeSet(I2CDRV_SCL_PORT, I2CDRV_SCL_PIN, gpioModeWiredAndPullUp, 1);
  GPIO_PinModeSet(I2CDRV_SDA_PORT, I2CDRV_SDA_PIN, gpioModeWiredAndPullUp, 1);

  /* In some situations (after a reset during an I2C transfer), the slave */
  /* device may be left in an unknown state. Send 9 clock pulses just in case. */
  for (i = 0; i < 9; i++)
  {
      /*
       * TBD: Seems to be clocking at appr 80kHz-120kHz depending on compiler
       * optimization when running at 14MHz. A bit high for standard mode devices,
       * but DK only has fast mode devices. Need however to add some time
       * measurement in order to not be dependable on frequency and code executed.
       */
    GPIO_PinOutSet(I2CDRV_SCL_PORT, I2CDRV_SCL_PIN);
    GPIO_PinOutClear(I2CDRV_SCL_PORT, I2CDRV_SCL_PIN);
  }

    /* Enable pins at config location (3 is default which is the location used on the DK) */
  I2C0->ROUTE = I2C_ROUTE_SDAPEN |
                I2C_ROUTE_SCLPEN |
                (I2CDRV_PORT_LOCATION << _I2C_ROUTE_LOCATION_SHIFT);

  I2C_Init(I2C0, &i2cInit);

}
void I2C0_IRQHandler(void)
{
  /* Just run the I2C_Transfer function that checks interrupts flags and returns */
  /* the appropriate status */
	I2C_Status = I2C_Transfer(I2C0);
}
int i2c_RegisterGet(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t *val)
{
  I2C_TransferSeq_TypeDef seq;
  uint8_t i2c_write_data[1];

  seq.addr = addr;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to be read */
  i2c_write_data[0] = reg;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len = 1;

  /* Select location/length to place register */
   seq.buf[1].data = val;
   seq.buf[1].len = 1;

  /* Do a polled transfer */
  I2C_Status = I2C_TransferInit(i2c, &seq);
  uint32_t timeout = I2CDRV_TRANSFER_TIMEOUT;
  while (I2C_Status == i2cTransferInProgress && timeout--)
  {
    /* Enter EM1 while waiting for I2C interrupt */
    //EMU_EnterEM1();
    /* Could do a timeout function here. */
	I2C_Status = I2C_Transfer(I2C0);
  }
  if(timeout==(uint32_t)(-1)){
  	  uart_sendText("\nERROR: I2C_get_timeout\n");
  }

  if (I2C_Status != i2cTransferDone)
  {
    return((int)I2C_Status);
  }

  return ((int) 1);
}
int i2c_RegisterSet(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint16_t  val)
{
  I2C_TransferSeq_TypeDef seq;
  uint8_t data[3];

  seq.addr = addr;
  seq.flags = I2C_FLAG_WRITE;
  /* Select register to be written */
  data[0] = ((uint8_t)reg);
  seq.buf[0].data = data;
  if (true)		//this means to read only 1 byte
  {
    /* Only 1 byte reg */
    data[1] = (uint8_t)val;
    seq.buf[0].len = 2;
  }
  else
  {
    data[1] = (uint8_t)(val >> 8);
    data[2] = (uint8_t)val;
    seq.buf[0].len = 3;
  }

  uint32_t timeout = I2CDRV_TRANSFER_TIMEOUT;
  /* Do a polled transfer */
  I2C_Status = I2C_TransferInit(i2c, &seq);
  while (I2C_Status == i2cTransferInProgress && timeout--)
  {
    /* Enter EM1 while waiting for I2C interrupt */
	  I2C_Status = I2C_Transfer(I2C0);
    /* Could do a timeout function here. */
  }

  if(timeout==0)
	  uart_sendText("I2C_set_timeout\n");

  return(I2C_Status);
}
bool i2c_Detect    (I2C_TypeDef *i2c, uint8_t addr)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t                    i2c_read_data[8];
  uint8_t                    i2c_write_data[2];

  /* Unused parameter */
  (void) i2c;

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select command to issue */
  i2c_write_data[0] = 0;
  i2c_write_data[1] = 0;
  seq.buf[0].data   = i2c_write_data;
  seq.buf[0].len    = 1;
  /* Select location/length of data to be read */
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 8;

  uint32_t timeout = I2CDRV_TRANSFER_TIMEOUT;
  /* Do a polled transfer */
  ret = I2C_TransferInit(I2C0, &seq);
  while (ret == i2cTransferInProgress && timeout--)
  {
      ret = I2C_Transfer(I2C0);
	  //UART_sendChar('e');
	  //EMU_EnterEM1();
  }

  if (ret != i2cTransferDone)
  {
    return(false);
  }

  return(true);
}

void i2c_Scan (I2C_TypeDef *i2c){
	int i=0;
	uart_sendText("i2cSCAN");
	for (;i<128;i++){
		if (i2c_Detect(i2c,i*2)==1){
			char buff[30];
			sprintf(buff,"Detected I2C device: %d %02x\n",i*2,i);
			uart_sendText(buff);
		}
	}
}


int i2c_Register_Read_Block (I2C_TypeDef *i2c,uint8_t addr, uint8_t reg, uint8_t length, uint8_t *data)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[1];
  /* Unused parameter */
  (void) i2c;

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to start reading from */
  i2c_write_data[0] = reg;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;
  /* Select length of data to be read */
  seq.buf[1].data = data;
  seq.buf[1].len  = length;

  uint32_t timeout = I2CDRV_TRANSFER_TIMEOUT;
  /* Do a polled transfer */
  I2C_Status = I2C_TransferInit(i2c, &seq);
  while (I2C_Status == i2cTransferInProgress && timeout--)
  {
  	  I2C_Status = I2C_Transfer(I2C0);
  }

  if(timeout==(uint32_t)(-1))
  	  uart_sendText("I2C_set_timeout\n");
  if (ret != i2cTransferDone)
  {
    return((int) ret);
  }

  return((int) length);
}
