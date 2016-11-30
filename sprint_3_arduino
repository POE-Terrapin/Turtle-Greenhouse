/*
   Terrapin Project
   PoE Fall 2016
   Zarin Bhuiyan, Jamie Cho, Kaitlyn Keil, Lauren Pudvan, Katya Soltan
   Sketch Model Code
   Sprint 3
   Started 11/15
*/

// INSERT LEG CONTROLLER THINGS HERE

// Constant values and pins.

// Set up the pins for the different sensors
//  LS stands for Light Sensor. Numbers increase
//  clockwise from the front: LS0 is front, LS1 is
//  at 3 o'clock (from top view), LS2 is in back, etc.
const int LS0 = 0, LS2 = 1;// LS1 = A1, LS3 = A3;

// Set up multiplexer pins
const int s[3] = {2, 3, 4};
const int multi_pin = A0; // pin to read from/output pin

// Eye1 and Eye2 power LEDs.
const int Eye1 = 8, Eye2 = 9;

// IR sensor pins on the multiplexer
//  f_ir indicates 'front IR', r_ir is 'right IR', l_ir is 'left IR'
const int f_ir = 4, r_ir = 3, l_ir=5;

// Temperature and moisture sensors
const int ts = 6, ms = 7;

// Create the variables to store sensor values. Each
//  corresponds with the matching name.
int LS0val = 0, LS1val = 0, LS2val = 0, LS3val = 0;
int f_value = 0, r_value = 0, l_value = 0;

// Create a variable to keep track of what direction the turtle is moving (straight=0, right turn=1, left turn = 2)
int directions = 0;

int norm_f_value, norm_r_value, norm_l_value; // Prepare values to normalize IR sensors

void setup() {
  Serial.begin(9600); // start Serial for RPi

  // ATTACH LEG STUFF HERE

  // Set up multiplexer pins
  pinMode(s[0], OUTPUT);
  pinMode(s[1], OUTPUT);
  pinMode(s[2], OUTPUT);

  // Turn on both of the LEDs for the eyes.
  pinMode(Eye1, OUTPUT);
  digitalWrite(Eye1, HIGH);
  pinMode(Eye2, OUTPUT);
  digitalWrite(Eye2, HIGH);
  digitalWrite(Eye1, LOW);
  digitalWrite(Eye2, LOW);
  // Start up turtle without anything around the head so that initial values are
  //  useable for normalizing
  // Set up global values to normalize the sensors
  norm_f_value = readMultiplexer(f_ir);
  norm_r_value = readMultiplexer(r_ir);
  norm_l_value = readMultiplexer(l_ir);
  delay(2000); // 2 second delay
}

void loop() {
  // Read the light sensors
  readSensors();
  directions = checkChoice();
  moveTurtle(directions);
}

void readSensors() {
  // Reads the sensors and writes them to Serial
  LS0val = readMultiplexer(LS0);
  LS2val = readMultiplexer(LS2);
  
  f_value = readMultiplexer(f_ir);
  r_value = readMultiplexer(r_ir);
  l_value = readMultiplexer(l_ir);
  int f_normalized = f_value - norm_f_value;
  int r_normalized = r_value - norm_r_value;
  int l_normalized = l_value - norm_l_value;
  
  int ts_value = readMultiplexer(ts);
  int ms_value = readMultiplexer(ms);

  serialWrite(LS0val);
  serialWrite(LS2val);
  serialWrite(f_normalized);
  serialWrite(r_normalized);
  serialWrite(l_normalized);
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
    return Serial.read()
  }
  else {
    return directions
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
  // Not yet implemented
}

void turnLeft() {
  // Not yet implemented
}

void turnRight() {
  // Not yet implemented
}

void stopMoving() {
  // Not yet implemented
}

// Returns the value of the requested pin from the multiplexer
int readMultiplexer(int pin_num) {
  digitalWrite(s[0], (pin_num & 1) ? HIGH : LOW);
  digitalWrite(s[1], (pin_num & 2) ? HIGH : LOW);
  digitalWrite(s[2], (pin_num & 4) ? HIGH : LOW);
  return analogRead(multi_pin);
}
