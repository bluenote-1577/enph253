// 
// 
// 

#include "navigation.h"

void Navigation::navigate(bool front_intersection_valid,
	bool left_intersection_valid,
	bool right_intersection_valid,
	volatile unsigned long &detection_state) {

	bool correct_turn;
	turn_number++;

	if (navigation_mode == RANDOM_LOST) {
		navigate_random(front_intersection_valid, left_intersection_valid, right_intersection_valid);
	}

	else if (navigation_mode == FIXEDPATH_RIGHT) {
		correct_turn = navigate_fixedpath_right(front_intersection_valid, 
			left_intersection_valid, right_intersection_valid);
	}

	else if (navigation_mode == FIXEDPATH_LEFT) {
		correct_turn = navigate_fixedpath_left(front_intersection_valid, 
			left_intersection_valid, right_intersection_valid);
	}

	if (!correct_turn) {
		LCD.clear();
		LCD.print("INTERSECTION PROBLEM");
		LCD.print(turn_number);
		detection_state = STOP;
	}

	if (turn_number > 69) {
		detection_state = STOP;
		LCD.clear();
		LCD.print("DONE");
	}

	if (detection_state != STOP) {
		detection_state = FOLLOWING_TAPE;
	}

	intersection_turn_timer = millis();

}

void Navigation::navigate_random(bool front_intersection_valid,
	bool left_intersection_valid,
	bool right_intersection_valid) {

	//initiate random turning
	int rand = random(0, 3);
	bool can_turn = false;

	//Generate random numbers to turn until we come across a valid random number.
	while (!can_turn) {
		if (rand == 0 && front_intersection_valid) {
			can_turn = true;
		}

		else if (rand == 1 && left_intersection_valid) {
			turn(LEFT);
			can_turn = true;
		}

		else if (rand == 2 && right_intersection_valid) {
			turn(RIGHT);
			can_turn = true;
		}

		else {
			rand = random(0, 3);
			can_turn = false;
		}
	}
}

bool Navigation::navigate_fixedpath_right(bool front_intersection_valid,
	bool left_intersection_valid,
	bool right_intersection_valid) {

	if (turn_number == 1) {
		turn(RIGHT);
		return validate(front_intersection_valid, left_intersection_valid, right_intersection_valid,
			true, false, true);
	}

	if (turn_number == 2) {
		turn(LEFT);
		return validate(front_intersection_valid, left_intersection_valid, right_intersection_valid,
			true, true, true);
	}

	if (turn_number == 3) {
		turn(LEFT);
		/*	validate(front_intersection_valid, left_intersection_valid, right_intersection_valid,
		false, true, true); TODO*/
		return true;
	}

	if (turn_number == 4) {
		turn(LEFT);
		/*validate(front_intersection_valid, left_intersection_valid, right_intersection_valid,
		false, true, false); TODO*/
		return true;
	}

	if (turn_number == 5) {
		turn(LEFT);
		return validate(front_intersection_valid, left_intersection_valid, right_intersection_valid,
			true, true, false);
	}

	if (turn_number == 6) {
		turn(RIGHT);
		return validate(front_intersection_valid, left_intersection_valid, right_intersection_valid,
			true, true, true);
	}

	if (turn_number == 7) {
		turn(RIGHT);
		return validate(front_intersection_valid, left_intersection_valid, right_intersection_valid,
			true, false, true);
	}

	if (turn_number == 8) {
		LCD.print("RIGHT PATH DONE");
		return false;
	}
}

bool Navigation::navigate_fixedpath_left(bool front_intersection_valid,
	bool left_intersection_valid,
	bool right_intersection_valid) {

	if (turn_number == 1) {
		turn(LEFT);
		return validate(front_intersection_valid, left_intersection_valid, right_intersection_valid,
			true, true, false);
	}

	else if (turn_number == 2) {
		turn(RIGHT);
		return validate(front_intersection_valid, left_intersection_valid, right_intersection_valid,
			true, true, true);
	}

	else if (turn_number == 3) {
		turn(RIGHT);
		return validate(front_intersection_valid, left_intersection_valid, right_intersection_valid,
			false, true, true);
	}

	else if (turn_number == 4) {
		turn(LEFT);
		return validate(front_intersection_valid, left_intersection_valid, right_intersection_valid,
			false, false, true);
	}

	else if (turn_number == 5) {
		turn(LEFT);
		LCD.print("LEFT PATH DONE");
		return false;
	}
}

bool Navigation::validate(bool front_intersection_valid,
	bool left_intersection_valid,
	bool right_intersection_valid,
	bool front_should_be_valid,
	bool left_should_be_valid,
	bool right_should_be_valid) {

	if (front_should_be_valid != front_intersection_valid) {
		return false;
	}

	if (left_should_be_valid != left_intersection_valid) {
		return false;
	}

	if (right_should_be_valid != right_intersection_valid) {
		return false;
	}

	return true;
}