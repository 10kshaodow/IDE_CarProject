// TimerA.c

/*  LJBeato
    2021
    TimerA functionality to drive DC motor
    and Servo Motor
 */


#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include "TimerA.h"
#include "uart.h"

// Make these arrays 5 deep, since we are using indexes 1-4 for the pins
static uint32_t DEFAULT_PERIOD_A0[5] = {0,0,0,0,0};
static uint32_t DEFAULT_PERIOD_A2[5] = {0,0,0,0,0};
//***************************PWM_Init*******************************
// PWM output on P2.4, P2.5, P2.6, P2.7
// Inputs:  period of P2.4...P2.7 is number of counts before output changes state
//          percentDutyCycle (0 -> 1.0)
//          pin number (1,2,3,4)
// Outputs: none


int TIMER_A0_PWM_Init(uint16_t period, double percentDutyCycle, uint16_t pin)
{
	uint16_t dutyCycle;
	// Timer A0.1
	if (pin == 1)
	{
		// Set the pin to output
		P2->DIR |= BIT4;
		// Set the pin to primary mode
		P2->SEL1 &= ~BIT4;
		P2->SEL0 |= BIT4;
	}
	else if (pin == 2)
	{
		// Set the pin to output
		P2->DIR |= BIT5;
		// Set the pin to primary mode
		P2->SEL1 &= ~BIT5;
		P2->SEL0 |= BIT5;
	}
	else if (pin == 3)
	{
		// Set the pin to output
		P2->DIR |= BIT6;
		// Set the pin to primary mode
		P2->SEL1 &= ~BIT6;
		P2->SEL0 |= BIT6;
	}
	else if (pin == 4)
	{
		// Set the pin to output
		P2->DIR |= BIT7;
		// Set the pin to primary mode
		P2->SEL1 &= ~BIT7;
		P2->SEL0 |= BIT7;
	}
	else return -2;
	
	// save the period for this timer instance
	// DEFAULT_PERIOD_A0[pin] where pin is the pin number
	DEFAULT_PERIOD_A0[pin] = period;
	// TIMER_A0->CCR[0]	
	TIMER_A0->CCR[0] = period;
	

	// TIMER_A0->CCTL[pin]
    TIMER_A0->CCTL[pin] &= ~BIT7;
		TIMER_A0->CCTL[pin] |= BIT6;
		TIMER_A0->CCTL[pin] &= ~BIT5;
	
	// set the duty cycle
	dutyCycle = (uint16_t)(percentDutyCycle * (double)DEFAULT_PERIOD_A0[pin]);

	// CCR[n] contains the dutyCycle just calculated, where n is the pin number
    //TIMER_A0->CCR[pin]
    TIMER_A0->CCR[pin] = dutyCycle;
	
	// Timer CONTROL register
	// TIMER_A0->CTL
	
	TIMER_A0->CTL |= BIT9;
	TIMER_A0->CTL &= ~BIT8; 
	TIMER_A0->CTL &= ~BIT7; 
	TIMER_A0->CTL &= ~BIT6; 
	TIMER_A0->CTL &= ~BIT5; 
	TIMER_A0->CTL |= BIT4; 
	TIMER_A0->CTL |= BIT1; 

	return 0;
}
//***************************PWM_Duty1*******************************
// change duty cycle of PWM output on pin
// Inputs:  dutycycle, pin
// Outputs: none
// percentDutyCycle is a number between 0 and 1  (ie. 0.5 = 50%)
void TIMER_A0_PWM_DutyCycle(double percentDutyCycle, uint16_t pin)
{
	// set the duty cycle
	uint16_t dutyCycle = (uint16_t) (percentDutyCycle * (double)DEFAULT_PERIOD_A0[pin]);
	// CCR[n] contains the dutyCycle just calculated, where n is the pin number
	//TIMER_A0->CCR[pin]
	TIMER_A0->CCR[pin] = dutyCycle;
}

//***************************PWM_Init*******************************
// PWM output on P5.6
// Inputs:  period of P5.6 is number of counts before output changes state
//          percentDutyCycle (0 -> 1.0)//          duty cycle
//          pin number (1,2,3,4), but always 1
// Outputs: none
int TIMER_A2_PWM_Init(uint16_t period, double percentDutyCycle, uint16_t pin)
{
	uint16_t dutyCycle;
	// NOTE: Timer A2 only exposes 1 PWM pin
	// TimerA2.1
	if (pin == 1)
	{
		// Set the pin to output
		P5->DIR |= BIT6;
		// Set the pin to primary mode
		P5->SEL1 &= ~BIT6;
		P5->SEL0 |= BIT6;
	}
	else return -2; 

    // NOTE: Setup similar to TimerA0
    // You will have to use the prescaler (clock divider) to get down to 20ms
	
		// save the period for this timer instance
		// DEFAULT_PERIOD_A2[pin] where pin is the pin number
		DEFAULT_PERIOD_A2[pin] = (SystemCoreClock/period)/64;
		// TIMER_A2->CCR[0]	
		TIMER_A2->CCR[0] = (SystemCoreClock/period)/64;
	

		// TIMER_A0->CCTL[pin]
    TIMER_A2->CCTL[pin] &= ~BIT7;
		TIMER_A2->CCTL[pin] |= BIT6;
		TIMER_A2->CCTL[pin] &= ~BIT5;
	
	// set the duty cycle
	dutyCycle = (uint16_t)(percentDutyCycle * (double)DEFAULT_PERIOD_A2[pin]);

	// CCR[n] contains the dutyCycle just calculated, where n is the pin number
    //TIMER_A0->CCR[pin]
    TIMER_A2->CCR[pin] = dutyCycle;
	
	// Timer CONTROL register
	// TIMER_A2->CTL
	
	TIMER_A2->CTL |= BIT9;
	TIMER_A2->CTL &= ~BIT8; 
	TIMER_A2->CTL |= BIT7; 
	TIMER_A2->CTL |= BIT6; 
	TIMER_A2->CTL &= ~BIT5; 
	TIMER_A2->CTL |= BIT4; 
	TIMER_A2->CTL |= BIT1; 
	
	TIMER_A2->EX0 |= BIT2;
	TIMER_A2->EX0 |= BIT1;
	TIMER_A2->EX0 |= BIT0;
	
	
	return 0;
}
//***************************PWM_Duty1*******************************
// change duty cycle of PWM output on P5.6
// Inputs:  percentDutyCycle, pin  (should always be 1 for TimerA2.1)
//         
// Outputs: none
// 
void TIMER_A2_PWM_DutyCycle(double percentDutyCycle, uint16_t pin)
{
	// set the duty cycle
	uint16_t dutyCycle = (uint16_t) (percentDutyCycle * (double)DEFAULT_PERIOD_A2[pin]);
	// CCR[n] contains the dutyCycle just calculated, where n is the pin number
	//TIMER_A2->CCR[pin]
	TIMER_A2->CCR[pin] = dutyCycle;
}
