#include <stdio.h>
#include <stdlib.h>

#include "motors.h"
#include "switches.h"

//Files for Camera
#include "ADC14.h"
#include "ControlPins.h"
#include "CortexM.h"

#include "Common.h"
#define SMOOTHING 1

#define THRESHOLD 2000



///////////////////////////////////////////////////////
//
// NOTE: For the camera, you may want to change the default
//       clock to 48MHz
//
// To do that: Edit system_msp432p401r.c
//             Change:   #define  __SYSTEM_CLOCK    3000000
//             To:       #define  __SYSTEM_CLOCK    48000000 
// ADC will be P4.7 A6
//
// SI Pin will be P5.5 A0
//
// CLK Pin will be P5.4 A1//
//

// line stores the current array of camera data
extern uint16_t line[128]; // raw ADC values (brightness levels)
uint8_t bintrace[128]; //just the thresholded result, either 1 (white) or 0 (dark).
extern BOOLEAN g_sendData; 


void myDelay1(float k)
{
	int counter = 1000000;
	volatile int j = 0;
	counter = k * counter;
	for (j = 0; j < (int)counter; j++)
	{
		;
	}
}

int main(){
	DisableInterrupts();
	init_motors();
	Switch2_Init();
	g_sendData = FALSE;
	ControlPin_SI_Init();
	ControlPin_CLK_Init();
	ADC0_InitSWTriggerCh6();
	EnableInterrupts();
	

	while(1){

		int left_sum = 0; 
		int	center_sum = 0;
		int right_sum = 0;
		int i = 0;
		uint16_t min_val = 16383;
		uint16_t max_val = 0;
		uint16_t dynamic_threshold = 0;  // initialize it here

		if(Switch1_Pressed() == TRUE){
			if(g_sendData == TRUE){
				g_sendData = FALSE;
				
			
			for (i = 0; i < 128; i++) {
				if (line[i] < min_val) min_val = line[i];
				if (line[i] > max_val) max_val = line[i];
			}
			dynamic_threshold = (max_val + min_val) / 2;
			for (i = 0; i < 128; i++) {
				if (line[i] > dynamic_threshold) {
					bintrace[i] = 1;
				} else {
					bintrace[i] = 0;
				}
			}
			
			// 3. Sum regions
			
			for ( i = 0; i < 42; i++) {left_sum += bintrace[i];}
			for ( i = 56; i <= 71; i++){ center_sum += bintrace[i];}
			for ( i = 85; i < 128; i++) {right_sum += bintrace[i];}

			// 4. Decision logic based on region sums
			if (center_sum < 3) {
					if (left_sum > right_sum) {
							
							servo_left();          // white is mostly on left
							move_forward(0.15);
							myDelay1(0.6); // normal
							move_forward(0.22);
							myDelay1(0.3); // speed f
						

					} else if (right_sum > left_sum) {
							
							servo_right();         // white is mostly on right
							move_forward(0.15);
							myDelay1(0.6);
							move_forward(0.22);
							myDelay1(0.3); // speed f
					} else {
							stop_motors();         // track lost
							break;
					}
			} else {
					servo_center();            // track is centered
			}

move_forward(0.2); // always move forward unless stopped above
			
		}else if(Switch2_Pressed() == TRUE){
				stop_motors();
				break;
		}
}
}
return 0;
}
