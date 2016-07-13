7#include <phys253.h>
#include <LiquidCrystal.h>

//CONSTANTS
const unsigned int MOTOR_SPEED = 130;

const unsigned int FORWARD = 0;
const unsigned int LEFT = 1;
const unsigned int RIGHT = 2;

//TUNING PARAMETERS
const int ERROR_GAIN = 3;
const int MAX_GAIN = 150;
const unsigned int STOP = 69;
const int TURN_SPEED = 100;
const unsigned int INTERSECTION_TIMER = 6500 / TURN_SPEED;
const unsigned int TURN_TIMER = 45000 / TURN_SPEED;
const unsigned int LOST_TAPE_TIMER = 300;
const unsigned int COLLISION_TURN_TIMER = 500;

//SIGNAL PINS
const unsigned int LEFT_MOTOR = 0;
const unsigned int RIGHT_MOTOR = 1;
const unsigned int LEFT_TAPEFOLLOWER = 3;
const unsigned int RIGHT_TAPEFOLLOWER = 4;
const unsigned int FRONT_INTERSECTION = 0;
const unsigned int LEFT_INTERSECTION = 1;
const unsigned int RIGHT_INTERSECTION = 2;

//DETECTION STATES
const unsigned int FOLLOWING_TAPE = 1;
const unsigned int INTERSECTION_DETECTED = 2;
const unsigned int PASSENGER_DETECTED_LEFT = 3;
const unsigned int PASSENGER_DETECTED_RIGHT = 4;
const unsigned int DROPOFF_DETECTED_LEFT = 5;
const unsigned int DROPOFF_DETECTED_RIGHT = 6;
const unsigned int COLLISION_DETECTED = 7;

//NAVIGATION MODES
const unsigned int START = 8;
const unsigned int FIXEDPATH_LEFT = 9;
const unsigned int FIXEDPATH_RIGHT = 10;
const unsigned int FIXEDPATH_AFTERDROPOFF_LEFT = 11;
const unsigned int FIXEDPATH_AFTERDROPOFF_RIGHT = 12;
const unsigned int RANDOM_TODROPOFF = 13;
const unsigned int RANDOM_LOST = 14;

//VARIABLE DECLARATIONS
volatile unsigned int detection_state = FOLLOWING_TAPE;
unsigned int navigation_mode = FIXEDPATH_RIGHT;
unsigned int turn_number = 0;
bool tape_is_lost = false;
int initial_turn_timer;
int tape_lost_time;
bool passenger_picked_up = false;

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

void setup() {
  #include <phys253setup.txt>
  LCD.clear();
  LCD.home();
  randomSeed(420);
}

