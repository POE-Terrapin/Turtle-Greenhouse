/*
 * Terrapin
 * Light vs Shadow Sensor Test
 * 
 * A test to see if the temperature sensor or the light sensor is better at detecting shade
 */

const int temperaturePin = A0, lightPin = A1; // Set up the pins for both sensors.
int inLightTemp, inLightLight; // These values will be read when the test is booted up. Make sure the sensors are in the type of light you want.
int inShadeTemp, inShadeLight; // These values will be read 10 seconds after the first values. Make sure to get it into some type of shade you don't want.
int tempDiff, lightDiff; // The difference between above values.

void setup() {
  Serial.begin(9600); // start Serial to see values
  Serial.println("Welcome to the light/shadow test to compare temperature and light sensors.");
  Serial.println("Please position both sensors in your desired type of light.");
  delay(5000);
  
  // Get the average value for these sensors in light.
  inLightTemp = getAverageValue(temperaturePin);
  inLightLight = getAverageValue(lightPin);
  Serial.print("Average value for temperature sensor in light: ");
  Serial.println(inLightTemp);
  Serial.print("Average value for light sensor in light: ");
  Serial.println(inLightLight);

  Serial.println("Please position both sensors in the shade you want to avoid.");
  Serial.println("You have 5 seconds until the reading.");
  Serial.println(5);
  delay(1000);
  Serial.println(4);
  delay(1000);
  Serial.println(3);
  delay(1000);
  Serial.println(2);
  delay(1000);
  Serial.println(1);
  delay(1000);
  inShadeTemp = getAverageValue(temperaturePin);
  inShadeLight = getAverageValue(lightPin);

  // Calculate the difference of these values.
  tempDiff = inLightTemp - inShadeTemp;
  lightDiff = inLightLight - inLightLight;

  Serial.print("Average value for temperature sensor in shade: ");
  Serial.println(inShadeTemp);
  Serial.print("Average value for light sensor in shade: ");
  Serial.println(inShadeLight);

  Serial.print("The difference in temperature between light and shade: ");
  Serial.println(tempDiff);
  Serial.print("The difference in light between light and shade: ");
  Serial.println(lightDiff);

  Serial.println("Starting the test.");
  delay(2000);
}

int tempVal, lightVal; // Variables to store the values in

void loop() {
  tempVal = analogRead(temperaturePin);
  lightVal = analogRead(lightPin);
  if (inLightTemp - tempVal > tempDiff) {
    Serial.print("Temperature: in shade.   ");
  }
  else {
    Serial.print("Tempearture: in light.   ");
  }
  if (inLightLight - lightVal > lightDiff) {
    Serial.println("Light: in shade.");
  }
  else {
    Serial.println("Light: in light.");
  }
}

int getAverageValue(int pin) {
  int totalValue = 0; // prepare to sum 10 reads
  for(int currentRead = 1; currentRead < 11; currentRead++) {
    totalValue += analogRead(pin); // add the newest read
    delay(250); // wait a quarter second before taking the next read
  }
  return totalValue/10; // return the average value
  
}

