#include <TM4C123.h>
#include "hc-sr04.h"

void delay_Microsecond(uint32_t time);
void Timer0_init(void);

double round(double);
 
const double _16MHz_1clock = 0.0625; // Value of 1clock cycle in mikroseconds
 
#define ECHO (1U << 6) //PB6
#define TRIGGER (1U << 4) //PA4(OUTPUT)
#define BLUE_LED (1U << 2) //PF2 onboard Blue LED
#define GREEN_LED (1U << 3) //PF3 onboard Green LED
#define RED_LED (1U << 1) //PF1 onboard Red LED

int32_t measureDistanceOnce(void) { 
		uint32_t highEdge,lowEdge;
		int32_t ddistance; /*Distance in centimeters*/
    double travelTime = 0;
    GPIOA->DATA &=~TRIGGER;
    delay_Microsecond(12);
    GPIOA->DATA |= TRIGGER;
    delay_Microsecond(12);
    GPIOA->DATA &=~TRIGGER;
    /*Capture firstEgde i.e. rising edge*/
    TIMER0->ICR =4;
    while((TIMER0->RIS & 4)==0){}; //Wait till captured
		highEdge =  TIMER0->TAR;

		/*Capture secondEdge i.e. falling edge */
		TIMER0->ICR =4; //clear timer capture flag
		while((TIMER0->RIS & 4)  ==0){};
		lowEdge = TIMER0->TAR;
		travelTime = (lowEdge - highEdge) * _16MHz_1clock;
		if (travelTime < 58) {
			ddistance = -1;
		}
		else if (travelTime > 11662) {
			ddistance = 9999;
		}
		else {
		  ddistance = round((travelTime * 0.5) * 0.0343);
		}

    return ddistance;
}

int32_t measureD() {
   uint32_t counter = 0;
   int32_t dist = 0;
	 int32_t instantDist = 0;
	
	 for (counter = 0; counter < 100; ++counter) {
		 instantDist = measureDistanceOnce();
		 if (instantDist > 0 && instantDist < 9999) {
		    dist = (dist + instantDist) / 2;
		 }
	 }
	 
	 return dist;
}

void InitRegisters(){
    SYSCTL->RCGCGPIO |=(1U << 0); //Enable clock for PORTA 
    SYSCTL->RCGCGPIO |=(1U << 1); //Enable clock for PORTB 
    SYSCTL->RCGCGPIO |=(1U << 5); //Enable clock for PORTF 
    GPIOA->DIR |= TRIGGER;  
    GPIOF->DIR |= RED_LED | GREEN_LED | BLUE_LED;
    GPIOA->DEN |= TRIGGER;
	  GPIOB->DEN |= ECHO;
    GPIOF->DEN |= RED_LED | GREEN_LED | BLUE_LED;
}

void OutSignal(uint32_t value) {
	GPIOF->DATA &= ~(RED_LED | GREEN_LED | BLUE_LED);
	
  if(value < 15) { 
    GPIOF->DATA |= RED_LED;
	}
  else if (value < 50) {
    GPIOF->DATA |= RED_LED | BLUE_LED;
	}
	else if (value < 100) {
		GPIOF->DATA |= RED_LED | GREEN_LED;
	}
	else if (value < 200) {
		GPIOF->DATA |= BLUE_LED | GREEN_LED;
	}
	else if (value < 300) {
		GPIOF->DATA |= BLUE_LED;
	}
	else if (value < 400) {
		GPIOF->DATA |= GREEN_LED;
	}
	else {
		GPIOF->DATA |= RED_LED | GREEN_LED | BLUE_LED;
	}
}

/*
int main(void){
		InitRegisters();

    while(1){
        Timer0_init();
         measureD();
    OutSignal(measureD() < 15); 
   delay_Microsecond(100);
 
  }
}*/


void delay_Microsecond(uint32_t time)
{
    int i;
    SYSCTL_RCGCTIMER_R |=(1U << 1); 
    TIMER1_CTL_R=0;
    TIMER1_CFG_R=0x04;
    TIMER1_TAMR_R=0x02;
    TIMER1_TAILR_R= 16-1;
    TIMER1_ICR_R =0x1;
    TIMER1_CTL_R |=0x01;
 
    for(i = 0; i < time; i++){ 
       while((TIMER1_RIS_R & 0x1) == 0) {}
        TIMER1_ICR_R = 0x1;
    }
//		    int i;
//    SYSCTL->RCGCTIMER |= 1;
//    TIMER1_CTL_R=0;
//    TIMER1_CFG_R=0x04;
//    TIMER1_TAMR_R=0x02;
//    TIMER1_TAILR_R= 16-1;
//    TIMER1_ICR_R =0x1;
//    TIMER1_CTL_R |=0x01;
// 
////    for(i = 0; i < time; i++){ 
////       while((TIMER1_RIS_R & 0x1) == 0) {}
////        TIMER1_ICR_R = 0x1;
////			 }
// 
}
void Timer0_init(void)
{
    SYSCTL->RCGCTIMER |=(1U << 0); 
    SYSCTL->RCGCGPIO |=(1U << 1); 
    GPIOB->DIR &= ~ECHO;
    GPIOB->DEN |=ECHO;
    GPIOB->AFSEL |=ECHO;
    GPIOB->PCTL &= ~0x0F000000;
    GPIOB->PCTL |= 0x07000000;
 
    TIMER0->CTL &= ~1;
    TIMER0->CFG = 4;
    TIMER0->TAMR = 0x17;
    TIMER0->CTL |= 0x0C;
    TIMER0->CTL |= 1;
}
