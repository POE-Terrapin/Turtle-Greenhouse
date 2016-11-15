/*
   Terrapin Project
   PoE Fall 2016
   Zarin Bhuiyan, Jamie Cho, Kaitlyn Keil, Lauren Pudvan, Katya Soltan

   Sketch Model Code
   Sprint 3
   Started 11/15
*/

// Make servo objects for the legs
#include <Servo.h>
Servo right_femur_servo;
Servo right_tibia_servo;
Servo left_femur_servo;
Servo left_tibia_servo;

// Constant values and pins.

// Set up the pins for the different sensors
//  LS stands for Light Sensor. Numbers increase
//  clockwise from the front: LS0 is front, LS1 is
//  at 3 o'clock (from top view), LS2 is in back, etc.
const int LS0 = 0, LS2 = 1;// LS1 = A1, LS3 = A3;

// Set up multiplexer pins
const int s[3] = {2, 3, 4};
const int multi_pin = A0; // pin to read from/output pin

// Eye1 and Eye2 power LEDs.
const int Eye1 = 8, Eye2 = 9;

// IR sensor pins on the multiplexer
//  f_ir indicates 'front IR', r_ir is 'right IR', l_ir is 'left IR'
const int f_ir = 4, r_ir = 3; // l_ir=4; // there is currently no left sensor

// Calibration code. Values are subject to change.
//  SEE_LIGHT implies that if the sensor value is greater
//  than this value, it is in light.
//  DISTANCE_LIMIT is the value above which, the IRs sense
//  an object in that direction.
const int SEE_LIGHT = 100, DISTANCE_LIMIT = 150;

// Create the variables to store sensor values. Each
//  corresponds with the matching name.
int LS0val = 0, LS1val = 0, LS2val = 0, LS3val = 0;
int f_value = 0, r_value = 0, l_value = 0;

// Create a variable to keep track of what direction the turtle is moving (straight=0, right turn=1, left turn = 2)
int directions = 0;

int norm_f_value, norm_r_value, norm_l_value; // Prepare values to normalize IR sensors

