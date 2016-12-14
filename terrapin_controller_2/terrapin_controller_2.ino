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
#include "sensors.h"
#include "gyro.h"
#include "utils.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor* motor_l = AFMS.getMotor(4);
Adafruit_DCMotor* motor_r = AFMS.getMotor(3);

Servo servo_l, servo_r;

int servo_r_pos = 0;
int servo_r_dir = 1;
const int D_THETA = 20;

// Servos on pins 6 and 10

// Constant values and pins.

const int FULL_SPEED = 150;
const int HALF_SPEED = FULL_SPEED / 2;

// Values for leg movement and sensors
const int GROUND_L_PIN = 5;// 0 when ON, 1 when off
const int GROUND_R_PIN = 6; // 0 when ON, 1 when off
const int ON_OFF_PIN = 8; // 0 when ON, 1 when OFF

const int SERVO_L_PIN = 9;
const int SERVO_R_PIN = 10;

enum {LEFT_MOVE, LEFT_UP, RIGHT_MOVE, RIGHT_UP};
enum {STOP, GO_FORWARD, TURN_LEFT, TURN_RIGHT};

int behavior = STOP;
int state;
float last_transition;

Gyro gyro(A1, A2, A3);


// RGB LEDs have color controlled by following pins
const int eye_g = 12, eye_b = 11, eye_r = 7;

// Create a variable to keep track of what direction the turtle is moving (straight=0, right turn=1, left turn = 2)

void setup() {
  Serial.begin(9600); // start Serial for RPi

  AFMS.begin();
  gyro.setup();

  pinMode(GROUND_L_PIN, INPUT_PULLUP);
  pinMode(GROUND_R_PIN, INPUT_PULLUP);
  pinMode(ON_OFF_PIN, INPUT_PULLUP);

  pinMode(SERVO_L_PIN, OUTPUT);
  pinMode(SERVO_R_PIN, OUTPUT);

  servo_l.attach(SERVO_L_PIN);
  servo_r.attach(SERVO_R_PIN);

  motor_l->setSpeed(HALF_SPEED);
  motor_r->setSpeed(HALF_SPEED);

  motor_l->run(RELEASE);
  motor_r->run(RELEASE);

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
  if (digitalRead(ON_OFF_PIN) == 1) {
    behavior = STOP;
    Serial.println("OFF");
    delay(1000);
  }
  behavior = checkChoice();
  moveTurtle(behavior);
  report();
  delay(100);
}

int checkChoice() {
  // Reads the direction choice from the raspberry pi
  //  Returns 0 for forward, 1 for right, 2 for left,
  //  and 3 for stop
  // If Serial is not available, returns directions
  //  so that it continues to follow the same path.

  if (Serial.available()) {
    int choice = Serial.read();
    if (choice > 52) {
      setColor(255, 255, 0);// turn eyes yellow
      choice = choice - 4;
    }
    else {
      setColor(0, 255, 0);  // turn eyes green
    }
    return choice - 48;
  }
  else {
    return behavior;
  }
}

void moveTurtle(int whichWay) {
  int next = state;

  if (whichWay == GO_FORWARD) {
    next = go_forward(state);
  }
  else if (whichWay == TURN_LEFT) {
    next = turn_left(state);
  }
  else if (whichWay == TURN_RIGHT) {
    turnRight();
  }
  else {
    stopMoving();
  }

  if (next != state) {
    float now = time_sec();
    Serial.println(now - last_transition);
    last_transition = now;
    Serial.print(state); Serial.print("-->"); Serial.println(next);
    state = next;
  }
}

void setColor(int red, int green, int blue)
{
  analogWrite(eye_r, red);
  analogWrite(eye_g, green);
  analogWrite(eye_b, blue);
}

// Returns the value of the requested pin from the multiplexer

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

