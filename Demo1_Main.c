#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "motors.h"
#include "switches.h"

// Files for Camera
#include "ADC14.h"
#include "ControlPins.h"
#include "CortexM.h"
#include "uart.h"
// Files for turning
#include "TimerA.h"

#include "Common.h"

#include "FastCar.h"
#include "SafeCar.h"

extern uint16_t line[128]; // raw ADC values (brightness levels)
extern BOOLEAN g_sendData; 


int main() {
    DisableInterrupts();
    init_motors();
    uart2_init();
    Switch2_Init();
    Switch1_Init();
    g_sendData = FALSE;
    ControlPin_SI_Init();
    ControlPin_CLK_Init();
    ADC0_InitSWTriggerCh6();
    EnableInterrupts();

	while (!Switch1_Pressed() && !Switch2_Pressed()) {}

	if (Switch1_Pressed()) {
    while (1) {
        run_fastcar();
    }
	}
	else if (Switch2_Pressed()) {
    while (1) {
        run_safecar();
    }
	}		    
	
	return 0;
}
