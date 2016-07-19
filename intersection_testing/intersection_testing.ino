#include <phys253.h>
#include <LiquidCrystal.h>
#include "constants.h"

//CONSTANTS

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
      follow_tape_normal();
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
        
		if (c == 150) {
			LCD.clear();
			LCD.print("P");
			LCD.print(kp);
			LCD.print("D");
			LCD.print(kd);
			LCD.print(" ");
			LCD.print(left);
			LCD.print(" ");
			LCD.print(right);
			c = 0;
		}
        
        c=c+1;
        m=m+1;
        motor.speed(0,MOTOR_SPEED+con);
        motor.speed(1,MOTOR_SPEED-con);
        lerr=error;
		
		bool front_intersection_invalid = !(digitalRead(FRONT_INTERSECTION1) || digitalRead(FRONT_INTERSECTION2));

        if(digitalRead(LEFT_INTERSECTION) ||
        digitalRead(RIGHT_INTERSECTION))
            //We don't want to read multiple turns in quick succession.
            //This happens when a turn overshoots and the turn detectors see stuff again.
            if((millis() - initial_turn_timer) >INTERSECTION_WAIT_TIMER){
                 detection_state = INTERSECTION_DETECTED;
            }

            else{
                LCD.print("FALSE TURN");
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
    bool front_intersection_valid = false; 

    while((millis() - initial_timer) < INTERSECTION_TIMER){

		bool front_intersection_invalid = !(digitalRead(FRONT_INTERSECTION1) ||
			digitalRead(FRONT_INTERSECTION3));

		if (front_intersection_invalid) {
			front_intersection_valid = false;
		}

		else {
			front_intersection_valid = true;
		}

        if(digitalRead(LEFT_INTERSECTION)){
            left_intersection_valid = true;
        }

        if(digitalRead(RIGHT_INTERSECTION)){
            right_intersection_valid = true;
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

	LCD.setCursor(0, 1);

    if(front_intersection_valid){
        LCD.write("FRONT");
    }

    if(left_intersection_valid){
        LCD.write("LEFT");
    }

    if(right_intersection_valid){
        LCD.write("RIGHT");
    }

	LCD.print(digitalRead(FRONT_INTERSECTION1));
	LCD.print(digitalRead(FRONT_INTERSECTION3));
	LCD.print(digitalRead(FRONT_INTERSECTION2));

    turn_number++;
    detection_state = STOP;
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
 //	LCD.clear();
	//
	//if(left_intersection_valid){
	//	turn_modified(LEFT);
	//}
	//
	//else if(right_intersection_valid){
	//	turn_modified(RIGHT);
	//}

 //  detection_state = FOLLOWING_TAPE;

 //  turn_number++;
 //  LCD.print("TURNED");
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
