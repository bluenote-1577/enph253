// movement.h

#ifndef _MOVEMENT_h
#define _MOVEMENT_h

#include <phys253.h>
#include <LiquidCrystal.h>
#include "constants.h"

#if defined(ARDUINO) && ARDUINO >= 100

#else
	#include "WProgram.h"
#endif

	void turn(int DIRECTION);
	void collision_turn_around();

#endif

