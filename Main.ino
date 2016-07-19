#include "navigation.h"
#include <avr/interrupt.h>


//VARIABLE DECLARATIONS
volatile unsigned long detection_state = FOLLOWING_TAPE;
bool tape_is_lost = false;
unsigned long tape_lost_time;
bool passenger_picked_up = false;
Navigation navigation;

//PID VARIABLES
int integral = 0;
int left;
int right;
int p;
int q;
int kd;
int kp;
int d;
int error;
int lerr;
int recerr;
int m;
int con;
int c;
int ki;

ISR(INT0_vect) {
	if (detection_state == START) {
		return;
	}
	detection_state = COLLISION_DETECTED;
};

void enableExternalInterrupt(unsigned int INTX, unsigned int mode)
{
	if (INTX > 3 || mode > 3 || mode == 1) return;
	cli();
	/* Allow pin to trigger interrupts        */
	EIMSK |= (1 << INTX);
	/* Clear the interrupt configuration bits */
	EICRA &= ~(1 << (INTX * 2 + 0));
	EICRA &= ~(1 << (INTX * 2 + 1));
	/* Set new interrupt configuration bits   */
	EICRA |= mode << (INTX * 2);
	sei();
}

void disableExternalInterrupt(unsigned int INTX)
{
	if (INTX > 3) return;
	EIMSK &= ~(1 << INTX);
}

void setup() {
  disableExternalInterrupt(INT0);
  disableExternalInterrupt(INT1);
  disableExternalInterrupt(INT2);
  disableExternalInterrupt(INT3);

  #include <phys253setup.txt>
  LCD.clear();
  LCD.home();
  randomSeed(analogRead(0));

}

void loop() {

	if(detection_state == FOLLOWING_TAPE){
        follow_tape_normal();
	}

	if(detection_state == INTERSECTION_DETECTED){
        follow_tape_intersection();
	}
   
	if(detection_state == COLLISION_DETECTED){
		collision_detected();
	}

	if(detection_state == STOP){
        motor.speed(LEFT_MOTOR,0);
        motor.speed(RIGHT_MOTOR,0);
	}
}

/** State function for following tape.
**/
void follow_tape_normal(){
	tape_lost_time - millis(); //restart the timer

    while(detection_state == FOLLOWING_TAPE){

		if (!digitalRead(COLLISION_SWITCH)) {
			detection_state = COLLISION_DETECTED;
		}

        kp = knob(6) / 4;
        kd = knob(7) / 4;
        left = digitalRead(LEFT_TAPEFOLLOWER);
        right = digitalRead(RIGHT_TAPEFOLLOWER);
     
        if ((left)&&(right)) {
            error = 0;
        }
        
        if ((left)&&(!right)){
            error = -1;
        }
        
        if ((!left)&&(right)) {
            error = +1;
        }
        
        if ((!left)&&(!right)){
			
			if(!tape_is_lost){
				tape_lost_time = millis();
			}
			
            tape_is_lost = true;       
				
            if (lerr>0) {
                error = ERROR_GAIN;
            }
            
            if (lerr<=0) {
                error=-ERROR_GAIN;
            }
        }
		
		else{
			if (tape_is_lost) {
				if ((unsigned long)(millis() - tape_lost_time) > LOST_TAPE_TIMER) {
					//	navigation_mode = RANDOM_LOST TODO;
					//LCD.print("LOST");
					//detection_state = STOP; // TODO REMOVE
				}

				tape_is_lost = false;
			}
		}
        
        if (!(error==lerr)){
            recerr=lerr;
            q=m;
            m=1;
        }

        integral= ki * error + integral;
        p=kp*error;
        d=(int)((float)kd*(float)(error-recerr)/(float)(q+m));
        con = p+d + integral;

		//Limit the error gain. This is for when we turn, since we rely on this error correction. IF
		//the gain is too high, it turns at a ridiculous rate.
        if (con > MAX_GAIN){
            con = MAX_GAIN;
        }
		
		else if (con < -MAX_GAIN){
			con = -MAX_GAIN;
		}
		
        if (c==150){
            LCD.clear();
			LCD.home();
            LCD.print("P");
            LCD.print(kp);
            LCD.print("D");
            LCD.print(kd);
            LCD.print(" ");
            LCD.print(left);
            LCD.print(" ");
            LCD.print(right);
			LCD.setCursor(0, 1);
			LCD.print(digitalRead(FRONT_INTERSECTION1));
			LCD.print(digitalRead(FRONT_INTERSECTION2));
			LCD.print(digitalRead(FRONT_INTERSECTION3));
            c = 0;
        }
        
        c=c+1;
        m=m+1;
        motor.speed(LEFT_MOTOR,MOTOR_SPEED+con);
        motor.speed(RIGHT_MOTOR,MOTOR_SPEED-con);
        lerr=error;

        if(digitalRead(LEFT_INTERSECTION) ||
        digitalRead(RIGHT_INTERSECTION)){
			//We don't want to read multiple turns in quick succession.
			//This happens when a turn overshoots and the turn detectors see stuff again.
            if((unsigned long)(millis() - navigation.intersection_turn_timer) > INTERSECTION_WAIT_TIMER ){
                 detection_state = INTERSECTION_DETECTED;
            }
        }
    }
}

