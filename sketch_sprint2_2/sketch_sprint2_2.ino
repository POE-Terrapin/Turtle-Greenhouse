/*
 * Terrapin Project
 * PoE Fall 2016
 * Zarin Bhuiyan, Jamie Cho, Kaitlyn Keil, Lauren Pudvan, Katya Soltan
 * 
 * Sketch Model Code
 * Sprint 2
 * Started 11/8
*/

// Motor set up placeholder (might be what we use longterm)
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Set up motors with pins. For sketch model, all of
//  motors will either work together or cause a turn.
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *frontMotor0 = AFMS.getMotor(1);
Adafruit_DCMotor *frontMotor1 = AFMS.getMotor(2);
Adafruit_DCMotor *backMotor0 = AFMS.getMotor(3);
Adafruit_DCMotor *backMotor1 = AFMS.getMotor(4);

// Constant values and pins.

// Set up the pins for the different sensors
//  LS stands for Light Sensor. Numbers increase
//  clockwise from the front: LS0 is front, LS1 is
//  at 3 o'clock (from top view), LS2 is in back, etc.
const int LS0 = 0, LS2 = 1;// LS1 = A1, LS3 = A3;

// Set up multiplexer pins
const int s0 = 0, s1 = 1, s2 = 2; // select pins
const int multi_pin = A0; // pin to read from/output pin

// Eye1 and Eye2 power LEDs.
const int Eye1 = 8, Eye2 = 9;

// IR sensor pins on the multiplexer
//  f_ir indicates 'front IR', r_ir is 'right IR', l_ir is 'left IR'
const int f_ir = 4, r_ir = 3; // l_ir=4; // there is currently no left sensor

// Calibration code. Values are subject to change.
//  see_light implies that if the sensor value is greater 
//  than this value, it is in light.
//  distance_limit is the value above which, the IRs sense
//  an object in that direction.
const int see_light = 100, distance_limit = 150;
const int must_move = 150; // drive value for wheels

// Create the variables to store sensor values. Each
//  corresponds with the matching name.
int LS0val = 0, LS1val = 0, LS2val = 0, LS3val = 0;
int f_value = 0, r_value = 0, l_value = 0;

int norm_f_value, norm_r_value, norm_l_value; // Prepare values to normalize IR sensors

void setup() {
  Serial.begin(9600); // start Serial to see values
  Serial.println("Starting Up...");
  
  AFMS.begin();
  Serial.println("Motor Shield Started...");
  // Run all motors forward to begin with (make sure
  //  they are facing the proper direction)
  runForward();
  Serial.println("Motors running forward...");
  
  // Set up multiplexer pins
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);

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

void loop() {
  // Read the light sensors
  LS0val = readMultiplexer(LS0);
  LS2val = readMultiplexer(LS2);
  Serial.print("Light Sensors: ");
  Serial.print(LS0val);
  Serial.print(',');
  Serial.println(LS2val);

  // Check the values to see if we need to move. If so, decide a direction.
  if(LS0val > see_light && LS2val < see_light) { // Back sensor is covered
    int directions = checkPath(); // checks to see which direction it should go
    movement(directions); // moves in that direction
  }
  else if(LS0val < see_light && LS2val > see_light) { // Front sensor is covered
    runBackward(); // Goes backwards
    timeToMove(); // Moves
  }
//  else if(LS0val < see_light && LS2val < see_light) { // Both sensors are covered
//    int directions = checkPath();
//    movement(directions);
//  }
  else { // Otherwise, just stop.
    timeToStop();
  }
}


int checkPath() {
  // Reads, normalizes and compares the IR sensor values. Returns 0 for forward,
  //  1 for turn right, and 2 for turn left
  f_value = readMultiplexer(f_ir);
  r_value = readMultiplexer(r_ir);
//  l_value = readMultiplexer(l_ir);

  // Normalize the values based on first read
  int f_normalized = f_value - norm_f_value;
  int r_normalized = r_value - norm_r_value;
//  int l_normalized = l_value - norm_l_value;
  Serial.print("IR Sensors: ");
  Serial.print(f_normalized);
  Serial.print(',');
  Serial.println(r_normalized);
//  Serial.print(',');
//  Serial.println(l_normalized);

  if (abs(f_normalized) > distance_limit) {
    // If there is something in front of us, check directions to turn
    if (r_normalized < distance_limit)  {
      // If there is nothing to the right, tell the turtle
      // to turn right
      return 1;
    }
    else {
      // Otherwise, turn left
      return 2;
    }
  }
  else {
    // Otherwise, keep going forward
    return 0;
  } 
}

