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
#include <tables/COS2048_int8.h> // table for Oscils to play
//include <mozzi_utils.h>
#include <mozzi_fixmath.h>
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#define CONTROL_RATE 64 // powers of 2 please
// audio oscils
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModDepth(COS2048_DATA);
// for scheduling note changes in updateControl()
EventDelay  kNoteChangeDelay;
// synthesis parameters in fixed point formats
Q8n8 ratio; // unsigned int with 8 integer bits and 8 fractional bits
Q24n8 carrier_freq; // unsigned long with 24 integer bits and 8 fractional bits
Q24n8 mod_freq; // unsigned long with 24 integer bits and 8 fractional bits
// for random notes//here variable that would change
Q8n0 octave_start_note = 42;

int sensorPin = A0; 
byte gain =250;
boolean sound =false;




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

void setup() {

  pinMode(LED_ONE, OUTPUT);
  pinMode(LED_TWO, OUTPUT);
  pinMode(LED_THREE, OUTPUT);
  pinMode(LED_FOUR, OUTPUT);
  pinMode(LED_FIVE, OUTPUT);
  pinMode(LED_SIX, OUTPUT);


  //MOzzi
  ratio = float_to_Q8n8(3.0f);   // define modulation ratio in float and convert to fixed-point
  kNoteChangeDelay.set(200); // note duration ms, within resolution of CONTROL_RATE
  aModDepth.setFreq(30.f);     // vary mod depth to highlight am effects
  randSeed(); // reseed the random generator for different results each time the sketch runs
  startMozzi(CONTROL_RATE);



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
  static Q16n16 last_note = octave_start_note;

  if (kNoteChangeDelay.ready()) {

    //gain = analogRead(sensorPin);
    //Serial.println(gain);

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
        
        
        
        octave_start_note = red_light;
        digitalWrite(LED_ONE, HIGH);
        digitalWrite(LED_TWO, HIGH);
        digitalWrite(LED_THREE, HIGH);
        digitalWrite(LED_FOUR, HIGH);
        digitalWrite(LED_FIVE, HIGH);
        digitalWrite(LED_SIX, HIGH);
       
       
    if (rand((byte)5) == 0) {
      last_note = red_light ;
    }

    // change step up or down a semitone occasionally
    if (rand((byte)13) == 0) {
      last_note = green_light;
    }

    // change modulation ratio now and then
    if (rand((byte)5) == 0) {
      ratio = ((Q8n8) 1 + rand((byte)5)) << 8;
    }

    // sometimes add a fractionto the ratio
    if (rand((byte)5) == 0) {
      ratio += rand((byte)255);
    }

    // step up or down 3 semitones (or 0)
    last_note = blue_light;

    // convert midi to frequency
    Q16n16 midi_note = Q8n0_to_Q16n16(last_note);
    carrier_freq = Q16n16_to_Q24n8(Q16n16_mtof(midi_note));

    // calculate modulation frequency to stay in ratio with carrier
    mod_freq = (carrier_freq * ratio) >> 8; // (Q24n8   Q8n8) >> 8 = Q24n8

    // set frequencies of the oscillators
    aCarrier.setFreq_Q24n8(carrier_freq);
    aModulator.setFreq_Q24n8(mod_freq);

    // reset the note scheduler
    kNoteChangeDelay.start();

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

}

void playsound() {

  digitalWrite(LED_ONE, HIGH);
  digitalWrite(LED_TWO, HIGH);
  digitalWrite(LED_THREE, HIGH);
  digitalWrite(LED_FOUR, HIGH);
  digitalWrite(LED_FIVE, HIGH);
  digitalWrite(LED_SIX, HIGH);
  noTone(BUZZER_PIN);
  tone(BUZZER_PIN, red_light);
  delay(100);
}

int updateAudio() {
  if (sound ==true){
    long mod = (128u + aModulator.next()) * ((byte)128 + aModDepth.next());
  int out = (aCarrier.next()*mod) >> 16;
  return out;
  }else if(sound==false){
   
    return 0;
  }
  
}


void loop() {
  audioHook();
}
