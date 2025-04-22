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

extern uint16_t line[128]; // raw ADC values (brightness levels)
uint8_t bintrace[128];     // thresholded binary values (1 or 0)
extern BOOLEAN g_sendData; 
char debugStr[128];

// === NEW FUNCTION ===
void smooth_bintrace(uint8_t* input, uint8_t* output, int length, int window_size) {
    int half_window = window_size / 2;
    int i, j;
		float average;

    for (i = 0; i < length; i++) {
        int sum = 0;
        int count = 0;

        for (j = -half_window; j <= half_window; j++) {
            int idx = i + j;
            if (idx >= 0 && idx < length) {
                sum += input[idx];
                count++;
            }
        }

        average = (float)sum / count;
        output[i] = (average >= 0.5f) ? 1 : 0;
    }
}

void myDelay1(float k) {
    int counter = 1000000;
    volatile int j = 0;
    counter = k * counter;
    for (j = 0; j < (int)counter; j++) {
        ;
    }
}

float compute_line_position() {
    float sum = 0, weighted_sum = 0;
    int k = 0;
    for (k = 0; k < 128; k++) {
        if (bintrace[k]) {
            weighted_sum += k;
            sum += 1;
        }
    }
    if (sum == 0) return 64.0; // fallback to center if line is lost
    return weighted_sum / sum;
}

void adjust_steering(float control) {
    float base_duty = 0.075;  // 7.5% center (1.5ms pulse)
    float max_offset = 0.025; // ±2.5% for left/right steering
    float offset;
    float duty;

    offset = (control / 1.0f) * max_offset;
    duty = base_duty + offset;

    TIMER_A2_PWM_DutyCycle(1.0f - duty, 1);
}

int main() {
    uint16_t min_val = 0;
    uint16_t max_val = 0;
    uint16_t dynamic_threshold = 0;

    float Vdes = 64.0;

    float kp = 0.8;
    float ki = 0.02;
    float kd = 0.17;

    float controlOld = 0;
    float errOld1 = 0;
    float errOld2 = 0;

    float Vact = 0;
    float err = 0;
    float control = 0;

    float base_speed = 0.55;
    float min_speed = 0.3;
    float speed_range = base_speed - min_speed;
    float dynamic_speed = 0;
		float control_intensity = 0;

    uint8_t smoothed_trace[128]; // === New smoothed array ===

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

    while (!Switch1_Pressed()) {}

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

            // === Apply smoothing here ===
            smooth_bintrace(bintrace, smoothed_trace, 128, 5);
            memcpy(bintrace, smoothed_trace, sizeof(bintrace)); // Copy smoothed values back

            if ((max_val - min_val) < 2500) {
                stop_motors(); // Off track
                break;
            }

            Vact = compute_line_position();
            err = Vdes - Vact;

            control = controlOld + kp * (err - errOld1) +
                      ki * ((err + errOld1) / 2.0f) +
                      kd * (err - 2.0f * errOld1 + errOld2);

            if (control > 1) control = 1;
            if (control < -1) control = -1;

            adjust_steering(control);

            controlOld = control;
            errOld2 = errOld1;
            errOld1 = err;

						control_intensity = fabsf(control);
						dynamic_speed = base_speed - (control_intensity * control_intensity * speed_range);
            //dynamic_speed = base_speed - (fabsf(control) * speed_range);
            if (dynamic_speed < min_speed) dynamic_speed = min_speed;
            if (dynamic_speed > base_speed) dynamic_speed = base_speed;

            move_forward(dynamic_speed); // Fixed forward speed (you can switch to dynamic_speed if needed)
        }

        if (Switch2_Pressed() == TRUE) {
            stop_motors();
            break;
        }
    }

    return 0;
}
