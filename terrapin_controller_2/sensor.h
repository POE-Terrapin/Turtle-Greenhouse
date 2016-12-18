#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <Arduino.h>

enum {
  ANALOG, DIGITAL
};

//generic multi-sensor interface
struct Sensor {
  const int n;
  char type;
  const char* name;
  const int* pins;
  int* vals;
  
  Sensor(const char* _name, const int* _pins, int _n, char _type, bool pullup=false):
    name(_name), pins(_pins), n(_n), type(_type) {
    for (int i = 0; i < n; ++i) {
      pinMode(pins[i], pullup?INPUT_PULLUP:INPUT);
    }
    // allocate vals
    vals = (int*)malloc(sizeof(int) * n);
  }
  
  ~Sensor() {
    if (vals) {
      free(vals);
      vals = nullptr;
    }
  }
  void read() {
    for (int i = 0; i < n; ++i) {
      vals[i] = (type == ANALOG) ? analogRead(pins[i]) : digitalRead(pins[i]);
    }
  }
  const int* get() {
    return vals;
  }
  const int get(int i){
    return vals[i];
  }
  void print() {
    Serial.print(name);
    Serial.print(" : ");
  }
};


struct Multiplexer {
  const int n_pins; //selector bit width
  const int* sel_pins;
  const int val_pin;

  Multiplexer(const int n_pins, int val_pin, const int* sel_pins):
    n_pins(n_pins), val_pin(val_pin), sel_pins(sel_pins) {
    pinMode(val_pin, INPUT);
    for (int i = 0; i < n_pins; ++i) {
      pinMode(sel_pins[i], OUTPUT);
    }
  }
  
  int read(const int pin_num) {
    digitalWrite(sel_pins[0], (pin_num & 1) ? HIGH : LOW);
    digitalWrite(sel_pins[1], (pin_num & 2) ? HIGH : LOW);
    digitalWrite(sel_pins[2], (pin_num & 4) ? HIGH : LOW);
    return analogRead(val_pin);
  }
};


#endif
