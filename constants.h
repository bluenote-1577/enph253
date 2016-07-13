// constants.h

#ifndef _CONSTANTS_h
#define _CONSTANTS_h

#if defined(ARDUINO) && ARDUINO >= 100
#else
	#include "WProgram.h"
#endif
//CONSTANTS
const unsigned long MOTOR_SPEED = 150;

//TUNING PARAMETERS
const int ERROR_GAIN = 3;
const int MAX_GAIN = 150;
const unsigned long INTERSECTION_TIMER = 75;
const unsigned long TURN_TIMER = 300;
const unsigned long LOST_TAPE_TIMER = 700;
const unsigned long COLLISION_TURN_TIMER = 950;
const unsigned long INTERSECTION_WAIT_TIMER = 1000;

//SIGNAL PINS
const unsigned long LEFT_MOTOR = 0;
const unsigned long RIGHT_MOTOR = 1;
const unsigned long LEFT_TAPEFOLLOWER = 3;
const unsigned long RIGHT_TAPEFOLLOWER = 4;
const unsigned long FRONT_INTERSECTION = 0;
const unsigned long LEFT_INTERSECTION = 1;
const unsigned long RIGHT_INTERSECTION = 2;

//DETECTION STATES
const unsigned long FOLLOWING_TAPE = 1;
const unsigned long INTERSECTION_DETECTED = 2;
const unsigned long PASSENGER_DETECTED_LEFT = 3;
const unsigned long PASSENGER_DETECTED_RIGHT = 4;
const unsigned long DROPOFF_DETECTED_LEFT = 5;
const unsigned long DROPOFF_DETECTED_RIGHT = 6;
const unsigned long COLLISION_DETECTED = 7;

//TURNS
const unsigned long FORWARD = 0;
const unsigned long LEFT = 1;
const unsigned long RIGHT = 2;
const int TURN_SPEED = 120;

//NAVIGATION MODES
const unsigned long START = 8;
const unsigned long FIXEDPATH_LEFT = 9;
const unsigned long FIXEDPATH_RIGHT = 10;
const unsigned long FIXEDPATH_AFTERDROPOFF_LEFT = 11;
const unsigned long FIXEDPATH_AFTERDROPOFF_RIGHT = 12;
const unsigned long RANDOM_TODROPOFF = 13;
const unsigned long RANDOM_LOST = 14;

//STOP STATE
const unsigned long STOP = 69;

#endif

