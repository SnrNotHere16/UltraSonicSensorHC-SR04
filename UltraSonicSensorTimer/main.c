#include "hc-sr04.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "PLL.h"
// user button connected to PF4 (increment counter on falling edge)
/*
Pins 
Sensor Pins --
	Sensor 0 - 
		ECHO0 - PB6 (Timer0A)
		TRIGGER0 - PA6
	Sensor 1 -
		ECHO1 - PB0 (Timer2A)
		TRIGGER1 - PA5
	Sensor 2 - 
		ECHO2 - PB2 (Timer3A)
		TRIGGER2 - PA4
UART PINS -- 
	UART0 -- 
		PA0 - UART0 RX
		PA1 - UART0 TX
ONBOARD LEDS -- 
	PF1 - red (0x02) 
	PF2 - blue (0x04) 
	PF3 - green (0x08) 
*/

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
	UART0_Init();
	InitRegisters();
	Timer0A_init();
  Timer2A_init(); 
	Timer3A_init();
  while(1){
		dist = measureD(0);
		OutSignal(dist); //sensor 0 
	  sprintf(distStr0, "%d cm0 ", dist);
		UART_OutString(distStr0); 
		dist = measureD(1);
		//OutSignal(dist); //sensor 1 
	  sprintf(distStr1, "%d cm1 ", dist);
	  UART_OutString(distStr1); 
		dist = measureD(2);
	//	OutSignal(dist); //sensor 2
	  sprintf(distStr2, "%d cm2\r\n", dist);
	  UART_OutString(distStr2); 
		delay_Microsecond(10000);
  }
}