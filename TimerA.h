/*
 * File:        TimerA.h
 * Purpose:     TimerA function prototypes and constants
 *
 */

#ifndef TIMERA_H_
#define TIMERA_H_

#include <stdint.h>
#include "Common.h"


// Function prototypes
int TIMER_A0_PWM_Init(uint16_t period, double percentDutyCycle, uint16_t pin);
void TIMER_A0_PWM_DutyCycle(double percentDutyCycle, uint16_t pin);
int TIMER_A2_PWM_Init(uint16_t period, double percentDutyCycle, uint16_t pin);
void TIMER_A2_PWM_DutyCycle(double percentDutyCycle, uint16_t pin);

#endif /* TIMERA_H_ */
