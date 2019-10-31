#include <Keyboard.h> // library to simulate keyboard events
int sensorPinHeat = A0 ;  // define sensor port (analog A0)
int valueHeat = 0;    //set value to 0
int thresholdHeat = 370; // threshold for data range

int rawX;
int rawY;

float scaledX, scaledY;

void setup()
{
  Keyboard.begin();
  Serial.begin(9600);
}

void loop()
{
  valueHeat = analogRead(sensorPinHeat);
  //Serial.println(valueHeat);
  if(valueHeat > thresholdHeat){
    Serial.println("X");
    Keyboard.press('x');
  }
  else {
    Keyboard.release('x');
  }

  // Get raw accelerometer data for each axis
  rawX = analogRead(A1);
  rawY = analogRead(A2);

  scaledX = map(rawX, 230, 430, -1, 2);
  scaledY = map(rawY, 230, 430, -1, 2);

  if(scaledY > 0){
    Serial.println("UP");
    Keyboard.press(217);
    Keyboard.release(218);
    Keyboard.release(215);
    Keyboard.release(216);
  }
  else if(scaledY < 0) {
    Serial.println("Down");
    Keyboard.press(218);
    Keyboard.release(217);
    Keyboard.release(215);
    Keyboard.release(216);
  }else if(scaledX > 0){
    Serial.println("Left");
    Keyboard.press(216);
    Keyboard.release(215);
    Keyboard.release(217);
    Keyboard.release(218);
  } else if(scaledX < 0) {
    Serial.println("Right");
    Keyboard.press(215);
    Keyboard.release(216);
    Keyboard.release(217);
    Keyboard.release(218);
  }else{
    Serial.println("RELEASE");
    Keyboard.release(215);
    Keyboard.release(216);
    Keyboard.release(217);
    Keyboard.release(218);
  }
  
  delay(10);  //delay 0.01S
}