void loop() {
   if(detection_state == FOLLOWING_TAPE){
        follow_tape_normal();
   }

   if(detection_state == INTERSECTION_DETECTED){
        follow_tape_intersection();
   }
   
   if(detection_state == COLLISION_DETECTED){
       collision_turn_around();
   }

   if(detection_state == STOP){
        motor.speed(LEFT_MOTOR,0);
        motor.speed(RIGHT_MOTOR,0);
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
            if(tape_is_lost){
                if((millis() - tape_lost_time) > LOST_TAPE_TIMER){
                    navigation_mode = RANDOM_LOST;
                }
            }
            
            tape_is_lost = false;
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
        motor.speed(LEFT_MOTOR,MOTOR_SPEED+con);
        motor.speed(RIGHT_MOTOR,MOTOR_SPEED-con);
        lerr=error;

        if((digitalRead(LEFT_INTERSECTION) ||
        digitalRead(RIGHT_INTERSECTION))){
            //We don't want to read multiple turns in quick succession.
            //This happens when a turn overshoots and the turn detectors see stuff again.
            if((millis() - initial_turn_timer) > 2000){
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

        if(digitalRead(FRONT_INTERSECTION)){
          //  front_intersection_valid = true; not working yet TODO
        }

        kp=knob(6) / 4;
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
        
        p=kp*error;
        d=(int)((float)kd*(float)(error-recerr)/(float)(q+m));
        con = p+d;
            
        c=c+1;
        m=m+1;
        motor.speed(LEFT_MOTOR,MOTOR_SPEED+con);
        motor.speed(RIGHT_MOTOR,MOTOR_SPEED-con);
        lerr=error;
    }

    turn_number++;

    if(tape_is_lost){
        navigate_random(front_intersection_valid, left_intersection_valid, right_intersection_valid);
    }
    navigate(front_intersection_valid, left_intersection_valid, right_intersection_valid);
    detection_state = FOLLOWING_TAPE;
}

/**
Function for turning. Called in follow_tape_intersection(). 
Goes back into tape folloing after this function is called.

Can only take 3 parameters, LEFT, RIGHT, or FORWARD. These parameters are defined
as integers at the top.
**/
void turn(int DIRECTION){
if(DIRECTION == FORWARD){
        return;
    }
    
    initial_turn_timer = millis();
    bool has_turned = false;

    motor.speed(0, TURN_SPEED);
    motor.speed(1, TURN_SPEED);         
    delay(20000/TURN_SPEED);
    
    if(DIRECTION == RIGHT){
        motor.speed(0,TURN_SPEED);
        motor.speed(1,-TURN_SPEED);    
    }

    else{
        motor.speed(0,-TURN_SPEED);
        motor.speed(1,TURN_SPEED);    
    }
    delay(20000/TURN_SPEED);   
    
    bool is_on_tape;
    while((millis() - initial_turn_timer) < TURN_TIMER){
        is_on_tape = digitalRead(LEFT_TAPEFOLLOWER) || digitalRead(RIGHT_TAPEFOLLOWER);

        if(!is_on_tape && has_turned == false){
            has_turned == true;
        }

        if(is_on_tape && has_turned == true){
            LCD.clear();
            LCD.print("TOO MUCH");
        
        }
    }
    
    while(!is_on_tape){
        is_on_tape = digitalRead(LEFT_TAPEFOLLOWER) || digitalRead(RIGHT_TAPEFOLLOWER);
    }
}

void navigate(bool front_intersection_valid, 
                bool left_intersection_valid,
                bool right_intersection_valid){
    
    if(navigation_mode == RANDOM_LOST){
        navigate_random(front_intersection_valid, left_intersection_valid, right_intersection_valid);
    }
    
    if(navigation_mode == FIXEDPATH_RIGHT){
        navigate_fixedpath_right(front_intersection_valid, left_intersection_valid, right_intersection_valid);
    }
    
}

void navigate_random(bool front_intersection_valid, 
                bool left_intersection_valid,
                bool right_intersection_valid){

        //initiate random turning
    int rand = random(0,3);
    bool can_turn = false;
    
    //Generate random numbers to turn until we come across a valid random number.
    while(!can_turn){
        if(rand == 0 && front_intersection_valid){
            can_turn = true;
        }
        
        else if (rand == 1 && left_intersection_valid){
            turn(LEFT);
            can_turn = true;
        }
        
        else if (rand == 2 && right_intersection_valid){
            turn(RIGHT);
            can_turn = true;
        }
        
        else{
            rand = random(0,3);
            can_turn = false;
        }
    }
}

void navigate_fixedpath_right(bool front_intersection_valid, 
                bool left_intersection_valid,
                bool right_intersection_valid){
    
    if(turn_number == 1){       
        turn(RIGHT);
    }
    
    if(turn_number == 2){
        turn(LEFT);
    }
    
    if(turn_number == 3){
        turn(LEFT);
    }
    
    if(turn_number == 4){
        turn(LEFT)
    }
    
    if(turn_number == 5){
        turn(LEFT);
    }
    
    if(turn_number == 6){
        turn(RIGHT);
    }
    
    if(turn_number == 7){
        turn(RIGHT);
    }
}

void collision_turn_around(){
    
}
