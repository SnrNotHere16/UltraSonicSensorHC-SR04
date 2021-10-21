/********************************
	TM4C123 microboard project and adafruit thumbstick 
	**Reads input from an analog thumbstick (x and y direction) 
		and alters the LED based on the thumbsticks position.
		1. The x direction conditions. 
			a. The positive x direction will make the LED(PortF) Red (0x02).
			b. The negative x direction will make the LED(PORTF) Green (0x08).
			c. The center x direction will make the LED (PORTF) no color (0x00).
		2. The y direction conditions. 
			a. The positive y direction will make the LED(PortF) BLUE (0x04).
			b. The negative y direction will make the LED(PORTF) BLUE (0x04).
			c. The center y direction will make the LED (PORTF) no color (0x00).
		PE1 - X diretion 
		PE2 - Y direction 
		

********************************/
#include <stdio.h>
#include <stdint.h>
//#include "PLL.h"
#include "tm4c123gh6pm.h"
volatile unsigned long ADCvalueY;
volatile unsigned long ADCvalueX;


void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;      // 1) F clock   used to turn on a specific port F E D C B A  in this case 1 0 0 0 0 0, F = 1 so port F is turned on
  delay = SYSCTL_RCGC2_R;            // delay - two clock cycles
  GPIO_PORTF_LOCK_R = 0x4C4F434B;    // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R |= 0x0E;           // allow changes to PF2-PF0       
  GPIO_PORTF_AMSEL_R &= ~0x0E;       // 3) disable analog function  - set all to 0
  GPIO_PORTF_PCTL_R &= ~0x0000000E;   // 4) GPIO clear bit PCTL - set all to 0 - Same as Ox0 but Ox00000000 tells user its 32 bits long
  GPIO_PORTF_DIR_R |= 0x0E;          // 5) PF3,PF2,PF1 are output (LED)-  0 0 0 0 1 1 1 0  
  GPIO_PORTF_AFSEL_R &= ~0x0E;       // 6) no alternate function - set all to 0
  GPIO_PORTF_DEN_R |= 0x0E;          // 7) enable digital pins PF4-PF0 - 0 0 0 1 1 1 1 1 enable all pins      
}

void ADC1_InitSWTriggerSeq2PE1(void){ volatile unsigned long delay;
	SYSCTL_RCGCADC_R |= 0x02;
  SYSCTL_RCGC2_R |= 0x00000010;   							// 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;         							//  delay
  GPIO_PORTE_DIR_R &= ~0x06;      							// 2) make PE1 input
  GPIO_PORTE_AFSEL_R |= 0x06;    								// 3) enable alternate function on PE1,PE2
  GPIO_PORTE_DEN_R &= ~0x06;      							// 4) disable digital I/O on PE1, PE2
  GPIO_PORTE_AMSEL_R |= 0x06;     							// 5) enable analog function on PE1, PE2
  SYSCTL_RCGC0_R |= 0x00020000;   							// 6) enable ADC1 (pg 458)
  delay = SYSCTL_RCGC2_R;         
  SYSCTL_RCGC0_R &= ~0x00000300;  							// 7) configure for 125K  (pg458)
  ADC1_SSPRI_R = 0x0123;          							// 8) Sequencer 2 is highest priority (pg841)
																								//  [3:0] - disable(0) enable(1) ASEN0-ASEN3
  ADC1_EMUX_R |= 0x0F00;         							// 9) seq2 is always (continuosly sampling) (pg 833)
																								// [15:12]- seq3, [11:8] - seq2, [7:4]-seq1, [3:0] - seq0
  ADC1_SSMUX2_R |= 	0x0000021;									// 10) channel Ain2 (PE1, 2)  Ain1 (PE2, 1) pg801, pg875
  ADC1_SSCTL2_R |= 0x0060;         						  // 11) disable TS0 D0, enable IE0 END0, pg876
  ADC1_ACTSS_R |= 0x0004;         							// 12) enable sample sequencer 2 (pg821)
	ADC1_IM_R = 0x00000004;  											// enable interrupt ss2, pg825
	NVIC_PRI4_R = ( NVIC_PRI4_R & 0xFFFF0FFF ) | 0x00002000;	// Set interrupt priority to 1
	NVIC_EN1_R          |=  0x00040000;           // pg 104, 134, 142
}






int main(void){unsigned long volatile delay;
	ADC1_InitSWTriggerSeq2PE1();         // ADC1 initialization PE1/AIN2
	PortF_Init();      									 //intialize LEDS
  while(1){
	}
}

void ADC1Seq2_Handler(void){
uint32_t Adirection[2];  
  ADC1_PSSI_R = 0x0004;            // 1) initiate SS2 (pg845)
  while((ADC1_RIS_R&0x04)==0){};   // 2) wait for conversion done ADC raw interrupt status, 
																	 //    checks for SS2, pg823

		Adirection[0] = ADC1_SSFIFO2_R&0xFFF;  // read second result, y direction read.
		Adirection[1] = ADC1_SSFIFO2_R&0xFFF;  // read first result, x direction read. 
		

		if (Adirection[0] < 409)
			GPIO_PORTF_DATA_R = 0x00;
		else if(Adirection[0] <818)
			GPIO_PORTF_DATA_R = 0x02;
		else if (Adirection[0] < 1227)
			GPIO_PORTF_DATA_R = 0x04;
	else if (Adirection[0] < 1636)
		GPIO_PORTF_DATA_R = 0x08;
else if (Adirection[0] < 2454)
	GPIO_PORTF_DATA_R = 0x0A;
	else if (Adirection[0] < 2863)
		GPIO_PORTF_DATA_R = 0x0B;
		else if (Adirection[0] < 3272)
			GPIO_PORTF_DATA_R = 0x0F;
			else if (Adirection[0] < 3681)
				GPIO_PORTF_DATA_R = 0x0C;
				else 
					GPIO_PORTF_DATA_R = 0x0D;

	ADC1_ISC_R = 0x0004;             // 4) clear interrupt, ADC interrupt status and clear, pg 803,823  
}
