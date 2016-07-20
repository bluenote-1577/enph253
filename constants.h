// constants.h

#ifndef _CONSTANTS_h
#define _CONSTANTS_h

#if defined(ARDUINO) && ARDUINO >= 100
#else
	#include "WProgram.h"
#endif

#define ARM_ROTATION_SERVO RCServo1
#define CLAW_SERVO RCServo0

//CONSTANTS
const unsigned long MOTOR_SPEED = 165;

//TUNING PARAMETERS
const unsigned long PASSENGER_DETECTED_ROLLBACK_TIMER = 200;
const int ERROR_GAIN = 8;
const int MAX_GAIN = 150;
const unsigned long INTERSECTION_TIMER = 150;
const unsigned long TURN_TIMER = 300;
const unsigned long LOST_TAPE_TIMER = 800;
const unsigned long COLLISION_TURN_TIMER = 1300;
const unsigned long INTERSECTION_WAIT_TIMER = 1000;
const unsigned long PASSENGER_IR_THRESHOLD = 800;

//PASSENGER TUNING
const unsigned long SLOW_THRESHOLD = 400;
const unsigned long	STOP_THRESHOLD = 800;
const unsigned long FINAL_THRESHOLD = 600;
const unsigned long SLOW_ROLLING_SPEED = 60;

//SIGNAL PINS
const unsigned long ARM_STOP_SWITCH = 8;
const unsigned long COLLISION_SWITCH = 0;
const unsigned long LEFT_MOTOR = 0;
const unsigned long RIGHT_MOTOR = 1;
const unsigned long ARM_MOTOR = 2;
const unsigned long LEFT_TAPEFOLLOWER = 1;
const unsigned long RIGHT_TAPEFOLLOWER = 2;
const unsigned long LEFT_INTERSECTION = 3;
const unsigned long RIGHT_INTERSECTION = 4;
const unsigned long FRONT_INTERSECTION1 = 5;
const unsigned long FRONT_INTERSECTION2 = 6;
const unsigned long FRONT_INTERSECTION3 = 7;

//IR PINS
const unsigned long LEFT_PASSENGER_IR = 0;
const unsigned long RIGHT_PASSENGER_IR = 2;
const unsigned long LEFT_DROPOFF_IR = 1;
const unsigned long RIGHT_DROPOFF_IR = 3;
const unsigned long FORWARD_DROPOFF_IR = 4;

//DETECTION STATES
const unsigned long FOLLOWING_TAPE = 1;
const unsigned long INTERSECTION_DETECTED = 2;
const unsigned long PASSENGER_DETECTED = 3;
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
const unsigned long GUIDED_DROPOFF = 13;
const unsigned long RANDOM_LOST = 14;

//STOP STATE
const unsigned long STOP = 69;

#endif

