#include "Si7013.h"
#include "..\ucPeripheralDrivers\i2c_connection.h"
#include "..\ucPeripheralDrivers\uart_connection.h"

#define SI7013_ADDR 	0x40*2

uint8_t Si7013_detect(void){
	return i2c_Detect(I2C0, SI7013_ADDR);
}

void Si7013_init(void){

}


static int Si7013_readTemperature(I2C_TypeDef *i2c, uint8_t addr, uint32_t *data,
                          	  	  uint8_t command){
	I2C_TransferSeq_TypeDef    seq;
	uint8_t                    i2c_read_data[2];
	uint8_t                    i2c_write_data[1];

	seq.addr  = addr;
	seq.flags = I2C_FLAG_WRITE_READ;
	/* Select command to issue */
	i2c_write_data[0] = command;
	seq.buf[0].data   = i2c_write_data;
	seq.buf[0].len    = 1;
	/* Select location/length of data to be read */
	seq.buf[1].data = i2c_read_data;
	seq.buf[1].len  = 2;

	I2C_Status = I2C_TransferInit(i2c, &seq);
	uint32_t timeout = I2CDRV_TRANSFER_TIMEOUT;
	while (I2C_Status == i2cTransferInProgress && timeout--) {
		I2C_Status = I2C_Transfer(I2C0);
	}
	if(timeout==(uint32_t)(-1)){
		uart_sendText("\nERROR: I2C_get_timeout\n");
	}

	if (I2C_Status != i2cTransferDone)
	{
	  *data = 0;
	  return((int) I2C_Status);
	}

	*data = ((uint32_t) i2c_read_data[0] << 8) + (i2c_read_data[1] & 0xfc);

	return((int) 2);
}

void Si7013_forceRhMeasurment(void){
	I2C_TransferSeq_TypeDef    seq;
	uint8_t                    i2c_write_data[1];

	seq.addr  = SI7013_ADDR;
	seq.flags = I2C_FLAG_WRITE;
	/* Select command to issue */
	i2c_write_data[0] = 0xE5;			//Measure Relative Humidity, No Hold Master Mode
	seq.buf[0].data   = i2c_write_data;
	seq.buf[0].len    = 1;

	I2C_Status = I2C_TransferInit(I2C0, &seq);
	uint32_t timeout = I2CDRV_TRANSFER_TIMEOUT;
	while (I2C_Status == i2cTransferInProgress && timeout--) {
		I2C_Status = I2C_Transfer(I2C0);
	}
	if(timeout==(uint32_t)(-1)){
		uart_sendText("\nERROR: I2C_get_timeout\n");
	}
}

int Si7013_readHumidityAndTemperature(uint32_t *rhData, int32_t *tData){
	 I2C_TransferSeq_TypeDef seq;
	 uint8_t data[2];

	 seq.addr = SI7013_ADDR;
	 seq.flags = I2C_FLAG_READ;

	 seq.buf[0].data = data;
	 seq.buf[0].len  = 2;

	 I2C_Status = I2C_TransferInit(I2C0, &seq);
	 uint32_t timeout = I2CDRV_TRANSFER_TIMEOUT;
	 while (I2C_Status == i2cTransferInProgress && timeout--)
	 {
	    //EMU_EnterEM1();
		I2C_Status = I2C_Transfer(I2C0);
	 }
	 if(timeout==(uint32_t)(-1)){
		 uart_sendText("\nERROR: I2C_get_timeout\n");
	 }

	 if (I2C_Status != i2cTransferDone)
	 {
	    return((int)I2C_Status);
	 }

	 *rhData = ((((((uint16_t)data[0])<<8)+data[1]) * 15625L) >> 13) - 6000;

	 Si7013_readTemperature(I2C0,SI7013_ADDR,tData,0xE0);
	 *tData = (((*tData) * 21965L) >> 13) - 46850; /* convert to milli-degC */

	 return ((int) 1);
}
