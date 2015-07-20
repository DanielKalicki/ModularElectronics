#ifndef __AS3935_H
#define __AS3935_H

#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AS3935_ADDR 	0x03*2

uint8_t AS3935_detect(void);
void AS3935_init(void);

void AS3935_powerDown();
void AS3935_powerUp();
void AS3935_set_GainBoost(uint8_t indoor);
void AS3935_set_Watchdog_And_NoiseFloor(uint8_t watchdog, uint8_t noiseFloorLevel);
void AS3935_set_MinLightingNumber_And_SpikeRejection(uint8_t minLightNumber, uint8_t spikeRejection);
void AS3935_set_TuningCaps(uint8_t tunCap);

uint8_t AS3935_read_Interrupt();
uint8_t AS3935_read_Distance();

#ifdef __cplusplus
}
#endif

#endif