void movement(int direction_to_move) {
  if (direction_to_move == 0) {
    runForward();
    timeToMove();
  }
  else if (direction_to_move == 1) {
    turnRight();
    timeToMove();
  }
  else {
    turnLeft();
    timeToMove();
  }
}

void runForward() {
  // Make all the motors run forward
  digitalWrite(Eye1, HIGH);
  digitalWrite(Eye2, HIGH);
  frontMotor0->run(FORWARD);
  backMotor0->run(FORWARD);
  frontMotor1->run(FORWARD);
  backMotor1->run(FORWARD);
}

void runBackward() {
  digitalWrite(Eye1, LOW);
  digitalWrite(Eye2, LOW);
  frontMotor0->run(BACKWARD);
  backMotor0->run(BACKWARD);
  frontMotor1->run(BACKWARD);
  backMotor1->run(BACKWARD);
}

void turnLeft() {
  digitalWrite(Eye1, HIGH);
  digitalWrite(Eye2, LOW);
  frontMotor0->run(RELEASE);
  backMotor0->run(RELEASE);
  frontMotor1->run(FORWARD);
  backMotor1->run(FORWARD);
}

void turnRight() {
  digitalWrite(Eye1, LOW);
  digitalWrite(Eye2, HIGH);
  frontMotor0->run(FORWARD);
  backMotor0->run(FORWARD);
  frontMotor1->run(RELEASE);
  backMotor1->run(RELEASE);
}

void timeToMove() {
  frontMotor0->setSpeed(must_move);
  backMotor0->setSpeed(must_move);
  frontMotor1->setSpeed(must_move);
  backMotor1->setSpeed(must_move);
}

void timeToStop() {
  digitalWrite(Eye1, HIGH);
  digitalWrite(Eye2, HIGH);
  frontMotor0->run(RELEASE);
  backMotor0->run(RELEASE);
  frontMotor1->run(RELEASE);
  backMotor1->run(RELEASE);
}

// Returns the value of the requested pin from the multiplexer

int readMultiplexer(int pin_num) {
  if (pin_num == 0) {
    return readM0();
  }
  else if (pin_num == 1) {
    return readM1();
  }
  else if (pin_num == 2) {
    return readM2();
  }
  else if (pin_num == 3) {
    return readM3();
  }
  else if (pin_num == 4) {
    return readM4();
  }
  else if (pin_num == 5) {
    return readM5();
  }
  else if (pin_num == 6) {
    return readM6();
  }
  else if (pin_num == 7) {
    return readM7();
  }
}

// Functions to read specific pins from the multiplexer
// High-Low things are based on http://www.ti.com/lit/ds/symlink/cd4051b.pdf
int readM0() {
  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  int pin_value = analogRead(multi_pin);
  return pin_value;
}

int readM1() {
  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  int pin_value = analogRead(multi_pin);
  return pin_value;
}

int readM2() {
  digitalWrite(s0, LOW);
  digitalWrite(s1, HIGH);
  digitalWrite(s2, LOW);
  int pin_value = analogRead(multi_pin);
  return pin_value;
}

int readM3() {
  digitalWrite(s0, HIGH);
  digitalWrite(s1, HIGH);
  digitalWrite(s2, LOW);
  int pin_value = analogRead(multi_pin);
  return pin_value;
}

int readM4() {
  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, HIGH);
  int pin_value = analogRead(multi_pin);
  return pin_value;
}

int readM5() {
  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
  digitalWrite(s2, HIGH);
  int pin_value = analogRead(multi_pin);
  return pin_value;
}

int readM6() {
  digitalWrite(s0, LOW);
  digitalWrite(s1, HIGH);
  digitalWrite(s2, HIGH);
  int pin_value = analogRead(multi_pin);
  return pin_value;
}

int readM7() {
  digitalWrite(s0, HIGH);
  digitalWrite(s1, HIGH);
  digitalWrite(s2, HIGH);
  int pin_value = analogRead(multi_pin);
  return pin_value;
}
