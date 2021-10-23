//#ifndef HC_SR04_H
//#define HC_SR04_H
#include "tm4c123gh6pm.h"
#include <stdint.h>

void InitRegisters(void);
void Timer0A_init(void);
void Timer2A_init(void); 
void Timer3A_init(void);
void delay_Microsecond(uint32_t time);
int32_t measureD(char timer);
void OutSignal(uint32_t value);
void TriggerSignal(void);

//#endif  HC_SR04_H