#ifndef __SENSORS_H__
#define __SENSORS_H__

#include "sensor.h"
#include "gyro.h"
#include "utils.h"

// Temperature and moisture sensors
const int ts = 5, ms = 6;

// Create the variables to store sensor values. Each
//  corresponds with the matching name.
int LS0val = 0, LS1val = 0, LS2val = 0, LS3val = 0;
int f_value = 0, r_value = 0, l_value = 0;
int ts_value = 0, ms_value = 0;

enum {LS0, LS2, f_ir, l_ir, r_ir}; //2,3,4 for ir, 0,1 for ls

// MUX
const int mux_sel[] = {2, 3, 4};
Multiplexer mux(3, A0, mux_sel);

// GROUND CONTACT
const int contact_pins[] = {5, 6}; //left,right
Sensor contact("contact", contact_pins, 2, DIGITAL, true); // pullup

// ON SWITCH
const int on_pin[] = {8};
Sensor on_sw("on", on_pin, 1, DIGITAL, true); //pullup

// GYRO
const int gyro_pins[] = {A1, A2, A3};
Gyro gyro(gyro_pins);

// Set up the pins for the different sensors
//  LS stands for Light Sensor. Numbers increase
//  clockwise from the front: LS0 is front, LS1 is
//  at 3 o'clock (from top view), LS2 is in back, etc.

// left-right contact touch sensor

void readSensors() {
  // Reads the sensors and writes them to Serial
  LS0val = mux.read(LS0);
  LS2val = mux.read(LS2);

  f_value = mux.read(f_ir);
  r_value = mux.read(r_ir);
  l_value = mux.read(l_ir);

  ts_value = mux.read(ts);
  ms_value = mux.read(ms);

  contact.read();
  on_sw.read();
  gyro.read();
}


bool ground(char s) {
  switch (s) {
    case 'l':
      return !contact.get(0);
      break;
    case 'r':
      return !contact.get(1);
      break;
  }
  return false;
}

bool on(){
  // low when on
  // high when off
  return !on_sw.get(0);
}
#endif

