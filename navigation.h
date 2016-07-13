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

	unsigned long navigation_mode = FIXEDPATH_RIGHT;
	unsigned long intersection_turn_timer;

	void navigate(bool front_intersection_valid,
		bool left_intersection_valid,
		bool right_intersection_valid,
		volatile unsigned long &detection_state);

	void navigate_random(bool front_intersection_valid,
		bool left_intersection_valid,
		bool right_intersection_valid);

	bool navigate_fixedpath_right(bool front_intersection_valid,
		bool left_intersection_valid,
		bool right_intersection_valid);

	bool navigate_fixedpath_left(bool front_intersection_valid,
		bool left_intersection_valid,
		bool right_intersection_valid);

	bool validate(bool front_intersection_valid,
		bool left_intersection_valid,
		bool right_intersection_valid,
		bool front_should_be_valid,
		bool left_should_be_valid,
		bool right_should_be_valid);

private:
	unsigned long turn_number = 0;

};

#endif

