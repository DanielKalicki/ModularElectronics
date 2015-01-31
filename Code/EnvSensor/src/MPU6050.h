#ifndef __MPU6050_H
#define __MPU6050_H

#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REG_MPU6050_CONFIG_NUMBER 0x06

#define DEFAULT_MPU_HZ  (100)

void MPU6050_init(void);
uint16_t MPU6050_getFifoCount(void);
void MPU6050_getMotion6(int16_t *ax, int16_t *ay, int16_t *az, int16_t *gx, int16_t *gy, int16_t *gz);

#ifdef __cplusplus
}
#endif

#endif
