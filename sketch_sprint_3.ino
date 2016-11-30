/*
   Terrapin Project
   PoE Fall 2016
   Zarin Bhuiyan, Jamie Cho, Kaitlyn Keil, Lauren Pudvan, Katya Soltan

   Sketch Model Code
   Sprint 3
   Started 11/30
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Servo.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor leg1 = AFMS.getMotor(1), leg2 = AFMS.getMotor(2);

// Constant values and pins.

// Set up the pins for the different sensors
//  LS stands for Light Sensor. Numbers increase
//  clockwise from the front: LS0 is front, LS1 is
//  at 3 o'clock (from top view), LS2 is in back, etc.
const int LS0 = 0, LS2 = 1;

// Set up multiplexer pins
const int s[3] = {2, 3, 4};
const int multi_pin = A0; // pin to read from/output pin

// Eye1 and Eye2 power LEDs.
const int Eye1 = 8, Eye2 = 9;

// IR sensor pins on the multiplexer
//  f_ir indicates 'front IR', r_ir is 'right IR', l_ir is 'left IR'
const int f_ir = 2, r_ir = 3, l_ir=4; // there is currently no left sensor

// Calibration code. Values are subject to change.
//  SEE_LIGHT implies that if the sensor value is greater
//  than this value, it is in light.
//  DISTANCE_LIMIT is the value above which, the IRs sense
//  an object in that direction.
const int DISTANCE_LIMIT = 250;

// Create the variables to store sensor values. Each
//  corresponds with the matching name.
int LS0val = 0, LS1val = 0, LS2val = 0, LS3val = 0;
int f_value = 0, r_value = 0;

// Create a variable to keep track of what direction the turtle is moving (straight=0, right turn=1, left turn = 2)
int directions = 0;

void setup() {
  Serial.begin(9600); // start Serial to see values
  AFMS.begin();
  leg1->run(RELEASE);
  leg2->run(RELEASE);

  // Set up multiplexer pins
  pinMode(s[0], OUTPUT);
  pinMode(s[1], OUTPUT);
  pinMode(s[2], OUTPUT);

  // Turn on both of the LEDs for the eyes.
  pinMode(Eye1, OUTPUT);
  digitalWrite(Eye1, HIGH);
  pinMode(Eye2, OUTPUT);
  digitalWrite(Eye2, HIGH);

  // Figure out the value for 'light'
  LS0val = readMultiplexer(LS0);
  LS2val = readMultiplexer(LS2);
  const int SEE_LIGHT = (LS0val + LS2val)/2 - 100;
  
  delay(2000); // 2 second delay
}

void loop() {
  LS0val = readMultiplexer(LS0);
  LS2val = readMultiplexer(LS2);

  Serial.print("Light Sensors: ");
  Serial.print(LS0val);
  Serial.print(',');
  Serial.println(LS2val);

  directions = checkPath(LS0val);
  movement(directions);
}


int checkPath(int LSval) {
  // Reads, normalizes and compares the IR sensor values. Returns 0 for forward,
  //  1 for turn right, and 2 for turn left
  // LSval is the value of the front light sensor
  f_value = readMultiplexer(f_ir);
  r_value = readMultiplexer(r_ir);
  
  Serial.print("IR Sensors: ");
  Serial.print(f_value);
  Serial.print(',');
  Serial.println(r_value);

  if (LSval > SEE_LIGHT) {
    if (abs(f_value) > DISTANCE_LIMIT) {
      return turnDecision(r_value);
    }
    else {
      // Otherwise, keep going forward
      return 0;
    }
  }
  else {
    return turnDecision(r_value);
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
  leg1->setSpeed(50);
  leg2->setSpeed(50);

  leg1->run(FORWARD);
  leg2->run(FORWARD);
}

void turnLeft() {
  leg1->run(RELEASE);
  leg2->run(FORWARD);
}

void turnRight() {
  leg1->run(FORWARD);
  leg2->run(RELEASE);
}


// Returns the value of the requested pin from the multiplexer
int readMultiplexer(int pin_num) {
  digitalWrite(s[0], (pin_num & 1) ? HIGH : LOW);
  digitalWrite(s[1], (pin_num & 2) ? HIGH : LOW);
  digitalWrite(s[2], (pin_num & 4) ? HIGH : LOW);
  return analogRead(multi_pin);
}
