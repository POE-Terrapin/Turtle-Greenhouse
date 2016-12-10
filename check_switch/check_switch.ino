#include <Adafruit_MotorShield.h>
#include <Servo.h>
#include "gyro.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor* motor_l = AFMS.getMotor(4);
Adafruit_DCMotor* motor_r = AFMS.getMotor(3);

const int GROUND_L_PIN = 6;// 0 when ON, 1 when off
const int GROUND_R_PIN = 7; // 0 when ON, 1 when off
const int ON_OFF_PIN = 8; // 0 when ON, 1 when OFF

const int SERVO_L_PIN = 9;
const int SERVO_R_PIN = 10;

enum {LEFT_MOVE, LEFT_UP, RIGHT_MOVE, RIGHT_UP};

enum {STOP, GO_FORWARD, TURN_LEFT, TURN_RIGHT};

int behavior = TURN_LEFT;
int state;

float last_transition;

Servo servo_l, servo_r;

Gyro gyro(A1, A2, A3);

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

void setup() {
  Serial.begin(9600);
  AFMS.begin();
  gyro.setup();

  pinMode(GROUND_L_PIN, INPUT_PULLUP);
  pinMode(GROUND_R_PIN, INPUT_PULLUP);
  pinMode(ON_OFF_PIN, INPUT_PULLUP);

  pinMode(SERVO_L_PIN, OUTPUT);
  pinMode(SERVO_R_PIN, OUTPUT);

  servo_l.attach(SERVO_L_PIN);
  servo_r.attach(SERVO_R_PIN);

  motor_l->setSpeed(100);
  motor_r->setSpeed(100);

  motor_l->run(FORWARD);
  motor_r->run(FORWARD);

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
}

int go_forward(int state, bool gl, bool gr) {
  int next = state;
  servo_l.write(0);
  servo_r.write(180);

  switch (state) {
    case LEFT_MOVE:
      motor_l->run(FORWARD);
      motor_r->run(RELEASE);
      if (!gl) {
        next = LEFT_UP;
        motor_r->setSpeed(50);
      }
      break;
    case LEFT_UP:
      motor_l->run(FORWARD);
      motor_r->run(FORWARD);
      if (gl) {
        next = RIGHT_MOVE;
        motor_r->setSpeed(100);
      }
      break;
    case RIGHT_MOVE:
      motor_l->run(RELEASE);
      motor_r->run(FORWARD);
      if (!gr) {
        next = RIGHT_UP;
        motor_l->setSpeed(50);
      }
      break;
    case RIGHT_UP:
      motor_l->run(FORWARD );
      motor_r->run(FORWARD);
      if (gr) {
        next = LEFT_MOVE;
        motor_l->setSpeed(100);
      }
      break;
  }
  return next;
}

int servo_r_pos = 0;
int servo_r_dir = 1;
const int D_THETA = 20;

int turn_left(int state, bool gl, bool gr) {
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
    motor_l->setSpeed(50);
    motor_l->run(FORWARD);
    motor_r->setSpeed(50);
    motor_r->run(FORWARD);
  } else {
    // fix left leg position
    motor_l->run(RELEASE);
    if (gr) { //right leg ground contact
      if (servo_r_pos > 0 && servo_r_dir == -1) {
        // can move servo
        servo_r_pos = max(servo_r_pos - D_THETA, 0);
        motor_r->setSpeed(50);
        motor_r->run(FORWARD);
        servo_r.write(servo_r_pos);
      } else {
        // can't move servo anymore
        /// lift leg and return servo
        servo_r_dir = 1;
        motor_r->setSpeed(100);
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
        motor_r->setSpeed(100);
        motor_r->run(FORWARD);
      }
    }

  }
  return next;
}

void loop() {
  if (digitalRead(ON_OFF_PIN) == 1) {
    //Stop, don't move.

    motor_l->run(RELEASE);
    motor_r->run(RELEASE);

    Serial.println("OFF");
    delay(1000);
  } else {
    bool gl = ground('l');
    bool gr = ground('r');
    int next = state;


    switch (behavior) {
      case GO_FORWARD:
        next = go_forward(state, gl, gr);
        break;
      case TURN_LEFT:
        next = turn_left(state, gl, gr);
        break;
      default:
        next = go_forward(state, gl, gr);
        break;
    }

    if (next != state) {
      float now = time_sec();
      Serial.println(now - last_transition);
      last_transition = now;
      Serial.print(state); Serial.print("-->"); Serial.println(next);
      state = next;
    }


    /*
      Serial.print('(');
      Serial.print(gl);
      Serial.print(',');
      Serial.print(gr);
      Serial.println(')');
    */

    delay(100);
  }

  gyro.read();

  Serial.print(gyro.x);
  Serial.print(',');
  Serial.print(gyro.y);
  Serial.print(',');
  Serial.println(gyro.z);


}
