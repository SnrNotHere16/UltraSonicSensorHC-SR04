#include "hc-sr04.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "PLL.h"
// user button connected to PF4 (increment counter on falling edge)


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


int sprintf(char*, const char*, ...);

//debug code
int main(void){
	int32_t dist = 0;
	char distStr0[20] = "";
	char distStr1[20] = "";
	char distStr2[20] = "";
	PLL_Init();
	UART0_Init();
	InitRegisters();
  while(1){
    Timer0A_init();
		Timer2A_init(); 
		Timer3A_init();
		dist = measureD(0);
		OutSignal(dist);
	  sprintf(distStr0, "%d cm0     ", dist);
		UART_OutString(distStr0); 
		dist = measureD(1);
		OutSignal(dist);
	  sprintf(distStr1, "%d cm1     ", dist);
	  UART_OutString(distStr1); 
		delay_Microsecond(10000);
  }
}