void setup() {
  Serial.begin(9600); // start Serial to see values
  Serial.println("Starting Up...");

  // use PWM pins for servos, multiplexer selection, and eyes
  right_femur_servo.attach(5);
  right_tibia_servo.attach(6);
  left_femur_servo.attach(10);
  left_tibia_servo.attach(11);
  right_tibia_servo.write(180);
  left_tibia_servo.write(180);

  // Set up multiplexer pins
  pinMode(s[0], OUTPUT);
  pinMode(s[1], OUTPUT);
  pinMode(s[2], OUTPUT);

  // Turn on both of the LEDs for the eyes.
  pinMode(Eye1, OUTPUT);
  digitalWrite(Eye1, HIGH);
  pinMode(Eye2, OUTPUT);
  digitalWrite(Eye2, HIGH);
  digitalWrite(Eye1, LOW);
  digitalWrite(Eye2, LOW);
  // Start up turtle without anything around the head so that initial values are
  //  useable for normalizing
  // Set up global values to normalize the sensors
  norm_f_value = readMultiplexer(f_ir);
  norm_r_value = readMultiplexer(r_ir);
  //  norm_l_value = readMultiplexer(l_ir);
  Serial.print(norm_f_value);
  Serial.print(',');
  Serial.println(norm_r_value);
  //  Serial.print(',');
  //  Serial.println(norm_l_value);
  delay(2000); // 2 second delay
  Serial.println('Start');
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
boolean check_sensor_flag = true;
//boolean left_moving = true;

float right_tibia = right_tibia_down;
float right_femur = right_femur_left;
float left_tibia = left_tibia_down;
float left_femur = left_femur_left;

float delta = 1; // degrees servo moves each step
float total_right_deg_travel = delta * (right_femur_left - right_femur_right) + 2 * (right_tibia_down - right_tibia_up);
float total_left_deg_travel = delta * (left_femur_left - right_femur_right) + 2 * (right_tibia_down - right_tibia_up);

float delta_left_backwards = (left_femur_left - left_femur_right) / total_right_deg_travel;
float delta_right_backwards = (right_femur_left - right_femur_right) / total_left_deg_travel;

void loop() {
  if (check_sensor_flag) {
    // Read the light sensors
    LS0val = readMultiplexer(LS0);
    LS2val = readMultiplexer(LS2);

    Serial.print("Light Sensors: ");
    Serial.print(LS0val);
    Serial.print(',');
    Serial.println(LS2val);

    directions = checkPath(LS0val);
    check_sensor_flag = false;
  }
  movement(directions);
}


int checkPath(int LSval) {
  // Reads, normalizes and compares the IR sensor values. Returns 0 for forward,
  //  1 for turn right, and 2 for turn left
  // LSval is the value of the front light sensor
  f_value = readMultiplexer(f_ir);
  r_value = readMultiplexer(r_ir);

  // Normalize the values based on first read
  int f_normalized = f_value - norm_f_value;
  int r_normalized = r_value - norm_r_value;
  Serial.print("IR Sensors: ");
  Serial.print(f_normalized);
  Serial.print(',');
  Serial.println(r_normalized);

  if (LSval > SEE_LIGHT) {
    if (abs(f_normalized) > DISTANCE_LIMIT) {
      return turnDecision(r_normalized);
    }
    else {
      // Otherwise, keep going forward
      return 0;
    }
  }
  else {
    return turnDecision(r_normalized);
  }
}

int turnDecision(int side_ir_val) {
  // Decides which direction to turn based on the value of the side IR sensor
  if (side_ir_val < DISTANCE_LIMIT)  {
    // If there is nothing to the right, tell the turtle
    // to turn right
    return 1;
  }
  else {
    // Otherwise, turn left
    return 2;
  }
}

void movement(int direction_to_move) {
  if (direction_to_move == 0) {
    moveForward();
  }
  else if (direction_to_move == 1) {
    turnRight();
  }
  else {
    turnLeft();
  }
}

void moveForward() {
  if (right_moving) {
    moveRightForward();
    moveLeftBackward();
  }
  else {
    moveLeftForward();
    moveRightBackward();
  }
}

void turnLeft() {
  // Not yet implemented
}

void turnRight() {
  // Not yet implemented
}

void moveRightForward() {
  if (right_femur <= right_femur_left && right_tibia >= right_tibia_up) {
    // move tibia up
    right_tibia -= delta;
    right_tibia_servo.write(right_tibia);
  }
  else if (right_tibia <= right_tibia_up && right_femur <= right_femur_left) {
    // move femur left
    right_femur += delta;
    right_femur_servo.write(right_femur);
  }
  else if (right_femur >= right_femur_left && right_tibia <= right_tibia_down) {
    // move tibia down
    right_tibia += delta;
    right_tibia_servo.write(right_tibia);
  }
  else if (right_femur >= right_femur_left && right_tibia >= right_tibia_down) {
    right_moving = false;
    check_sensor_flag = true;
    //     set left legs to starting pos
    left_tibia = left_tibia_down;
    left_femur = left_femur_left;
    left_tibia_servo.write(left_tibia);
    left_femur_servo.write(left_femur);

  }
}

void moveRightBackward(){
  Serial.println(right_femur);
  if(right_femur > right_femur_left){
    // keep moving right
    right_femur -= delta_right_backwards;
    right_femur_servo.write(right_femur);
  }
}

void moveLeftForward() {
  Serial.print("Left Femur: ");
  Serial.print(left_femur);
  Serial.println();
  Serial.print("Left Tibia: ");
  Serial.print(left_tibia);
  Serial.println();
  if (left_femur >= left_femur_left && left_tibia > left_tibia_up) {
    // move tibia up
    left_tibia -= delta;
    left_tibia_servo.write(left_tibia);
  }
  else if (left_tibia <= left_tibia_up && left_femur > left_femur_right) {
    // move femur left
    left_femur -= delta;
    left_femur_servo.write(left_femur);
  }
  else if (left_femur <= left_femur_right && left_tibia < left_tibia_down) {
    // move tibia down
    left_tibia += delta;
    left_tibia_servo.write(left_tibia);
  }
  else if (left_femur <= left_femur_right && left_tibia >= left_tibia_down) {
    right_moving = true;
    check_sensor_flag = true;
    // set right legs to starting pos
    right_tibia = right_tibia_down;
    right_femur = right_femur_right;
    right_tibia_servo.write(right_tibia);
    right_femur_servo.write(right_femur);

  }
}

void moveLeftBackward(){
  if(left_femur < left_femur_left){
    // keep moving left
    left_femur += delta_left_backwards;
    left_femur_servo.write(left_femur);
  }
}

// Returns the value of the requested pin from the multiplexer
int readMultiplexer(int pin_num) {
  digitalWrite(s[0], (pin_num & 1) ? HIGH : LOW);
  digitalWrite(s[1], (pin_num & 2) ? HIGH : LOW);
  digitalWrite(s[2], (pin_num & 4) ? HIGH : LOW);
  return analogRead(multi_pin);
}
