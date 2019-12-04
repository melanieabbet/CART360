/****************************************************************
  ColorSensor.ino
  APDS-9960 RGB and Gesture Sensor
  Shawn Hymel @ SparkFun Electronics
  October 15, 2014
  https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor

  Tests the color and ambient light sensing abilities of the
  APDS-9960. Configures APDS-9960 over I2C and polls the sensor for
  ambient light and color levels, which are displayed over the
  serial console.

  Hardware Connections:

  IMPORTANT: The APDS-9960 can only accept 3.3V!

  Arduino Pin  APDS-9960 Board  Function

  3.3V         VCC              Power
  GND          GND              Ground
  A4           SDA              I2C Data
  A5           SCL              I2C Clock

  Resources:
  Include Wire.h and SparkFun_APDS-9960.h

  Development environment specifics:
  Written in Arduino 1.0.5
  Tested with SparkFun Arduino Pro Mini 3.3V

  This code is beerware; if you see me (or any other SparkFun
  employee) at the local, and you've found our code helpful, please
  buy us a round!

  Distributed as-is; no warranty is given.
****************************************************************/

#include <Wire.h>
#include <SparkFun_APDS9960.h>


//MOZZI EXAMPLE:
#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <RollingAverage.h>
#include <ControlDelay.h>
unsigned int echo_cells_1 = 32;
unsigned int echo_cells_2 = 60;
unsigned int echo_cells_3 = 127;

#define CONTROL_RATE 64 // powers of 2 please
ControlDelay <128, int> kDelay; // 2seconds
// oscils to compare bumpy to averaged control input
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin0(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin1(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin2(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin3(SIN2048_DATA);
// use: RollingAverage <number_type, how_many_to_average> myThing
RollingAverage <int, 32> kAverage; // how_many_to_average has to be power of 2
int averaged;


// Global Variables SENSOR
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;
uint8_t proximity_data = 0;

//DEFINE LED AND SOUND PIN
#define LED_ONE 6
#define LED_TWO 7
#define LED_THREE 2
#define LED_FOUR 3
#define LED_FIVE 4
#define LED_SIX 5
#define BUZZER_PIN 9
boolean sound =false;

void setup() {

  pinMode(LED_ONE, OUTPUT);
  pinMode(LED_TWO, OUTPUT);
  pinMode(LED_THREE, OUTPUT);
  pinMode(LED_FOUR, OUTPUT);
  pinMode(LED_FIVE, OUTPUT);
  pinMode(LED_SIX, OUTPUT);


  //MOzzi
  kDelay.set(echo_cells_1);
  startMozzi();


  // Initialize Serial port
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun APDS-9960 - ColorSensor"));
  Serial.println(F("--------------------------------"));

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }

  // Start running the APDS-9960 light sensor (no interrupts)
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during light sensor init!"));
  }

  // Adjust the Proximity sensor gain
  if ( !apds.setProximityGain(PGAIN_2X) ) {
    Serial.println(F("Something went wrong trying to set PGAIN"));
  }

  // Start running the APDS-9960 proximity sensor (no interrupts)
  if ( apds.enableProximitySensor(false) ) {
    Serial.println(F("Proximity sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during sensor init!"));
  }
  // Wait for initialization and calibration to finish
  delay(500);
}




void updateControl() {

   //Read the proximity value
    if ( !apds.readProximity(proximity_data) ) {
      //Serial.println("Error reading proximity value");
    } else {
      //Serial.print("Proximity: ");
      //Serial.println(proximity_data);
    }


    if (proximity_data > 100) {
      sound =true;
      // Read the light levels (ambient, red, green, blue)
      if (  !apds.readAmbientLight(ambient_light) ||
            !apds.readRedLight(red_light) ||
            !apds.readGreenLight(green_light) ||
            !apds.readBlueLight(blue_light) ) {
       // Serial.println("Error reading light values");
      } else {
        Serial.print("Ambient: ");
        Serial.print(ambient_light);
        Serial.print(" Red: ");
        Serial.print(red_light);
        Serial.print(" Green: ");
        Serial.print(green_light);
        Serial.print(" Blue: ");
        Serial.println(blue_light);
        
        digitalWrite(LED_ONE, HIGH);
        digitalWrite(LED_TWO, HIGH);
        digitalWrite(LED_THREE, HIGH);
        digitalWrite(LED_FOUR, HIGH);
        digitalWrite(LED_FIVE, HIGH);
        digitalWrite(LED_SIX, HIGH);

        
  int bumpy_input = 0.1;
  averaged = kAverage.next(bumpy_input);
  aSin0.setFreq(averaged);
  aSin1.setFreq(kDelay.next(averaged));
  aSin2.setFreq(kDelay.read(echo_cells_2));
  aSin3.setFreq(kDelay.read(echo_cells_3));

      }

      // Wait before next reading
      delay(500);
    } else {
      sound =false;
      digitalWrite(LED_ONE, LOW);
      digitalWrite(LED_TWO, LOW);
      digitalWrite(LED_THREE, LOW);
      digitalWrite(LED_FOUR, LOW);
      digitalWrite(LED_FIVE, LOW);
      digitalWrite(LED_SIX, LOW);
    }
 

}



int updateAudio() {
  if (sound ==true){
  return 3*((int)aSin0.next()+aSin1.next()+(aSin2.next()>>1)
    +(aSin3.next()>>2)) >>3;
  } else if(sound==false){
   
    return 0;
  }
}


void loop() {
  audioHook();
}
