#include <Adafruit_MotorShield.h>
#include "gyro.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor* motor_l = AFMS.getMotor(4);
Adafruit_DCMotor* motor_r = AFMS.getMotor(3);

const int GROUND_L_PIN = 5;// 0 when ON, 1 when off
const int GROUND_R_PIN = 6; // 0 when ON, 1 when off
const int ON_OFF_PIN = 8; // 0 when ON, 1 when OFF

enum {LEFT_MOVE, LEFT_UP, RIGHT_MOVE, RIGHT_UP};
int state;
float last_transition;

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
        motor_l->run(FORWARD);
        motor_r->run(FORWARD);
        if (gr) {

          motor_l->setSpeed(100);
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

    Serial.print('(');
    Serial.print(gl);
    Serial.print(',');
    Serial.print(gr);
    Serial.println(')');

    delay(100);
  }

  gyro.read();

  Serial.print(gyro.x);
  Serial.print(',');
  Serial.print(gyro.y);
  Serial.print(',');
  Serial.println(gyro.z);


}
