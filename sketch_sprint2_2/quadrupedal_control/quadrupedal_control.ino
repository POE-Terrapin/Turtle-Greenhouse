#include <Servo.h>
Servo right_femur_servo;
Servo right_tibia_servo;
Servo left_femur_servo;
Servo left_tibia_servo;

void setup() {
  Serial.begin(9600);
  // use PWM pins for servos
  right_femur_servo.attach(5);
  right_tibia_servo.attach(6);
  left_femur_servo.attach(10);
  left_tibia_servo.attach(11);

}
// Define angles servos move through
float right_femur_left = 140;
float right_femur_right = 90;
float right_tibia_down = 180;
float right_tibia_up = 90;

float left_femur_left = 140;
float left_femur_right = 90;
float left_tibia_down = 180;
float left_tibia_up = 90;

boolean right_moving = true;
//boolean left_moving = true;

float right_tibia = right_tibia_down;
float right_femur = right_femur_right;
float left_tibia = left_tibia_down;
float left_femur = left_femur_right;

float delta = 1; // degrees servo moves each step
float total_right_deg_travel = delta * (right_femur_left - right_femur_right) + 2 * (right_tibia_down - right_tibia_up);
float total_left_deg_travel = delta * (left_femur_left - right_femur_right) + 2 * (right_tibia_down - right_tibia_up);

float delta_left_backwards = (left_femur_left - left_femur_right) / total_right_deg_travel;
float delta_right_backwards = (right_femur_left - right_femur_right) / total_left_deg_travel;

void loop() {
  moveForward();
}

void moveForward(){
  if(right_moving){
    moveRightForward();
//    moveLeftForward();
    moveLeftBackward();
  }
  else{
    moveLeftForward();
    moveRightBackward();
//    moveLeftBackward();
  }
}

void moveRightForward(){
  if(right_femur <= right_femur_left && right_tibia >= right_tibia_up){
    // move tibia up
    right_tibia -= delta;
    right_tibia_servo.write(right_tibia);
  }
  else if(right_tibia <= right_tibia_up && right_femur <= right_femur_left){
    // move femur left
    right_femur += delta;
    right_femur_servo.write(right_femur);
  }
  else if(right_femur >= right_femur_left && right_tibia < right_tibia_down){
    // move tibia down
    right_tibia += delta;
    right_tibia_servo.write(right_tibia);
  }
  else if(right_femur >= right_femur_left && right_tibia >= right_tibia_down){
    right_moving = false;
//     set left legs to starting pos
    left_tibia = left_tibia_down;
    left_femur = left_femur_left;
    left_tibia_servo.write(left_tibia);
    left_femur_servo.write(left_femur);

  }
}


void moveLeftForward(){
  Serial.print("Left Femur: ");
  Serial.print(left_femur);
  Serial.println();
  Serial.print("Left Tibia: ");
  Serial.print(left_tibia);
  Serial.println();
  if(left_femur >= left_femur_left && left_tibia > left_tibia_up){
    // move tibia up
    left_tibia -= delta;
    left_tibia_servo.write(left_tibia);
  }
  else if(left_tibia <= left_tibia_up && left_femur > left_femur_right){
    // move femur left
    left_femur -= delta;
    left_femur_servo.write(left_femur);
  }
  else if(left_femur <= left_femur_right && left_tibia < left_tibia_down){
    // move tibia down
    left_tibia += delta;
    left_tibia_servo.write(left_tibia);
  }
  else if(left_femur <= left_femur_right && left_tibia >= left_tibia_down){
    right_moving = true;
//    left_moving = false;
    // set right legs to starting pos
    right_tibia = right_tibia_down;
    right_femur = right_femur_right;
    right_tibia_servo.write(right_tibia);
    right_femur_servo.write(right_femur);
  
}
}

void moveRightBackward(){
  Serial.println(delta_right_backwards);
  if(right_femur > right_femur_right){
    // keep moving right
    right_femur -= delta_right_backwards;
    right_femur_servo.write(right_femur);
  }
//  else{
//    right_moving = true;
//  }
}

void moveLeftBackward(){
  Serial.print("Left Femur: ");
  Serial.print(left_femur);
  Serial.println();
  Serial.print("Left Tibia: ");
  Serial.print(left_tibia);
  Serial.println();
  if(left_femur < left_femur_left){
    // keep moving left
    left_femur += delta_left_backwards;
    left_femur_servo.write(left_femur);
  }
//  else{
//    left_moving = true;
//  }
}

// moving backwards...not forwards
void moveRightForwardBackwardMov(){
  Serial.print("Femur: ");
  Serial.print(right_femur);
  Serial.println();
  Serial.print("Tibia: ");
  Serial.print(right_tibia);
  Serial.println();

  if(right_femur > right_femur_right && right_tibia > right_tibia_up){
    // move tibia up
    right_tibia -= delta;
    right_tibia_servo.write(right_tibia);
  }
  else if(right_tibia <= right_tibia_up && right_femur > right_femur_right){
    // move femur left
    right_femur -= delta;
    right_femur_servo.write(right_femur);
  }
  else if(right_femur <= right_femur_right && right_tibia < right_tibia_down){
    // move tibia down
    right_tibia += delta;
    right_tibia_servo.write(right_tibia);
  }
  else if(right_femur <= right_femur_right && right_tibia >= right_tibia_down){
    right_moving = false;
    // set left legs to starting pos
  }
}
void moveLeftForwardBackwardMov(){
  Serial.print("Femur: ");
  Serial.print(right_femur);
  Serial.println();
  Serial.print("Tibia: ");
  Serial.print(right_tibia);
  Serial.println();
  if(left_femur <= left_femur_left && left_tibia >= left_tibia_up){
    // move tibia up
    left_tibia -= delta;
    left_tibia_servo.write(left_tibia);
  }
  else if(left_tibia <= left_tibia_up && left_femur <= left_femur_left){
    // move femur left
    left_femur += delta;
    left_femur_servo.write(left_femur);
  }
  else if(left_femur >= left_femur_left && left_tibia < left_tibia_down){
    // move tibia down
    left_tibia += delta;
    left_tibia_servo.write(left_tibia);
  }
  else if(left_femur >= left_femur_left && left_tibia >= right_tibia_down){
    right_moving = true;
    // set right legs to starting pos
  }
}
