#include <phys253.h>
#include <LiquidCrystal.h>

//CONSTANTS
const unsigned int MOTOR_SPEED = 130;

const unsigned int FORWARD = 0;
const unsigned int LEFT = 1;
const unsigned int RIGHT = 2;

const int ERROR_GAIN = 3;
const int MAX_GAIN = 150;
const unsigned int STOP = 69;
const int TURN_SPEED = 100;
const unsigned int INTERSECTION_TIMER = 6500 / TURN_SPEED;
const unsigned int TURN_TIMER = 45000 / TURN_SPEED;
const unsigned int LOST_TAPE_TIMER = 300;
const unsigned int COLLISION_TURN_TIMER = 500;

const unsigned int LEFT_MOTOR = 0;
const unsigned int RIGHT_MOTOR = 1;
const unsigned int LEFT_TAPEFOLLOWER = 3;
const unsigned int RIGHT_TAPEFOLLOWER = 4;
const unsigned int FRONT_INTERSECTION = 0;
const unsigned int LEFT_INTERSECTION = 1;
const unsigned int RIGHT_INTERSECTION = 2;

const unsigned int FOLLOWING_TAPE = 1;
const unsigned int INTERSECTION_DETECTED = 2;
const unsigned int PASSENGER_DETECTED_LEFT = 3;
const unsigned int PASSENGER_DETECTED_RIGHT = 4;
const unsigned int DROPOFF_DETECTED_LEFT = 5;
const unsigned int DROPOFF_DETECTED_RIGHT = 6;
const unsigned int COLLISION_DETECTED = 7;

const unsigned int START = 8;
const unsigned int FIXEDPATH_LEFT = 9;
const unsigned int FIXEDPATH_RIGHT = 10;
const unsigned int FIXEDPATH_AFTERDROPOFF_LEFT = 11;
const unsigned int FIXEDPATH_AFTERDROPOFF_RIGHT = 12;
const unsigned int RANDOM_TODROPOFF = 13;
const unsigned int RANDOM_LOST = 14;

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

int collision_testing_timer;

void setup() {
  #include <phys253setup.txt>
  LCD.clear();
  LCD.home();
  randomSeed(analogRead(0));
  
  collision_testing_timer = 1000;
}

void loop() {
   COLLISION)TURN_TIMER = knob(6)/4;
   
   if(collision_testing_timer < millis()){
	   detection_state == COLLISION_DETECTED;
   }
    
   if(detection_state == FOLLOWING_TAPE){
        follow_tape_normal();
   }

   
   if(detection_state == COLLISION_DETECTED){
	   collision_turn_around();
   }

   if(detection_state == STOP){
        motor.speed(0,0);
        motor.speed(1,0);
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
            LCD.print("INT ");
            LCD.print(INTERSECTION_TIMER);
            LCD.print("TURN ");
            LCD.print(TURN_TIMER);
            c = 0;
        }
        
        c=c+1;
        m=m+1;
        motor.speed(0,MOTOR_SPEED+con);
        motor.speed(1,MOTOR_SPEED-con);
        lerr=error;

        if((digitalRead(LEFT_INTERSECTION) ||
        digitalRead(RIGHT_INTERSECTION))){
            //We don't want to read multiple turns in quick succession.
            //This happens when a turn overshoots and the turn detectors see stuff again.
            if((millis() - initial_turn_timer) > 2000){
                 detection_state = INTERSECTION_DETECTED;
            }

            else{
                LCD.print("FALSE TURN");
            }
        }
    }
}


void collision_turn_around(){
	motor.speed(LEFT_MOTOR,0);
	motor.speed(RIGHT_MOTOR,0);
	
	int initial_timer = millis();
	bool is_on_tape;
	
	while((millis() - initial_timer) < COLLISION_TURN_TIMER)){
		motor.speed(LEFT_MOTOR, -TURN_SPEED);
		motor.speed(RIGHT_MOTOR, -TURN_SPEED/2);
	}
	
	while(!is_on_tape){
		is_on_tape = digitalRead(LEFT_TAPEFOLLOWER) || digitalRead(RIGHT_TAPEFOLLOWER);
		
		motor.speed(LEFT_MOTOR, TURN_SPEED/2);
		motor.speed(RIGHT_MOTOR, TURN_SPEED);
	}
	
	if(!passenger_picked_up){
		navigation_mode = RANDOM_LOST;
	}
	
	detection_state = STOP;
}
