#include <stdio.h>
#include <stdlib.h>

#include "motors.h"
#include "switches.h"


void myDelay1(void)
{
	volatile int j = 0;
	for (j = 0; j < 800000; j++)
	{
		;
	}
}
int main(){
	
	init_motors();
	servo_left();
	myDelay1();
	servo_right();
	myDelay1();
	servo_center();
	

while(1){	
if(Switch1_Pressed()){
	move_forward(0.1);
	
}else if(Switch2_Pressed()){
	stop_motors();
}
}

return 0;
}