//calibrated parameters
#ifndef __GYRO_H__
#define __GYRO_H__

#include "sensor.h"

const int X_MIN = 406;
const int X_MAX = 611;
const int Y_MIN = 397;
const int Y_MAX = 603;
const int Z_MIN = 415;
const int Z_MAX = 621;
const int sampleSize = 10;

class Gyro {
    Sensor sensor;
  public:
    float x, y, z; // gyro acceleration
    Gyro(const int* pins):
    sensor("gyro", pins, 3, ANALOG) {
      
    }
    void read() {
      const int* vals = sensor.get();
      x=y=z=0;
      for(int i=0; i<sampleSize; ++i){
        sensor.read();
        x += vals[0];
        y += vals[1];
        z += vals[2];
      }
      x /= sampleSize;
      y /= sampleSize;
      z /= sampleSize;
      
      x = map(x, X_MIN, X_MAX, -980, 980) / 1000.0;
      y = map(y, Y_MIN, Y_MAX, -980, 980) / 1000.0;
      z = map(z, Z_MIN, Z_MAX, -980, 980) / 1000.0;
    }
};

#endif
