/*  Example of simple FM with the phase modulation technique,
    using Mozzi sonification library.
  
    Demonstrates Oscil::phMod() for phase modulation, 
    Smooth() for smoothing control signals, 
    and Mozzi's fixed point number types for fractional frequencies.
  
    Also shows the limitations of Mozzi's 16384Hz Sample rate,
    as aliasing audibly intrudes as the sound gets brighter around 
    midi note 48.
    
    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.0/3.1, or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012, CC by-nc-sa.
    
*/

//#include <ADC.h>  // Teensy 3.0/3.1 uncomment this line and install http://github.com/pedvide/ADC
#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <mozzi_midi.h>
#include <mozzi_fixmath.h>
#include <EventDelay.h>
#include <Smooth.h>

#define CONTROL_RATE 256 // powers of 2 please

Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kModIndex(COS2048_DATA);

// The ratio of deviation to modulation frequency is called the "index of modulation". ( I = d / Fm )
// It will vary according to the frequency that is modulating the carrier and the amount of deviation.
// so deviation d = I   Fm
// haven't quite worked this out properly yet...

Q8n8 mod_index;// = float_to_Q8n8(2.0f); // constant version
Q16n16 deviation;

Q16n16 carrier_freq, mod_freq;

// FM ratio between oscillator frequencies, stays the same through note range
Q8n8 mod_to_carrier_ratio = float_to_Q8n8(3.f);

EventDelay kNoteChangeDelay;

// for note changes
Q7n8 target_note, note0, note1, note_upper_limit, note_lower_limit, note_change_step, smoothed_note;

// using Smooth on midi notes rather than frequency, 
// because fractional frequencies need larger types than Smooth can handle
// Inefficient, but...until there is a better Smooth....
Smooth <int> kSmoothNote(0.95f);




#include <Wire.h>
#include <SparkFun_APDS9960.h>


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

void setup(){
  kNoteChangeDelay.set(768); // ms countdown, taylored to resolution of CONTROL_RATE
  kModIndex.setFreq(.768f); // sync with kNoteChangeDelay
  target_note = note0;
  note_change_step = Q7n0_to_Q7n8(3);
  note_upper_limit = Q7n0_to_Q7n8(50);
  note_lower_limit = Q7n0_to_Q7n8(32);
  note0 = note_lower_limit;
  note1 = note_lower_limit + Q7n0_to_Q7n8(5);
  startMozzi(CONTROL_RATE);

  pinMode(LED_ONE, OUTPUT);
  pinMode(LED_TWO, OUTPUT);
  pinMode(LED_THREE, OUTPUT);
  pinMode(LED_FOUR, OUTPUT);
  pinMode(LED_FIVE, OUTPUT);
  pinMode(LED_SIX, OUTPUT);

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

void setFreqs(Q8n8 midi_note){
  carrier_freq = Q16n16_mtof(Q8n8_to_Q16n16(midi_note)); // convert midi note to fractional frequency
  mod_freq = ((carrier_freq>>8) * mod_to_carrier_ratio)  ; // (Q16n16>>8)   Q8n8 = Q16n16, beware of overflow
  deviation = ((mod_freq>>16) * mod_index); // (Q16n16>>16)   Q8n8 = Q24n8, beware of overflow
  aCarrier.setFreq_Q16n16(carrier_freq);
  aModulator.setFreq_Q16n16(mod_freq);
}

void updateControl(){
  // change note
  if(kNoteChangeDelay.ready()){

    //Read the proximity value
    if ( !apds.readProximity(proximity_data) ) {
      //Serial.println("Error reading proximity value");
    } else {
      //Serial.print("Proximity: ");
      //Serial.println(proximity_data);
    }
   if (proximity_data > 100) {


    sound =true;
    Serial.println("HIgher");
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



        if (target_note==note0){
      note1 += note_change_step;
      target_note= Q7n0_to_Q7n8(red_light/2);
    }
    else{ 
      note0 += note_change_step;
      target_note=Q7n0_to_Q7n8(blue_light/2);;
    }
    
      // Wait before next reading
      delay(500);
      }//fin detection action
   
   }else{
    Serial.println("Lower");
      sound =false;
      digitalWrite(LED_ONE, LOW);
      digitalWrite(LED_TWO, LOW);
      digitalWrite(LED_THREE, LOW);
      digitalWrite(LED_FOUR, LOW);
      digitalWrite(LED_FIVE, LOW);
      digitalWrite(LED_SIX, LOW);
    };

    // change direction
    if(note0>note_upper_limit) note_change_step = Q7n0_to_Q7n8(-3);
    if(note0<note_lower_limit) note_change_step = Q7n0_to_Q7n8(3);
    
    // reset eventdelay
    kNoteChangeDelay.start();
  }
  
  // vary the modulation index
  mod_index = (Q8n8)350+kModIndex.next();
  
  // here's where the smoothing happens
  smoothed_note = kSmoothNote.next(target_note);
  setFreqs(smoothed_note);

}


int updateAudio(){
  if (sound ==true){
  Q15n16 modulation = deviation * aModulator.next() >> 8;
  return (int)aCarrier.phMod(modulation);
  }else if(sound==false){
   
    return 0;
  }
}


void loop(){
  audioHook();
}
