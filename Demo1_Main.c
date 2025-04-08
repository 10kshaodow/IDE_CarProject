#include <stdio.h>
#include <stdlib.h>

#include "motors.h"
#include "switches.h"

//Files for Camera
#include "ADC14.h"
#include "ControlPins.h"
#include "CortexM.h"
#include "uart.h"

#include "Common.h"



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
	uart2_init();
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
		//int test_num = 0;
		uint16_t min_val = 16383;
		uint16_t max_val = 0;
		uint16_t dynamic_threshold = 0;  // initialize it here
		//char debugStr[64];

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
//		test_num = 	max_val - min_val;
//		sprintf(debugStr, "max_val:%d min_val:%d test_num:%d", max_val, min_val, test_num);
//		uart2_put(debugStr);
//		uart2_put("\r\n");
		
		if ((max_val - min_val) < 2500) {  // average for IoT was 2400
			stop_motors();
			break;
		}
			// 3. Sum regions
			
			for ( i = 0; i < 42; i++) {left_sum += bintrace[i];}
			for ( i = 56; i <= 71; i++){ center_sum += bintrace[i];}
			for ( i = 85; i < 128; i++) {right_sum += bintrace[i];}

			// 4. Decision logic based on region sums
			if ((center_sum < 10)|| (right_sum < 10 ) ||(left_sum < 10)) {
					if ((left_sum > right_sum) ) {	
						
							servo_left();          // white is mostly on left
							move_forward(0.2);
							myDelay1(0.7); // normal
							move_forward(0.23);
							myDelay1(0.25); // speed f
						

					} else if ((right_sum > left_sum)) {
							
							servo_right();         // white is mostly on right
							move_forward(0.2);
							myDelay1(0.7);
							move_forward(0.23);
							myDelay1(0.25); // speed f
					} else {
							stop_motors();         // track lost
							break;
					}
			} else {
					servo_center();            // track is centered
			}
//				sprintf(debugStr, "L:%d C:%d R:%d", left_sum, center_sum, right_sum);
//				uart2_put(debugStr);  // Send over UART2 (Bluetooth)
//				uart2_put("\r\n");    // New line for clarity			
		}

move_forward(0.2); // always move forward unless stopped above
			
		}else if(Switch2_Pressed() == TRUE){
				stop_motors();
				break;
		}
		
}

return 0;
}
