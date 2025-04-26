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
#include "CarFunctions.h"

// === EXTERNAL VARIABLES ===
extern BOOLEAN g_sendData;
extern uint16_t line[128];       // Raw ADC values (brightness levels)

// === GLOBAL VARIABLES ===
extern char debugStr[128];       // Debug string
extern uint8_t bintrace[128];    // Thresholded binary values

void run_fastcar() {
	  uint16_t min_val = 0;
    uint16_t max_val = 0;
    uint16_t dynamic_threshold = 0;

    float Vdes = 62; //originally 64

    float kp = 0.58;		//0.6
    float ki = 0.02;	//0.02
    float kd = 0.6;	//0.15 **0.6**

    float controlOld = 0;
    float errOld1 = 0;
    float errOld2 = 0;

    float Vact = 0;
    float err = 0;
    float control = 0;

	float base_speed = 0.5;		// was 0.68, cannot be higher than 0.5 :(
    float min_speed = 0.34;	// This speed is probably the one, 0.35 orig. 0.32 works
    float speed_range = base_speed - min_speed;
    float dynamic_speed = 0;
    float control_intensity = 0;
		float speedfactor = 0;

    uint8_t smoothed_trace[128]; // smoothed array
		
		while (1) {
			int i = 0;
			if (g_sendData == TRUE) {
					g_sendData = FALSE;

					min_val = 16383;
					max_val = 0;

					for (i = 0; i < 128; i++) {
							if (line[i] < min_val) min_val = line[i];
							if (line[i] > max_val) max_val = line[i];
					}

					dynamic_threshold = (max_val + min_val) / 2;

					for (i = 0; i < 128; i++) {
							bintrace[i] = (line[i] > dynamic_threshold) ? 1 : 0;
					}

					smooth_bintrace(bintrace, smoothed_trace, 128, 7);
					memcpy(bintrace, smoothed_trace, sizeof(bintrace));

					//if ((max_val - min_val) < 4500) {
					//		stop_motors(); // Off track
					//		break;
					//}

					Vact = compute_line_position();
					err = Vdes - Vact;

					control = controlOld + kp * (err - errOld1) +
										ki * ((err + errOld1) / 2.0f) +
										kd * (err - 2.0f * errOld1 + errOld2);

					if (control > 1) control = 1;
					if (control < -1) control = -1;
					
					speedfactor = 1 - (dynamic_speed / base_speed);

					if (Vact > 60.5f && Vact < 64.5f) { // centering position on line
							servo_center();
					} else {
							adjust_dual_steering(control, speedfactor);
					}

					controlOld = control;
					errOld2 = errOld1;
					errOld1 = err;

					control_intensity = fabsf(control);
					//dynamic_speed = base_speed * expf(-8.0f * control_intensity); // adjust -3.0f to control severity
					dynamic_speed = base_speed - powf(control_intensity, 3.5f) * speed_range; // THIS WORKS VERY WELL
					//dynamic_speed = base_speed - (control_intensity * control_intensity * speed_range);
					//dynamic_speed = base_speed - (control_intensity * speed_range);
					if (dynamic_speed < min_speed) dynamic_speed = min_speed;
					if (dynamic_speed > base_speed) dynamic_speed = base_speed;

					move_forward(dynamic_speed);
		}
	}
}

