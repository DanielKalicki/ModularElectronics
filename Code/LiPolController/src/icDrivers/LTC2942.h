#ifndef LTC2942_H_
#define LTC2942_H_

#include <stdint.h>

void LTC2942_Init(void);
uint8_t LTC2942_Detect(void);

void LTC2942_ChargeHistory_ResetAccumulated(void);
void LTC2942_ChargeHistory_AddNewSample(uint16_t charge);
uint8_t LTC2942_ChargeHistory_IsFull(void);
uint16_t LTC2942_ChargeHistory_GetOldestSample(void);
uint16_t LTC2942_ChargeHistory_GetBufferSize(void);

uint16_t LTC2942_GetCharger(void);
uint16_t LTC2942_GetVoltage(void);
uint16_t LTC2942_GetTemperature(void);

void LTC2942_ForceSingleVoltageMeasurement(void);
void LTC2942_ForceSingleTemperatureMeasurement(void);

#endif /* LTC2942_H_ */
