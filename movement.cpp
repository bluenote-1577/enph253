// 
// 
// 

#include "movement.h"

/**
Function for turning. Called in follow_tape_intersection().
Goes back into tape folloing after this function is called.

Can only take 3 parameters, LEFT, RIGHT, or FORWARD. These parameters are defined
as integers at the top.
**/
void turn(int DIRECTION) {
	if (DIRECTION == FORWARD) {
		return;
	}

	bool has_turned = false;

	if (DIRECTION == RIGHT) {
		motor.speed(0, TURN_SPEED);
		motor.speed(1, -TURN_SPEED);
	}

	else {
		motor.speed(0, -TURN_SPEED);
		motor.speed(1, TURN_SPEED);
	}

	delay(100);

	bool is_on_tape = true;
	while (is_on_tape) {
		is_on_tape = (digitalRead(LEFT_TAPEFOLLOWER) || digitalRead(RIGHT_TAPEFOLLOWER));
	}

	while (!is_on_tape) {
		is_on_tape = (digitalRead(LEFT_TAPEFOLLOWER) && digitalRead(RIGHT_TAPEFOLLOWER));
	}

}

void collision_turn_around() {
	motor.speed(0, -TURN_SPEED/3);
	motor.speed(1, -TURN_SPEED);
	delay(1100);

	motor.speed(0, TURN_SPEED);
	motor.speed(1, -TURN_SPEED);

	bool is_on_tape = true;

	while (is_on_tape) {
		is_on_tape = (digitalRead(LEFT_TAPEFOLLOWER) || digitalRead(RIGHT_TAPEFOLLOWER));
	}

	while (!is_on_tape) {
		is_on_tape = (digitalRead(LEFT_TAPEFOLLOWER) && digitalRead(RIGHT_TAPEFOLLOWER));
	}
}
