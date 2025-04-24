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

#ifndef fminf
#define fminf(a, b) ((a) < (b) ? (a) : (b))
#endif


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
    if (sum == 0) return 62.0; // fallback to center if line is lost
    return weighted_sum / sum;
}

void adjust_front_steering(float control) {
    float base_duty = 0.075;  // 7.5% center (1.5ms pulse)
    float max_offset = 0.025; // ±2.5% for left/right steering
    float offset;
    float duty;

    offset = (control / 1.0f) * max_offset;
    duty = base_duty + offset;

    TIMER_A2_PWM_DutyCycle(1.0f - duty, 1); // front servo
}

void adjust_rear_steering(float control, float speedfactor) {
    float base_duty = 0.075f;
    float max_offset = 0.025f;
    float offset;
    float duty;

    float scaled_control = -control * fminf(1.0f, speedfactor); // Limit rear input at high speed
    offset = scaled_control * max_offset;
    duty = base_duty + offset;

    TIMER_A2_PWM_DutyCycle(1.0f - duty, 2);
}


void adjust_dual_steering(float control, float speedfactor) {
    adjust_front_steering(control);
    adjust_rear_steering(control, speedfactor);
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
