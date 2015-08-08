#include "MPU6050.h"
#include "..\ucPeripheralDrivers\i2c_connection.h"
#include "..\ucPeripheralDrivers\uart_connection.h"
#include "MPU6050\inv_mpu.h"
#include "MPU6050\inv_mpu_dmp_motion_driver.h"

//MPU6050 functions
static inline unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}
static inline unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}

static void tap_cb(unsigned char direction, unsigned char count)
{
    char data[2];
    data[0] = (char)direction;
    data[1] = (char)count;
    char buff[30];
    sprintf(buff,"\n\n\tPACKET_TYPE_TAP dir:%d count:%d\n\n",data[0],data[1]);
    uart_sendText(buff);
    tapData[i_tapData]=(direction<<3)+(count&0x07);
    if(i_tapData<9) i_tapData++;
}
static void android_orient_cb(unsigned char orientation)
{
    char buff[30];
    sprintf(buff,"\n\n\tPACKET_TYPE_ANDROID_ORIENT %d\n\n",orientation);
    tapData[i_tapData]=orientation<<3;	//if count value is equal to 0 this means we got an orientation change event
    if(i_tapData<9) i_tapData++;
    uart_sendText(buff);
}

uint8_t MPU6050_detect(void){
	return i2c_Detect(I2C0, MPU6050_ADDR);
}

void MPU6050_init(){
	mpu_init(0);

	mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	mpu_set_sample_rate(DEFAULT_MPU_HZ);

	dmp_load_motion_driver_firmware();
	mpu_set_dmp_state(0);

	static signed char gyro_orientation[9] = {-1, 0, 0,
											   0,-1, 0,
											   0, 0, 1};
	dmp_set_orientation(
			inv_orientation_matrix_to_scalar(gyro_orientation));

	dmp_register_tap_cb(tap_cb);
	dmp_register_android_orient_cb(android_orient_cb);

	unsigned short dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
			DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
			DMP_FEATURE_GYRO_CAL;
	dmp_enable_feature(dmp_features);

	dmp_enable_gyro_cal(1);
	dmp_enable_6x_lp_quat(1);

	dmp_set_pedometer_step_count(0);
	dmp_set_pedometer_walk_time(0);

	dmp_set_interrupt_mode(DMP_INT_GESTURE);

	mpu_set_dmp_state(1);

	unsigned short dmp_rate=20;
	dmp_set_fifo_rate(dmp_rate); 		//only when DMP is on
	dmp_get_fifo_rate(&dmp_rate);
	mpu_set_sample_rate(dmp_rate);
}

void MPU6050_config_register(){
	//enter low-power accel-only mode
	/*if (i2c_registers[REG_MPU6050_CONFIG_NUMBER]&0x01){
		mpu_lp_accel_mode(20);
	}
	else {
		mpu_lp_accel_mode(0); // set to 0 to disabled this mode
		mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
		mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	}*/
}
