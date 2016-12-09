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
#include "gyro.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor* legL = AFMS.getMotor(4);
Adafruit_DCMotor* legR = AFMS.getMotor(3);

// Servos on pins 6 and 10

// Constant values and pins.

// Values for leg movement and sensors
const int GROUND_L_PIN = 11;// 0 when ON, 1 when off
const int GROUND_R_PIN = 10; // 0 when ON, 1 when off
const int ON_OFF_PIN = 9; // 0 when ON, 1 when OFF

enum {LEFT_MOVE, LEFT_UP, RIGHT_MOVE, RIGHT_UP};
int state;
float last_transition;

Gyro gyro(A1, A2, A3);

// Set up the pins for the different sensors
//  LS stands for Light Sensor. Numbers increase
//  clockwise from the front: LS0 is front, LS1 is
//  at 3 o'clock (from top view), LS2 is in back, etc.
const int LS0 = 0, LS2 = 1;// LS1 = A1, LS3 = A3;

// Set up multiplexer pins
const int s[3] = {2, 3, 4};
const int multi_pin = A0; // pin to read from/output pin

// RGB LEDs have color controlled by following pins
const int eye_g = 12, eye_b = 11, eye_r = 7;

// IR sensor pins on the multiplexer
//  f_ir indicates 'front IR', r_ir is 'right IR', l_ir is 'left IR'
const int f_ir = 2, r_ir = 3, l_ir=4;

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
  gyro.setup();

  pinMode(GROUND_L_PIN, INPUT_PULLUP);
  pinMode(GROUND_R_PIN, INPUT_PULLUP);
  pinMode(ON_OFF_PIN, INPUT_PULLUP);
  
  legL->setSpeed(100);
  legR->setSpeed(100);
  
  legL->run(RELEASE);
  legR->run(RELEASE);

  bool gl = ground('l');
  bool gr = ground('r');
  bool r = !digitalRead(GROUND_R_PIN);

  if (gl ^ gr) {
    state = gl ? RIGHT_MOVE : LEFT_MOVE;
  } else {
    // both or neither switches triggered
    state = RIGHT_MOVE; // pick a motion
  }
  last_transition = time_sec();

  // Set up multiplexer pins
  pinMode(s[0], OUTPUT);
  pinMode(s[1], OUTPUT);
  pinMode(s[2], OUTPUT);

  // Turn on both of the LEDs for the eyes.
  pinMode(eye_g, OUTPUT);
  pinMode(eye_r, OUTPUT);
  pinMode(eye_b, OUTPUT);
  setColor(0, 255, 0);  // green

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
        setColor(255, 255, 0);// turn eyes yellow
        choice = choice - 4;
     }
     else {
        setColor(0, 255, 0);  // turn eyes green
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
  if (digitalRead(ON_OFF_PIN) == 1) {
    //Stop, don't move.

    legL->run(RELEASE);
    legR->run(RELEASE);

    Serial.println("OFF");
    delay(1000);
  } else {
    bool gl = ground('l');
    bool gr = ground('r');
    int next = state;

    switch (state) {
      case LEFT_MOVE:
        legL->run(FORWARD);
        legR->run(RELEASE);
        if (!gl) {
          next = LEFT_UP;
        }
        break;
      case LEFT_UP:
        legL->run(FORWARD);
        legR->run(RELEASE);
        if (gl) {
          next = RIGHT_MOVE;
        }
        break;
      case RIGHT_MOVE:
        legL->run(RELEASE);
        legR->run(FORWARD);
        if (!gr) {
          next = RIGHT_UP;
        }
        break;
      case RIGHT_UP:
        legL->run(RELEASE);
        legR->run(FORWARD);
        if (gr) {
          next = LEFT_MOVE;
        }
        break;
    }

    if (next != state) {
      float now = time_sec();
      Serial.println(now - last_transition);
      last_transition = now;
      Serial.print(state); Serial.print("-->"); Serial.println(next);
      state = next;
    }
    delay(100);
  }

  gyro.read();
  Serial.println("Forward");
}

void turnLeft() {
  legL->run(RELEASE);
  legR->run(FORWARD);
  Serial.println("Left");
}

void turnRight() {
  legL->run(FORWARD);
  legR->run(RELEASE);
  Serial.println("Right");
}

void stopMoving() {
  legL->run(RELEASE);
  legR->run(RELEASE);
  Serial.println("Stop");
}

void setColor(int red, int green, int blue)
{
  analogWrite(eye_r, red);
  analogWrite(eye_g, green);
  analogWrite(eye_b, blue);  
}

// Returns the value of the requested pin from the multiplexer
int readMultiplexer(int pin_num) {
  digitalWrite(s[0], (pin_num & 1) ? HIGH : LOW);
  digitalWrite(s[1], (pin_num & 2) ? HIGH : LOW);
  digitalWrite(s[2], (pin_num & 4) ? HIGH : LOW);
  return analogRead(multi_pin);
}

// LEG MOVEMENT CODE
bool ground(char side) {
  switch (side) {
    case 'r':
      return !digitalRead(GROUND_R_PIN);
    case 'l':
      return !digitalRead(GROUND_L_PIN);
  }
  return 0;
}

float time_sec() {
  return millis() / 1000.;
}

