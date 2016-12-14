#ifndef __SENSORS_H__
#define __SENSORS_H__
#include "utils.h"

class Sensors{
  
};

// Set up multiplexer pins
const int s[3] = {2, 3, 4};

// IR sensor pins on the multiplexer
//  f_ir indicates 'front IR', r_ir is 'right IR', l_ir is 'left IR'
const int f_ir = 2, r_ir = 3, l_ir = 4;

// Temperature and moisture sensors
const int ts = 5, ms = 6;

// Create the variables to store sensor values. Each
//  corresponds with the matching name.
int LS0val = 0, LS1val = 0, LS2val = 0, LS3val = 0;
int f_value = 0, r_value = 0, l_value = 0;
int ts_value = 0, ms_value = 0;

const int multi_pin = A0; // pin to read from/output pin

int readMultiplexer(int pin_num) {
  digitalWrite(s[0], (pin_num & 1) ? HIGH : LOW);
  digitalWrite(s[1], (pin_num & 2) ? HIGH : LOW);
  digitalWrite(s[2], (pin_num & 4) ? HIGH : LOW);
  return analogRead(multi_pin);
}
void setup_sensors(){
  // Set up multiplexer pins
  pinMode(s[0], OUTPUT);
  pinMode(s[1], OUTPUT);
  pinMode(s[2], OUTPUT);
}

// Set up the pins for the different sensors
//  LS stands for Light Sensor. Numbers increase
//  clockwise from the front: LS0 is front, LS1 is
//  at 3 o'clock (from top view), LS2 is in back, etc.
const int LS0 = 0, LS2 = 1;// LS1 = A1, LS3 = A3;

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
}

#endif

