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

// global variable visible in Watch window of debugger
// increments at least once per button press
volatile unsigned long FallingEdges = 0;
void EdgeCounter_Init(void){                          
  SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
	//GPIO_PORTF_IBE_R |= 0x10;
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
	GPIO_PORTF_IEV_R |= 0x10;
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
}
void GPIOPortF_Handler(void){
  GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
  FallingEdges = FallingEdges + 1;
}
int sprintf(char*, const char*, ...);

//debug code
int main(void){
	int32_t dist = 0;
	char distStr[20] = "";
	PLL_Init();
  EdgeCounter_Init();           // initialize GPIO Port F interrupt
	UART0_Init();
	InitRegisters();
  while(1){
		//WaitForInterrupt();
    //Timer0A_init();
		//Timer2A_init(); 
		Timer3A_init();
		dist = measureD();
		OutSignal(dist);
	  sprintf(distStr, "%d cm0     ", dist);
	  UART_OutString(distStr); 
		delay_Microsecond(10000);
  }
}