/**
State function for the intersection detected state
**/
void follow_tape_intersection(){
    unsigned long initial_timer = millis();
    bool left_intersection_valid = false;
    bool right_intersection_valid = false;
    bool front_intersection_valid = true; //TODO CHANGE TO FALSE WHEN THIS IS FIXED

    while((millis() - initial_timer) < INTERSECTION_TIMER){
        if(digitalRead(LEFT_INTERSECTION)){
            left_intersection_valid = true;
        }

        if(digitalRead(RIGHT_INTERSECTION)){
            right_intersection_valid = true;
        }

        kp= knob(6) / 4;
        kd = knob(7) / 4;
        left = digitalRead(LEFT_TAPEFOLLOWER);
        right = digitalRead(RIGHT_TAPEFOLLOWER);
     
        if ((left)&&(right)) {
            error = 0;
        }
        
        if ((left)&&(!right)){
            error = -1;
        }
        
        if ((!left)&&(right)) {
            error = +1;
        }
        
        if ((!left)&&(!right)){
          
            if (lerr>0) {
                error = ERROR_GAIN;
            }
            
            if (lerr<=0) {
                error=-ERROR_GAIN;
            }
        }
        
        if (!(error==lerr)){
            recerr=lerr;
            q=m;
            m=1;
        }
        
        p=kp*error;
        d=(int)((float)kd*(float)(error-recerr)/(float)(q+m));
        con = p+d;
            
        c=c+1;
        m=m+1;
        motor.speed(LEFT_MOTOR,MOTOR_SPEED+con);
        motor.speed(RIGHT_MOTOR,MOTOR_SPEED-con);
        lerr=error;
    }

	bool straight_possible = (digitalRead(FRONT_INTERSECTION1) || 
		digitalRead(FRONT_INTERSECTION2) ||
		digitalRead(FRONT_INTERSECTION3));

	if (straight_possible) {
		front_intersection_valid = true;
	}

	else {
		front_intersection_valid = false;
	}


    navigation.navigate(front_intersection_valid, left_intersection_valid, right_intersection_valid, detection_state);
}

/*
State function for collision detection. 
*/
void collision_detected() {
	LCD.clear();
	LCD.home();
	LCD.print("TURNING");

	collision_turn_around();

	if (!navigation.should_turn_around()) {
		navigation.navigation_mode = RANDOM_LOST;
		LCD.clear();
		LCD.home();
		LCD.print("TURN AROUND FALSE");
		if (!passenger_picked_up) {
			navigation.navigation_mode = RANDOM_LOST;
		}
	}

	else {
		LCD.home();
		LCD.clear();
		LCD.print("TURN AROUND TRUE");
	}


	//detection_state = STOP;
	detection_state = FOLLOWING_TAPE;
}

