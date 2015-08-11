#ifndef ADP5063_H_
#define ADP5063_H_

#include <stdint.h>

void ADP5063_Init(void);
uint8_t ADP5063_Detect(void);
void ADP5063_PrintStatus(void);
void ADP5063_StopCharging(void);

#endif /* ADP5063_H_ */
