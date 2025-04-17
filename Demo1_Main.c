#include <stdio.h>
#include <stdlib.h>

#include "motors.h"
#include "switches.h"

//Files for Camera
#include "ADC14.h"
#include "ControlPins.h"
#include "CortexM.h"
#include "uart.h"
//Files for turning
#include "TimerA.h"

#include "Common.h"



// line stores the current array of camera data
extern uint16_t line[128]; // raw ADC values (brightness levels)
uint8_t bintrace[128]; //just the thresholded result, either 1 (white) or 0 (dark).
extern BOOLEAN g_sendData; 
char debugStr[128];

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
float compute_line_position() {
    float sum = 0, weighted_sum = 0;
		int k = 0;
    for ( k = 0; k < 128; k++) {
        if (bintrace[k]) {
            weighted_sum += k;
            sum += 1;
        }
    }
    if (sum == 0) return 64.0; // fallback to center if line is lost
    return weighted_sum / sum;
}

void adjust_steering(float control) {
    float base_duty = 0.075;        // 7.5% center (1.5ms pulse)
    float max_offset = 0.025;       // ±2.5% for left/right steering
    float offset;
    float duty;

    offset = (control / 1.0f) * max_offset;  // convert control to offset
    duty = base_duty + offset;                 // adjust from center

    TIMER_A2_PWM_DutyCycle(1.0f - duty, 1);    // send to servo (inverted)
}

int main(){
	
	uint16_t min_val = 0;
  uint16_t max_val = 0;
	uint16_t dynamic_threshold = 0;  // initialize it here
	
	float Vdes = 63.5;
	
	float kp = 0.45;
	float ki = 0.0;
	float kd = 0.0;
	
	float controlOld = 0;
	float errOld1 = 0; 
	float errOld2 = 0;
	
	float Vact = 0;
  float err = 0;
  float control = 0;

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
while (!Switch1_Pressed())	{}
	
	while(1){
		
		int i = 0;

//		sprintf(debugStr, "Vact:%f err:%f control:%f", Vact, err, control);
//		uart2_put(debugStr);
//		uart2_put("\r\n");
//		debugStr[0] = '\0';
//		
		
			if(g_sendData == TRUE){
				g_sendData = FALSE;
				
				 min_val = 16383;
				 max_val = 0;
				
			
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
			stop_motors(); // off track stop
			break;
		}
		
		 Vact = compute_line_position();
     err = Vdes - Vact;
		
		control = controlOld +  kp * (err - errOld1) + ki * ((err + errOld1) / (float)2.0) + kd * (err - (float)2.0 * errOld1 + errOld2);
		
   if (control > 1){ control = 1;}
   if (control < -1){ control = -1;}
		 
     adjust_steering(control);

     controlOld = control;
     errOld2 = errOld1;
     errOld1 = err;
			
		 move_forward(0.2); // always move forward unless stopped above	
	}
  if(Switch2_Pressed() == TRUE){
				stop_motors();
				break;
		}
		
}

return 0;
}
