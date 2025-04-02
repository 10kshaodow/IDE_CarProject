#ifndef MOTORS_H
#define MOTORS_H

#include "msp.h"
#include "Common.h"

// Function Prototypes
void init_motors(void);

void move_forward(double speed);

void move_backward(void);

void stop_motors(void);

void servo_left(void);

void servo_right(void);

void servo_center(void);

#endif /* MOTORS_H */