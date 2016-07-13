#include <phys253.h>
#include <LiquidCrystal.h>

//CONSTANTS
const unsigned long MOTOR_SPEED = 160;

const unsigned long FORWARD = 0;
const unsigned long LEFT = 1;
const unsigned long RIGHT = 2;

const int ERROR_GAIN = 3;
const int MAX_GAIN = 170;
const unsigned long STOP = 69;
const int TURN_SPEED = 120;
unsigned long INTERSECTION_TIMER = 7500 / MOTOR_SPEED;
unsigned long TURN_TIMER = 55000 / MOTOR_SPEED;

const unsigned long LEFT_TAPEFOLLOWER = 3;
const unsigned long RIGHT_TAPEFOLLOWER = 4;
const unsigned long FRONT_INTERSECTION = 0;
const unsigned long LEFT_INTERSECTION = 1;
const unsigned long RIGHT_INTERSECTION = 2;

const unsigned long FOLLOWING_TAPE = 1;
const unsigned long INTERSECTION_DETECTED = 2;
const unsigned long PASSENGER_DETECTED_LEFT = 3;
const unsigned long PASSENGER_DETECTED_RIGHT = 4;
const unsigned long DROPOFF_DETECTED_LEFT = 5;
const unsigned long DROPOFF_DETECTED_RIGHT = 6;
const unsigned long COLLISION_DETECTED = 7;

const unsigned long START = 8;
const unsigned long FIXEDPATH_LEFT = 9;
const unsigned long FIXEDPATH_RIGHT = 10;
const unsigned long FIXEDPATH_AFTERDROPOFF_LEFT = 11;
const unsigned long FIXEDPATH_AFTERDROPOFF_RIGHT = 12;
const unsigned long RANDOM_TODROPOFF = 13;
const unsigned long RANDOM_LOST = 14;

//VARIABLE DECLARATIONS
volatile unsigned long detection_state = FOLLOWING_TAPE;
unsigned long navigation_mode = FIXEDPATH_LEFT;
unsigned long turn_number = 0;
bool tape_is_lost = false;
int initial_turn_timer;

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

void setup() {
  #include <phys253setup.txt>
  LCD.clear();
  LCD.home();
  randomSeed(analogRead(0));
}

void loop() {

   if(detection_state == FOLLOWING_TAPE){
	   if (turn_number == 2) {
		   detection_state = STOP;
	   }

	   else {
		   follow_tape_normal();
	   }
   }

   if(detection_state == INTERSECTION_DETECTED){
        follow_tape_intersection();
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
        INTERSECTION_TIMER = knob(6)/4;
        TURN_TIMER = knob(7);
        kp = 100;
        kd = 50;
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
            if((millis() - initial_turn_timer) > 2750){
                 detection_state = INTERSECTION_DETECTED;
            }

            else{
                LCD.print("FALSE TURN");
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

        if(digitalRead(FRONT_INTERSECTION)){
          //  front_intersection_valid = true; not working yet
        }

        kp= 50;
        kd= 30;
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
        motor.speed(0,MOTOR_SPEED+con);
        motor.speed(1,MOTOR_SPEED-con);
        lerr=error;
    }

// UNCOMMENT THIS AND COMMENT THE BELOW CODE TO TEST INTERSECTION DETECTION.
///////////////////////////////////////////////
//    LCD.clear();
//    
//    if(front_intersection_valid){
//        LCD.write("FRONT");
//    }
//
//    if(left_intersection_valid){
//        LCD.write("LEFT");
//    }
//
//    if(right_intersection_valid){
//        LCD.write("RIGHT");
//    }
//
//    turn_number++;
//    detection_state = STOP;
//////////////////////////////////////////////	

//Just for turning and stuff
///////////////////////////////////////////////////
//    if(left_intersection_valid){
//        turn(LEFT);
//    }
//
//    else if(right_intersection_valid){
//        turn(RIGHT);
//    }
//    detection_state = FOLLOWING_TAPE;
///////////////////////////////////////////////////////

//UNCOMMENT THIS FOR TESTING TURNING.
 	LCD.clear();
	
	if(left_intersection_valid){
		turn_modified(LEFT);
	}
	
	else if(right_intersection_valid){
		turn_modified(RIGHT);
	}

   detection_state = FOLLOWING_TAPE;

   turn_number++;
   LCD.print("TURNED");
///////////////////// 
}

/**
Function for turning. Called in follow_tape_intersection(). 
Goes back into tape folloing after this function is called.

Can only take 3 parameters, LEFT, RIGHT, or FORWARD. These parameters are defined
as integers at the top.
**/
void turn(int DIRECTION){
    initial_turn_timer = millis();
    bool has_turned = false;
    switch(DIRECTION){
        //See case LEFT: for turning algorithm
        case RIGHT:
        {
            motor.speed(0,TURN_SPEED);
            motor.speed(1,-TURN_SPEED);
        
            delay(20000/TURN_SPEED);                
            while((millis() - initial_turn_timer) < TURN_TIMER){
                bool is_on_tape = digitalRead(LEFT_TAPEFOLLOWER) || digitalRead(RIGHT_TAPEFOLLOWER);

                if(!is_on_tape && has_turned == false){
                    has_turned == true;
                    lerr =1; //this makes the robot turn right more
                }

                if(is_on_tape && has_turned == true){
                    LCD.clear();
                    LCD.print("ON TAPE AND TURNED");
                
                }
            }

            break;
        }
        
        case LEFT:
        {
            //set the left motor to go back.
            motor.speed(0,-TURN_SPEED);
            motor.speed(1,TURN_SPEED);  
            
            //Delay a certain amount of time so that we are on white tape when we start again.
            delay(20000/TURN_SPEED);
                
            /**
            Turn for a certain amount of time.
            Read the tape followers. While they're on white, we know that we have to turn more.
            If we read white, then read black again, that means we've turned too much.
            **/
            while((millis() - initial_turn_timer) < TURN_TIMER){
                bool is_on_tape = digitalRead(LEFT_TAPEFOLLOWER) || digitalRead(RIGHT_TAPEFOLLOWER);

                if(!is_on_tape && has_turned == false){
                    has_turned == true;
                    lerr = -1; //this makes the robot turn left more
                }

                if(is_on_tape && has_turned == true){
                    LCD.clear();
                    LCD.print("ON TAPE AND TURNED");                   
                }
            }

            break;
        }
        
        case FORWARD:
        {
            break;
        }

    }
}

void turn_modified(int DIRECTION){
	
 if(DIRECTION == FORWARD){
        return;
    }
    
    initial_turn_timer = millis();
    bool has_turned = false;

    motor.speed(0, TURN_SPEED);
    motor.speed(1, TURN_SPEED);         
   // delay(50);
    
    if(DIRECTION == RIGHT){
        motor.speed(0,TURN_SPEED);
        motor.speed(1,-TURN_SPEED);    
    }

    else{
        motor.speed(0,-TURN_SPEED);
        motor.speed(1,TURN_SPEED);    
    } 
    
    bool is_on_tape = true;
    while(is_on_tape){
        is_on_tape = digitalRead(LEFT_TAPEFOLLOWER) && digitalRead(RIGHT_TAPEFOLLOWER);

        if(!is_on_tape && has_turned == false){
            has_turned == true;
        }
    }

    delay(TURN_TIMER);
    
    while(!is_on_tape){
        is_on_tape = digitalRead(LEFT_TAPEFOLLOWER) && digitalRead(RIGHT_TAPEFOLLOWER);
    }
}
