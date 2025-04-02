#include <stdio.h>
#include <stdlib.h>

#include "motors.h"


int main(){
 init_motors();
	
	
while(1){
move_forward(0.1);
}

return 0;
}