#include <phys253.h>
#include <LiquidCrystal.h>
#include <avr/interrupt.h>
#include "constants.h"

//VARIABLE DECLARATIONS
volatile unsigned int detection_state = FOLLOWING_TAPE;
unsigned int navigation_mode = FIXEDPATH_LEFT;
unsigned int turn_number = 0;
bool tape_is_lost = false;
int initial_turn_timer;
bool passenger_picked_up = false;

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
int int0;

int collision_testing_timer;

ISR(INT0_vect) {
	detection_state = COLLISION_DETECTED;
	int0++;
	LCD.clear();
	LCD.print(int0);
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
	enableExternalInterrupt(INT0, FALLING);
  #include <phys253setup.txt>
  LCD.clear();
  LCD.home();
  randomSeed(analogRead(0));
}

void loop() {    
   if(detection_state == FOLLOWING_TAPE){
        follow_tape_normal();
   }

   
   if(detection_state == COLLISION_DETECTED){
	   collision_turn_around();
	   detection_state = FOLLOWING_TAPE;
   }

   if(detection_state == STOP){
        motor.speed(0,0);
        motor.speed(2,0);
   }
}

/** State function for following tape.
**/
void follow_tape_normal(){
    while(detection_state == FOLLOWING_TAPE){
        
        kp = 50;
        kd = 30;
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
    
            tape_is_lost = true;        
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
            LCD.print(COLLISION_TURN_TIMER);
            LCD.print("TURN ");
            c = 0;
        }
        
        c=c+1;
        m=m+1;
        motor.speed(0,MOTOR_SPEED+con);
        motor.speed(2,MOTOR_SPEED-con);
        lerr=error;
    }
}


void collision_turn_around() {
	motor.speed(0, -TURN_SPEED / 3);
	motor.speed(2, -TURN_SPEED);
	delay(1100);

	motor.speed(0, TURN_SPEED);
	motor.speed(2, -TURN_SPEED);

	bool is_on_tape = true;

	while (is_on_tape) {
		is_on_tape = (digitalRead(LEFT_TAPEFOLLOWER) || digitalRead(RIGHT_TAPEFOLLOWER));
	}

	while (!is_on_tape) {
		is_on_tape = (digitalRead(LEFT_TAPEFOLLOWER) && digitalRead(RIGHT_TAPEFOLLOWER));
	}
}
