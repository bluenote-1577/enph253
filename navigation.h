// navigation.h

#ifndef _NAVIGATION_h
#define _NAVIGATION_h
#include "movement.h"


#if defined(ARDUINO) && ARDUINO >= 100
#else
	#include "WProgram.h"
#endif

class Navigation {
public:

	unsigned long navigation_mode = START;
	unsigned long intersection_turn_timer;

	void navigate(bool front_intersection_valid,
		bool left_intersection_valid,
		bool right_intersection_valid,
		volatile unsigned long &detection_state);

	bool should_turn_around();

	bool front_intersection_valid;
	bool left_intersection_valid;
	bool right_intersection_valid;

private:
	unsigned long turn_number = 0;

	void navigate_random();

	bool navigate_fixedpath_right();

	bool navigate_fixedpath_left();

	void navigate_guided_dropoff();

	bool validate(
		bool front_should_be_valid,
		bool left_should_be_valid,
		bool right_should_be_valid);
};

#endif

