#ifndef TIMER__H_
#define TIMER__H_

void Timer_Init(void);

void Timer_EnableIRQ(void);
void Timer_DisableIRQ(void);

void Timer_ClrIRQ(void);

#endif /* TIMER__H_ */
