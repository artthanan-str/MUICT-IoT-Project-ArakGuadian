#include "dht.h" // Library for DHT11
#include <Stepper.h> // Library for Stepper motor

#define STEPS 32
Stepper stepper(STEPS, 8, 10, 9, 11);
int val = 200;

// setup DHT11
#define DHTPin A0 // use A0 as a data input
dht dht; 
float humidValue = 0;

// variables for LM35
float tempValue = 0;
int tempPin = A2;

// varibles for Flame Sensor
int flamePin = A1;
int flameValue = 0;
int flameThreshold = 1400; // flame level

// varible for LED
int LEDPin_RED = 4;
int LEDPin_GREEN = 5;
int LEDPin_BLUE = 6;

// varible for interrupt button
int interruptPin = 2;
volatile byte interruptButtonState = LOW;

// variable for ky-012 Buzzer
int buzzerPin = 3;

// general variable
bool isFired = true;

void setup() {
  pinMode(LEDPin_RED, OUTPUT);
  pinMode(LEDPin_GREEN, OUTPUT);
  pinMode(LEDPin_BLUE, OUTPUT);
  
  pinMode(buzzerPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(interruptPin), callInterrupt, CHANGE);

  stepper.setSpeed(300);
  
  Serial.begin(9600);
}

void loop() {
  dht.read11(DHTPin);
  // read value from sensors
  // tempValue =((analogRead(tempPin) / 1024.0)*5.0) * 100.0;
  flameValue = analogRead(flamePin);
  humidValue = dht.humidity; // read humidity value from DHT11
  tempValue = dht.temperature;

  if (tempValue > 30 || humidValue < 40) {
      stepper.step(val);
  }
  
  if (flameValue >= flameThreshold) {
    interruptButtonState = HIGH;
    isFired = true;
    setColor(255, 0, 0); // Red Color
    stepper.step(val);
  }
  else {
    interruptButtonState = LOW;
    isFired = false;
    setColor(0, 255, 0); // Green Color
  }
  
  digitalWrite(buzzerPin, interruptButtonState);
  printOutPut(humidValue, tempValue, flameValue); // Call print command
  delay(1000);
 
}

void callInterrupt(){
  if (isFired) {
    // Serial.println("Threshold = 1500");
    flameThreshold = 1500;
    stepper.setSpeed(300);
  }
  else {
    // Serial.println("Threshold = 0");
    flameThreshold = 0;
    stepper.setSpeed(800);
  }
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(LEDPin_RED, redValue);
  analogWrite(LEDPin_GREEN, greenValue);
  analogWrite(LEDPin_BLUE, blueValue);
}

void printOutPut(float humid_value, float temp_value, int flame_value){
  String dataString = "";
  dataString += humid_value;
  dataString += " ";
  dataString += temp_value;
  dataString += " ";
  dataString += flame_value;
  dataString += " ";
  if (isFired) {
    dataString += "1";
  }
  else {
    dataString += "0";
  }
  Serial.println(dataString);
}