int go_forward(int state) {
  bool gl = ground('l');
  bool gr = ground('r');

  int next = state;
  servo_l.write(0);
  servo_r.write(180);

  switch (state) {
    case LEFT_MOVE:
      motor_l->run(FORWARD);
      motor_r->run(RELEASE);
      if (!gl) {
        next = LEFT_UP;
        motor_r->setSpeed(HALF_SPEED);
      }
      break;
    case LEFT_UP:
      motor_l->run(FORWARD);
      motor_r->run(FORWARD);
      if (gl) {
        next = RIGHT_MOVE;
        motor_r->setSpeed(FULL_SPEED);
      }
      break;
    case RIGHT_MOVE:
      motor_l->run(RELEASE);
      motor_r->run(FORWARD);
      if (!gr) {
        next = RIGHT_UP;
        motor_l->setSpeed(HALF_SPEED);
      }
      break;
    case RIGHT_UP:
      motor_l->run(FORWARD );
      motor_r->run(FORWARD);
      if (gr) {
        next = LEFT_MOVE;
        motor_l->setSpeed(FULL_SPEED);
      }
      break;
  }
  return next;
}

int turn_left(int state) {

  bool gl = ground('l');
  bool gr = ground('r');

  int next = state;
  // still want to alternate between legs

  switch (state) {
    case LEFT_MOVE:
      break;
    case LEFT_UP:
      break;
    case RIGHT_MOVE:
      break;
    case RIGHT_UP:
      break;
  }
  if (!gl) {
    // no ground contact on left foot
    //set left foot down as pivot
    motor_l->setSpeed(HALF_SPEED);
    motor_l->run(FORWARD);
    motor_r->setSpeed(HALF_SPEED);
    motor_r->run(FORWARD);
  } else {
    // fix left leg position
    motor_l->run(RELEASE);
    if (gr) { //right leg ground contact
      if (servo_r_pos > 0 && servo_r_dir == -1) {
        // can move servo
        servo_r_pos = max(servo_r_pos - D_THETA, 0);
        motor_r->setSpeed(HALF_SPEED);
        motor_r->run(FORWARD);
        servo_r.write(servo_r_pos);
      } else {
        // can't move servo anymore
        /// lift leg and return servo
        servo_r_dir = 1;
        motor_r->setSpeed(FULL_SPEED);
        motor_r->run(FORWARD);
      }

    } else {
      // no ground contact on right leg
      motor_l->run(RELEASE); // need left leg support
      if (servo_r_pos < 180 && servo_r_dir == 1) {
        // return servo
        servo_r_pos = min(servo_r_pos + D_THETA, 180);
        motor_r->run(RELEASE);
        servo_r.write(servo_r_pos);
      } else {
        // done returning servo
        servo_r_dir = -1;
        motor_r->setSpeed(FULL_SPEED);
        motor_r->run(FORWARD);
      }
    }

  }
  return next;
}

void turnRight() {
  if (digitalRead(ON_OFF_PIN) == 1) {
    //Stop, don't move.

    motor_l->run(RELEASE);
    motor_r->run(RELEASE);

    delay(1000);
  } else {
    bool gl = ground('l');
    bool gr = ground('r');
    int next = state;

    switch (state) {
      case LEFT_MOVE:
        motor_l->run(FORWARD);
        motor_r->run(RELEASE);
        if (!gl) {
          next = LEFT_MOVE;
        }
        break;
      case RIGHT_MOVE:
        motor_l->run(RELEASE);
        motor_r->run(FORWARD);
        if (!gr) {
          next = RIGHT_UP;
        }
        break;
      case RIGHT_UP:
        motor_l->run(RELEASE);
        motor_r->run(FORWARD);
        if (gr) {
          next = LEFT_MOVE;
        }
        break;
    }

    if (next != state) {
      float now = time_sec();
      last_transition = now;
      state = next;
    }
    delay(100);
  }
  gyro.read();
  Serial.println("Right");
}

void stopMoving() {
  motor_l->run(RELEASE);
  motor_r->run(RELEASE);
}

void report() {
  
  serialWrite(LS0val, LS2val, f_value, r_value, l_value, ts_value, ms_value, behavior);
}
