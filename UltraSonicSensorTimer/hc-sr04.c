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
    GPIO_PORTA_DATA_R &=~TRIGGER;
    delay_Microsecond(12);
    GPIO_PORTA_DATA_R |= TRIGGER;
    delay_Microsecond(12);
    GPIO_PORTA_DATA_R &=~TRIGGER;
    /*Capture firstEgde i.e. rising edge*/
    TIMER0_ICR_R =4;
    while((TIMER0_RIS_R & 4)==0){}; //Wait till captured
		highEdge =  TIMER0_TAR_R;

		/*Capture secondEdge i.e. falling edge */
		TIMER0_ICR_R =4; //clear timer capture flag
		while((TIMER0_RIS_R & 4)  ==0){};
		lowEdge = TIMER0_TAR_R;
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
    SYSCTL_RCGCGPIO_R |=(1U << 0); //Enable clock for PORTA 
    SYSCTL_RCGCGPIO_R |=(1U << 1); //Enable clock for PORTB 
    SYSCTL_RCGCGPIO_R|=(1U << 5); //Enable clock for PORTF 
    GPIO_PORTA_DIR_R |= TRIGGER;  
    GPIO_PORTF_DIR_R |= RED_LED | GREEN_LED | BLUE_LED;
    GPIO_PORTA_DEN_R |= TRIGGER;
	  GPIO_PORTB_DEN_R |= ECHO;
    GPIO_PORTF_DEN_R |= RED_LED | GREEN_LED | BLUE_LED;
}

void OutSignal(uint32_t value) {
	GPIO_PORTF_DATA_R &= ~(RED_LED | GREEN_LED | BLUE_LED);
	
  if(value < 15) { 
    GPIO_PORTF_DATA_R |= RED_LED;
	}
  else if (value < 50) {
    GPIO_PORTF_DATA_R |= RED_LED | BLUE_LED;
	}
	else if (value < 100) {
		GPIO_PORTF_DATA_R |= RED_LED | GREEN_LED;
	}
	else if (value < 200) {
		GPIO_PORTF_DATA_R |= BLUE_LED | GREEN_LED;
	}
	else if (value < 300) {
		GPIO_PORTF_DATA_R |= BLUE_LED;
	}
	else if (value < 400) {
		GPIO_PORTF_DATA_R |= GREEN_LED;
	}
	else {
		GPIO_PORTF_DATA_R |= RED_LED | GREEN_LED | BLUE_LED;
	}
}


void delay_Microsecond(uint32_t time)
{
    int i;
    SYSCTL_RCGCTIMER_R |= 0x02; 
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
}
void Timer0_init(void)
{
    SYSCTL_RCGCTIMER_R |= 0x01; 
    SYSCTL_RCGCGPIO_R |= 0x02; 
    GPIO_PORTB_DIR_R &= ~ECHO;
    GPIO_PORTB_DEN_R |=ECHO;
    GPIO_PORTB_AFSEL_R |=ECHO;
    GPIO_PORTB_PCTL_R &= ~0x0F000000;
    GPIO_PORTB_PCTL_R |= 0x07000000;
 
    TIMER0_CTL_R &= ~1;
    TIMER0_CFG_R = 4;
    TIMER0_TAMR_R = 0x17;
    TIMER0_CTL_R |= 0x0C;
    TIMER0_CTL_R |= 1;
}
