//calibrated parameters

const int X_MIN = 406;
const int X_MAX = 611;
const int Y_MIN = 397;
const int Y_MAX = 603;
const int Z_MIN = 415;
const int Z_MAX = 621;
const int sampleSize = 10;

int readAxis(const int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading / sampleSize;
}

class Gyro {
    const int x_pin, y_pin, z_pin;
  public:
    float x, y, z; // gyro acceleration
    Gyro(const int x_pin, const int y_pin, const int z_pin): x_pin(x_pin), y_pin(y_pin), z_pin(z_pin) {
      
    }
    void setup(){
      // just in case
      pinMode(x_pin, INPUT);
      pinMode(y_pin, INPUT);
      pinMode(z_pin, INPUT);
      
    }
    void read() {
      x = readAxis(x_pin);
      y = readAxis(y_pin);
      z = readAxis(z_pin);
      
      x = map(x, X_MIN, X_MAX, -980, 980) / 1000.0;
      y = map(y, Y_MIN, Y_MAX, -980, 980) / 1000.0;
      z = map(z, Z_MIN, Z_MAX, -980, 980) / 1000.0;

    }

};
