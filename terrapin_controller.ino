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
Adafruit_DCMotor *leg1 = AFMS.getMotor(4), *leg2 = AFMS.getMotor(3);


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
const int f_ir = 2, r_ir = 3, l_ir=3;

// Temperature and moisture sensors
const int ts = 5, ms = 6;

// Create the variables to store sensor values. Each
//  corresponds with the matching name.
int LS0val = 0, LS1val = 0, LS2val = 0, LS3val = 0;
int f_value = 0, r_value = 0, l_value = 0;

// Create a variable to keep track of what direction the turtle is moving (straight=0, right turn=1, left turn = 2)
int directions = 0;

void setup() {
  Serial.begin(9600); // start Serial for RPi

  Serial.begin(9600); // start Serial to see values
  AFMS.begin();
  leg1->setSpeed(50);
  leg2->setSpeed(50);
  
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

  // Start up turtle without anything around the head so that initial values are
  //  useable for calibrating

  delay(2000); // 2 second delay
}

void loop() {
  // Read the light sensors
  readSensors();
  directions = checkChoice();
  moveTurtle(directions);
  delay(100);
}

void testRun() {
 // Sends rubbish data to the RPi for testing
 LS0val = 400;
 LS2val = 400;
 
 f_value = 250;
 r_value = 250;
 l_value = 250;
  
 int ts_value = 25;
 int ms_value = 50;

 serialWrite(LS0val);
 serialWrite(LS2val);
 serialWrite(f_value);
 serialWrite(r_value);
 serialWrite(l_value);
 serialWrite(ts_value);
 Serial.println(ms_value);
}

void readSensors() {
  // Reads the sensors and writes them to Serial
  LS0val = readMultiplexer(LS0);
  LS2val = readMultiplexer(LS2);
  
  f_value = readMultiplexer(f_ir);
  r_value = readMultiplexer(r_ir);
  l_value = readMultiplexer(l_ir);
  
  // int ts_value = readMultiplexer(ts);
  // int ms_value = readMultiplexer(ms);
  int ts_value = 25;
  int ms_value = 50;

  serialWrite(LS0val);
  serialWrite(LS2val);
  serialWrite(f_value);
  serialWrite(r_value);
  serialWrite(l_value);
  serialWrite(ts_value);
  Serial.println(ms_value);
}

void serialWrite(int value) {
  // prints the value followed by a comma
  Serial.print(value);
  Serial.print(',');
}

int checkChoice() {
  // Reads the direction choice from the raspberry pi
  //  Returns 0 for forward, 1 for right, 2 for left,
  //  and 3 for stop
  // If Serial is not available, returns directions
  //  so that it continues to follow the same path.
  if(Serial.available()) {
     int choice = Serial.read();
     if(choice > 52) { 
        digitalWrite(Eye1, LOW);
        digitalWrite(Eye2, LOW);
        choice = choice - 4;
     }
     else {
        digitalWrite(Eye1, HIGH);
        digitalWrite(Eye2, HIGH);
     }
    return choice - 48;
  }
  else {
    return directions;
  }
}

void moveTurtle(int whichWay) {
  if(whichWay == 0) {
    moveForward();
  }
  else if(whichWay == 1) {
    turnRight();
  }
  else if(whichWay == 2) {
    turnLeft();
  }
  else {
    stopMoving();
  }
}

void moveForward() {
  leg1->run(FORWARD);
  leg2->run(FORWARD);
  Serial.println("Forward");
}

void turnLeft() {
  leg1->run(RELEASE);
  leg2->run(FORWARD);
  Serial.println("Left");
}

void turnRight() {
  leg1->run(FORWARD);
  leg2->run(RELEASE);
  Serial.println("Right");
}

void stopMoving() {
  leg1->run(RELEASE);
  leg2->run(RELEASE);
  Serial.println("Stop");
}

// Returns the value of the requested pin from the multiplexer
int readMultiplexer(int pin_num) {
  digitalWrite(s[0], (pin_num & 1) ? HIGH : LOW);
  digitalWrite(s[1], (pin_num & 2) ? HIGH : LOW);
  digitalWrite(s[2], (pin_num & 4) ? HIGH : LOW);
  return analogRead(multi_pin);
}
