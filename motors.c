#include <stdio.h>
#include <stdlib.h>

#include "ADC14.h"
#include "Common.h"
#include "ControlPins.h"
#include "CortexM.h"
#include "leds.h"
#include "msp.h"
#include "switches.h"
#include "SysTickTimer.h"
#include "TimerA.h"
#include "uart.h"


void init_motors(void){
	// Init timers for DC motors
    uint16_t freq = 10000; // Frequency = 10 kHz 
    uint16_t period = SystemCoreClock / freq;
    TIMER_A0_PWM_Init(SystemCoreClock/period, 0.0, 1);
    TIMER_A0_PWM_Init(SystemCoreClock/period, 0.0, 2);
    TIMER_A0_PWM_Init(SystemCoreClock/period, 0.0, 3);
    TIMER_A0_PWM_Init(SystemCoreClock/period, 0.0, 4);

    TIMER_A2_PWM_Init(50, (1-0.075), 1); // 50Hz frequency, 7.5% duty cycle (1.5ms pulse width)
}

void move_forward(double speed){
	TIMER_A0_PWM_DutyCycle(0.0, 1);	// Left wheel forwards
	TIMER_A0_PWM_DutyCycle(speed, 2);
	TIMER_A0_PWM_DutyCycle(0.0, 3);// Right wheel forwards
	TIMER_A0_PWM_DutyCycle(speed, 4);
}

void move_backward(void){
	TIMER_A0_PWM_DutyCycle(0.2, 1);	// Left wheel backwards
	TIMER_A0_PWM_DutyCycle(0.0, 2);
	TIMER_A0_PWM_DutyCycle(0.2, 3);	// Right wheel backwards
	TIMER_A0_PWM_DutyCycle(0.0, 4);
}

void stop_motors(void){
	TIMER_A0_PWM_DutyCycle(0.0, 1); // Left wheel stop
	TIMER_A0_PWM_DutyCycle(0.0, 2);
	TIMER_A0_PWM_DutyCycle(0.0, 3); // Right wheel stop
	TIMER_A0_PWM_DutyCycle(0.0, 4);
	//Disable Motor
	P3->OUT &= ~BIT6;
	P3->OUT &= ~BIT7;
}

void servo_left(void){
    TIMER_A2_PWM_DutyCycle(1-0.05, 1);
}

void servo_right(void){
    TIMER_A2_PWM_DutyCycle(1-0.1, 1);
}

void servo_center(void){
    TIMER_A2_PWM_DutyCycle(1-0.075, 1);
}
