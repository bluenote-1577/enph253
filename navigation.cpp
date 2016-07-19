// 
// 
// 

#include "navigation.h"

void Navigation::navigate(bool front_intersection_valid,
	bool left_intersection_valid,
	bool right_intersection_valid,
	volatile unsigned long &detection_state) {

	LCD.clear();

	this->front_intersection_valid = front_intersection_valid;
	this->left_intersection_valid = left_intersection_valid;
	this->right_intersection_valid = right_intersection_valid;

	bool correct_turn;
	turn_number++;

	if (navigation_mode == START) {
		if (!right_intersection_valid) {
			navigation_mode = FIXEDPATH_LEFT;
		}

		else if (!left_intersection_valid) {
			navigation_mode = FIXEDPATH_RIGHT;
		}

		else {
			detection_state = STOP;
			LCD.clear();
			LCD.print("START ERROR");
		}
	}

	if (navigation_mode == RANDOM_LOST) {
		navigate_random();
	}

	if (navigation_mode == FIXEDPATH_RIGHT) {
		correct_turn = navigate_fixedpath_right();
	}

	if (navigation_mode == FIXEDPATH_LEFT) {
		correct_turn = navigate_fixedpath_left();
	}

	if (navigation_mode == GUIDED_DROPOFF) {
		correct_turn = true;
		navigate_guided_dropoff();
	}

	if (!correct_turn) {
		LCD.clear();
		LCD.home();
		LCD.print("INVALID ");
		LCD.print(turn_number);
		detection_state = STOP;

		if (front_intersection_valid) {
			LCD.print("f");
		}

		if (left_intersection_valid) {
			LCD.print("l");
		}

		if (right_intersection_valid) {
			LCD.print("r");
		}
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

void Navigation::navigate_random(){

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

bool Navigation::navigate_fixedpath_right(){

	if (turn_number == 1) {
		turn(RIGHT);
		return validate(true, false, true);
	}

	else if (turn_number == 2) {
		turn(LEFT);
		return validate(true, true, true);
	}

	else if (turn_number == 3) {
		turn(LEFT);
		return validate(false, true, true);
	}

	else if (turn_number == 4) {
		turn(LEFT);
		return validate(false, true, false);
	}

	else if (turn_number == 5) {
		turn(LEFT);
		return validate(true, true, false);
	}

	else if (turn_number == 6) {
		turn(RIGHT);
		return validate(true, true, true);
	}

	else if (turn_number == 7) {
		turn(RIGHT);
		return validate(true, false, true);
	}

	else if (turn_number == 8) {
		turn(RIGHT);
		return validate(false, true, true);
	}

	else if (turn_number == 9) {
		turn(FORWARD);
		return validate(true, true, false);
	}

	else if (turn_number == 10) {
		turn(RIGHT);
		return validate(true, true, true);
	}

	else if (turn_number == 11) {
		turn(LEFT);
		return validate(false, true, true);
	}

	else if (turn_number == 12) {
		turn(FORWARD);
		return validate(true, true, false);
	}

	else if (turn_number == 13) {
		turn(LEFT);
		return validate(true, true, false);
	}

	else if (turn_number == 14) {
		turn(LEFT);
		return validate(false, true, true);
	}

	else if (turn_number == 15) {
		turn(FORWARD);
		return validate(true, true, false);
	}

	else if (turn_number == 16) {
		turn(LEFT);
		return validate(true, true, false);
	}

	else if (turn_number == 17) {
		turn(RIGHT);
		return validate(true, true, true);
	}

	else if (turn_number == 18) {
		turn(LEFT);
		return validate(true, true, false);
	}

	else if (turn_number == 19) {
		turn(LEFT);
		return validate(false, true, true);
	}

	else if (turn_number == 20) {
		turn(FORWARD);
		return validate(true, false, true);
	}

	else if (turn_number == 21) {
		turn(FORWARD);
		return validate(true, true, true);
	}

	else if (turn_number == 22) {
		turn(RIGHT);
		return validate(false, true, true);
	}

	else if (turn_number == 23) {
		turn(RIGHT);
		return validate(false, false, true);
	}

	else if (turn_number == 24) {
		turn(RIGHT);
		return validate(true, false, true);
	}

	else if (turn_number == 25) {
		turn(RIGHT);
		return validate(true, true, true);
	}

	else if (turn_number == 26) {
		turn(LEFT);
		return validate(false, true, true);
	}

	else if (turn_number == 27) {
		turn(FORWARD);
		return validate(true, true, false);
	}

	else if (turn_number == 28) {
		turn(LEFT);
		return validate(false, true, false);
	}

	else if (turn_number == 29) {
		turn(LEFT);
		return validate(true, true, false);
		turn_number = 1;
	}

	else return false;
}

bool Navigation::navigate_fixedpath_left() {

	if (turn_number == 1) {
		turn(LEFT);
		return validate(true, true, false);
	}

	else if (turn_number == 2) {
		turn(RIGHT);
		return validate(true, true, true);
	}

	else if (turn_number == 3) {
		turn(RIGHT);
		return validate(false, true, true); 
	}

	else if (turn_number == 4) {
		turn(RIGHT);
		return validate(false, false, true); 
	}

	else if (turn_number == 5) {
		turn(RIGHT);
		return validate(true, false, true);
	}

	else if (turn_number == 6) {
		turn(LEFT);
		return validate(true, true, true);
	}

	else if (turn_number == 7) {
		turn(LEFT);
		return validate(true, true, false);
	}

	else if (turn_number == 8) {
		turn(LEFT);
		return validate(false, true, true); 
	}

	else if (turn_number == 9) {
		turn(FORWARD);
		return validate(true, false, true);
	}

	else if (turn_number == 10) {
		turn(LEFT);
		return validate(true, true, true);
	}

	else if (turn_number == 11) {
		turn(RIGHT);
		return validate(false, true, true);
	}

	else if (turn_number == 12) {
		turn(FORWARD);
		return validate(true, false, true);
	}

	else if (turn_number == 13) {
		turn(RIGHT);
		return validate(true, false, true);
	}

	else if (turn_number == 14) {
		turn(RIGHT);
		return validate(false, true, true);
	}

	else if (turn_number == 15) {
		turn(FORWARD);
		return validate(true, false, true);
	}

	else if (turn_number == 16) {
		turn(RIGHT);
		return validate(true, false, true);
	}

	else if (turn_number == 17) {
		turn(LEFT);
		return validate(true, true, true);
	}

	else if (turn_number == 18) {
		turn(RIGHT);
		return validate(true, false, true);
	}

	else if (turn_number == 19) {
		turn(RIGHT);
		return validate(false, true, true);
	}

	else if (turn_number == 20) {
		turn(FORWARD);
		return validate(true, true, false);
	}

	else if (turn_number == 21) {
		turn(FORWARD);
		return validate(true, true, true);
	}

	else if (turn_number == 22) {
		turn(LEFT);
		return validate(false, true, true);
	}

	else if (turn_number == 23) {
		turn(LEFT);
		return validate(false, true, false);
	}

	else if (turn_number == 24) {
		turn(LEFT);
		return validate(true, true, false);
	}

	else if (turn_number == 25) {
		turn(LEFT);
		return validate(true, true, true);
	}

	else if (turn_number == 26) {
		turn(RIGHT);
		return validate(false, true, true);
	}

	else if (turn_number == 27) {
		turn(FORWARD);
		return validate(true, false, true);
	}

	else if (turn_number == 28) {
		turn(RIGHT);
		return validate(false, false, true);
	}

	else if (turn_number == 29) {
		turn(RIGHT);
		return validate(true, false, true);
		turn_number = 1;
	}

	else return false;
}

void Navigation::navigate_guided_dropoff() {
	int left_ir = analogRead(LEFT_DROPOFF_IR);
	int right_ir = analogRead(RIGHT_DROPOFF_IR);
	int forward_ir = analogRead(FORWARD_DROPOFF_IR);

	unsigned long max_direction;

	if (left_ir > right_ir) {
		max_direction = LEFT;
	}

	else {
		max_direction = RIGHT;
	}

	if (max_direction == RIGHT) {
		if (forward_ir > right_ir) {
			max_direction = FORWARD;
		}
	}

	else {
		if (forward_ir > left_ir) {
			max_direction = FORWARD;
		}
	}

	turn(max_direction);
}

bool Navigation::validate(
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

bool Navigation::should_turn_around() {

	bool should_collide = false;

	if (navigation_mode == FIXEDPATH_LEFT||
		navigation_mode == FIXEDPATH_RIGHT) {

		if (turn_number == 7 ||
			turn_number == 8 ||
			turn_number == 13 ||
			turn_number == 18 ||
			turn_number == 19) {
			should_collide = true;
		}

	}

	return should_collide;
}