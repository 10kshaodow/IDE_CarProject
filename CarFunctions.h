#ifndef CARFUNCTIONS_H
#define CARFUNCTIONS_H

#include <stdint.h>
#include <stdbool.h> // for BOOLEAN

// === EXTERNAL VARIABLES ===
extern uint16_t line[128];       // Raw ADC values (brightness levels)

// === GLOBAL VARIABLES ===
extern char debugStr[128];       // Debug string
extern uint8_t bintrace[128];    // Thresholded binary values

// === FUNCTION DECLARATIONS ===

/**
 * Applies a smoothing window to binary input values.
 * @param input Binary input array
 * @param output Smoothed binary output array
 * @param length Length of the arrays
 * @param window_size Size of the smoothing window (must be odd)
 */
void smooth_bintrace(uint8_t* input, uint8_t* output, int length, int window_size);

/**
 * Delays the program execution for a specified multiplier of a base time.
 * @param k Delay multiplier (e.g., 1.0 for full delay, 0.5 for half)
 */
void myDelay1(float k);

/**
 * Computes the position of the detected line based on the binary trace.
 * @return A float representing the average position of the line
 */
float compute_line_position(void);

/**
 * Adjusts the front servo motor based on control input.
 * @param control Control value in range [-1.0, 1.0]
 */
void adjust_front_steering(float control);

/**
 * Adjusts the rear servo motor based on control input.
 * @param control Control value in range [-1.0, 1.0]
 */
void adjust_rear_steering(float control, float speedfactor);

/**
 * Adjusts both front and rear servos for coordinated steering.
 * @param control Control value in range [-1.0, 1.0]
 */
void adjust_dual_steering(float control, float speedfactor);

void adjust_steering(float control);

#endif // CARFUNCTIONS_H
