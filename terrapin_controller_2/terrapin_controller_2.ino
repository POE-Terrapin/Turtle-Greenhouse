/*
   Terrapin Project
   PoE Fall 2016
   Zarin Bhuiyan, Jamie Cho, Kaitlyn Keil, Lauren Pudvan, Katya Soltan
   Final Demo Code
   Started 12/14
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Servo.h>
#include "sensors.h"
#include "utils.h"


// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor* motor_l = AFMS.getMotor(4);
Adafruit_DCMotor* motor_r = AFMS.getMotor(3);

const int FULL_SPEED = 128;
const int HALF_SPEED = FULL_SPEED *3 / 4;

// servos
Servo servo_l, servo_r;

int servo_r_pos = 0;
int servo_r_dir = 1;
const int D_THETA = 20;

const int SERVO_L_PIN = 9;
const int SERVO_R_PIN = 10;

// behaviors and movement
int behavior = STOP;
int state;
float last_transition;

// RGB LEDs have color controlled by following pins
const int eye_g = 12, eye_b = 11, eye_r = 7;

// Create a variable to keep track of what direction the turtle is moving (straight=0, right turn=1, left turn = 2)

void setup() {
  Serial.begin(9600); // start Serial for RPi

  AFMS.begin();

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

  if (gl ^ gr) { // if gl != gr, aka both not on the ground
    state = gl ? RIGHT_MOVE : LEFT_MOVE; // if gl is on the ground, right foot moves, otherwise left foot
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
  // Read all sensors
  //readSensors();
  //serialWrite("gl : ", !contact.get(0), ", gr : ", !contact.get(1));
  
  if (!on()) {
    behavior = STOP;
    moveTurtle(STOP);
    Serial.println("OFF");
    delay(1000);
  } else {
    behavior = checkChoice();
    moveTurtle(behavior);
  }
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
    if (choice > '4') {
      setColor(255, 255, 0);// turn eyes yellow - warning
      return choice - '4';
    }
    else {
      setColor(0, 255, 0);  // turn eyes green
      return choice - '0';
    }
  }
  return behavior;
}

void moveTurtle(int whichWay) {
  int next = state;

  if (whichWay == GO_FORWARD) {
    next = go_forward(state);
  }
  else if (whichWay == TURN_LEFT) {
    next = turn_left(state);
    //next = go_forward(state);
  }
  else if (whichWay == TURN_RIGHT) {
    next = turn_right(state);
    //turnRight();
    
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
float time_sec() {
  return millis() / 1000.;
}

int go_forward(int state) {
  
  Serial.println("GOING FORWARD!");
  bool gl = !contact.get(0);
  bool gr = !contact.get(1);

  int next = state;
  servo_l.write(180);
  servo_r.write(0);

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
  Serial.println("TURNING LEFT!");
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


int turn_right(int state) {
  Serial.println("TURNING RIGHT!");
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
  if (!gr) {
    // no ground contact on right foot
    //set right foot down as pivot
    motor_l->setSpeed(HALF_SPEED);
    motor_l->run(FORWARD);
    motor_r->setSpeed(HALF_SPEED);
    motor_r->run(FORWARD);
  } else {
    // fix left leg position
    motor_r->run(RELEASE);
    if (gl) { //left leg ground contact
      if (servo_l_pos > 0 && servo_l_dir == -1) {
        // can move servo
        servo_l_pos = max(servo_l_pos - D_THETA, 0);
        motor_l->setSpeed(HALF_SPEED);
        motor_l->run(FORWARD);
        servo_l.write(servo_l_pos);
      } else {
        // can't move servo anymore
        /// lift leg and return servo
        servo_l_dir = 1;
        motor_l->setSpeed(FULL_SPEED);
        motor_l->run(FORWARD);
      }

    } else {
      // no ground contact on right leg
      motor_r->run(RELEASE); // need left leg support
      if (servo_l_pos < 180 && servo_l_dir == 1) {
        // return servo
        servo_l_pos = min(servo_l_pos + D_THETA, 180);
        motor_l->run(RELEASE);
        servo_l.write(servo_l_pos);
      } else {
        // done returning servo
        servo_l_dir = -1;
        motor_l->setSpeed(FULL_SPEED);
        motor_l->run(FORWARD);
      }
    }

  }
  return next;
}

void turnRight() {
  Serial.println("TURNING RIGHT!");
  if (!on()) {
    //Stop, don't move.

    motor_l->setSpeed(FULL_SPEED);
    motor_r->setSpeed(FULL_SPEED);
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
  Serial.println("Right");
}

void stopMoving() {
  motor_l->run(RELEASE);
  motor_r->run(RELEASE);
}

void report() {
  serialWrite(LS0val, LS2val, f_value, r_value, l_value, ts_value, ms_value, behavior, gyro.x, gyro.y, gyro.z);
}
