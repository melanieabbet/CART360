/********************** ETUDE 3 CART 360 2019 *******************************
 * WELCOME! 
 * THE PURPOSE OF THIS EXERCISE IS TO DESIGN A VERY SIMPLE KEYBOARD (5 KEYS)
 * WHICH ALLOWS YOU TO PLAY LIVE, RECORD, AND PLAYBACK SINGLE NOTES (NO CHORDS). 
 * THERE WILL BE A BUTTON WHICH WHEN PRESSED WILL TAKE THE USER TO THE NEXT MODE:
 * THERE ARE 5 POSSIBLE MODES
 * 0 ==  reset    ==   led off == also resets
 * 1 ==  live     ==   led BLUE
 * 2 ==  record   ==   led RED
 * 3 ==  play     ==   led GREEN
 * 4 ==  loopMODE ==   led PURPLE
 * 
 * ADDITIONALLY - WHEN THE USER CHANGES MODE, 
 * THE RGB LED WILL CHANGE COLOR (ACCORDING TO THE SPECIFICATIONS)

 * PLEASE FOLLOW THE INSTRUCTIONS IN THE COMMENTS:
 * DO NOT ADD ANY MORE FUNCTION DEFINITIONS 
 * ONLY IMPLEMENT THE FUNCTION DEFINITIONS SUPPLIED
 * THERE IS NO NEED TO ADD ANY NEW VARIABLES OR CONSTANTS
 * PROVIDE COMMENTS FOR ANY OF THE CODE IMPLEMENTED
 * GOOD LUCK!
 */
/**** CONSTANTS ********************************************************/

#define BUTTON_MODE_PIN 2 // Button to change the mode

// constants for RGB LED //change the colors pin because wasn't the good pin connected to
#define LED_PIN_R 9 // R PIN
#define LED_PIN_G 6 // G PIN
#define LED_PIN_B 5 // B PIN

// constant for note in (button-resistor ladder on breadboard)
# define NOTE_IN_PIN A0

//constant for max number of notes in array
#define MAX_NOTES 16

// a constant for duration
const int duration = 400;

// constant for pin to output for buzzer 
#define BUZZER_PIN 3 // PWM

/******** VARIABLES *****************************************************/
// counter for how many notes we have
int countNotes =0;
// array to hold the notes played (for record/play mode)
int notes [MAX_NOTES]; 
int mode =0; // start at off

/*************************************************************************/


/**********************SETUP() DO NOT CHANGE*******************************/
// Declare pin mode for the single digital input
void setup()
{
  pinMode(BUTTON_MODE_PIN, INPUT);
  Serial.begin(9600);
}

/**********************LOOP() DO NOT CHANGE *******************************
 * INSTRUCTIONS: 
 * There is NO NEED to change the loop - it establishes the flow of the program
 * We call here 3 functions repeatedly:
 * 1: chooseMode(): this function will determine the mode that your program is in 
 * based on if the button (linked to the BUTTON_MODE_PIN) was pressed
 * 2: setRGB(): will set the color of the RGB LED based on the value of the mode variable
 * 3: selectMode(): will determine which function to call based on the value of the mode variable

**************************************************************************/
void loop()
{
  chooseMode();
  setRGB();
  selectMode();
}
/******************CHOOSEMODE(): IMPLEMENT *********************************
 * INSTRUCTIONS:
 * Read the value from the Button (linked to the BUTTON_MODE_PIN) and 
 * if is being pressed then change the mode variable.
 * REMEMBER:
 * mode == 0 is reset 
 * mode == 1 is live
 * mode == 2 is record
 * mode == 3 is play
 * mode == 4 is loopMode
 * Every time the user presses the button, the program will go to the next mode,
 * once it reaches 4, it should go back to mode == 0. 
 * (i.e. if mode ==2 and we press, then mode ==3) ...
**************************************************************************/
void chooseMode(){
  // IMPLEMENT
  //if button pressed - value should be high
  if (digitalRead(BUTTON_MODE_PIN) == HIGH){
    
    //for mode = 4 return mode =0
    if (mode == 4){
      mode = 0;
      //to avoid error (there is a cont of 200 for delay realize afterwards
      //delay(200);
      
    }
    //for mode smaller than 4, add 1 to mode
    else{
      //+ should be bevore the = sign
      mode += 1;
      //to avoid error 
      //delay(200);
      
    }

    delay(duration);
    Serial.println(mode);
  }
}

/******************SETRGB(): IMPLEMENT *********************************
 * INSTRUCTIONS:
 * Depending on the value of the mode variable:
 * if the mode is 0 - RGB LED IS OFF
 * if the mode is 1 - RGB LED IS BLUE
 * if mode is 2 - RGB LED IS RED
 * if mode is 3 - RGB LED IS GREEN
 * if mode is 4 - RGB LED iS PURPLE
 * YOU MUST USE A SWITCH CASE CONSTRUCT (NOT A SERIES OF IF / ELSE STATEMENTS
**************************************************************************/
//create the different color fonction -> turning on/off the appropriate color of the led
//HIGH (=1) LOW -> just for digital / here need a range between 0-255
void colorOff(){
  analogWrite(LED_PIN_R, 0);
  analogWrite(LED_PIN_G, 0);
  analogWrite(LED_PIN_B, 0);
}
void colorBlue(){
  analogWrite(LED_PIN_R, 0);
  analogWrite(LED_PIN_G, 0);
  analogWrite(LED_PIN_B, 255);
}
void colorRed(){
  analogWrite(LED_PIN_R, 255);
  analogWrite(LED_PIN_G, 0);
  analogWrite(LED_PIN_B, 0);
}
void colorGreen(){
  analogWrite(LED_PIN_R, 0);
  analogWrite(LED_PIN_G, 255);
  analogWrite(LED_PIN_B, 255);
}
void colorPurple(){
  //put 100 to get a better purple
  analogWrite(LED_PIN_R, 100);
  analogWrite(LED_PIN_G, 0);
  analogWrite(LED_PIN_B, 255);
}

// give the color fonction which suit each case
void setRGB()
{
  switch (mode) {
    case 0: colorOff();
    break;
    case 1: colorBlue();
     break;
    case 2: colorRed();
     break;
    case 3: colorGreen();
     break;
    case 4: colorPurple();
  }
}
/**********************SELECTMODE() DO NOT CHANGE *******************************
 * INSTRUCTIONS: 
 * There is NO NEED to change this function - it selects WHICH function should run based on the mode variable
 * There are 4 possibilities
 * 1: reset(): this function will reset any arrays etc, and will do nothing else
 * 2: live(): this function will play the corresponding notes 
 * to the user pressing the respective buttons. 
 * NOTE:: the notes played are NOT stored
 * 3: record(): this function will play the corresponding notes 
 * to the user pressing the respective buttons
 * AND will STORE up to 16 consecutive notes in an array.
 * 4: play(): this function will playback any notes stored in the array that were recorded
 * in the previous mode
 * 5: loopMode(): this function will playback any notes stored in the array that were recorded, 
 * BUT unlike the previous mode, you can choose in which sequence the notes are played.
 * REQUIRED: only play notes from the array (no live stuff)

******************************************************************************/
void selectMode()
{
  if(mode ==0) { 
    reset();
  }
  else if(mode ==1) {
    live();
  }
  else if(mode ==2) {
    record();
  }
  
  else if(mode ==3) {
    play();
  }
   
   else if(mode ==4) {
    loopMode();
  }
}
/******************RESET(): IMPLEMENT **************************************
 * INSTRUCTIONS:
 * this function should ensure that any notes recorded are no longer available
**************************************************************************/
void reset()
{
  // IMPLEMENT
 //reet the sound to be sure it make no sound anymore
noTone(BUZZER_PIN);

//take all the value of the array and set it to 0
for ( int i=0; i < MAX_NOTES; i ++){
  notes[i] = 0;
}

//reset the note count
countNotes = 0;
  
}
/******************LIVE(): IMPLEMENT **************************************
 * INSTRUCTIONS:
 * this function will play the corresponding notes 
 * to the user pressing the respective buttons
 * NOTE:: the notes played are NOT stored
 * SO: you need read in the input from the analog input (linked to the button-resistor ladder combo)
 * THEN - output the note to the buzzer using the tone() function
**************************************************************************/
void live()
{
    //IMPLEMENT
    //documentation: tone(pin, frequency)   / tone(pin, frequency, duration) Diiférence?
    
    //take the buzzer pin signal, read teh analog signal and take the number as a Note, play the sound
    if (analogRead(NOTE_IN_PIN) > 15){
    tone(BUZZER_PIN, analogRead(NOTE_IN_PIN));
    delay(duration);
    //debugging
    //Serial.println(analogRead(NOTE_IN_PIN));
    }
    
    //make the sound stop if the button are not activated
    if (analogRead(NOTE_IN_PIN) < 15){
      noTone(3);
    }
    
}
/******************RECORD(): IMPLEMENT **********************************
 * INSTRUCTIONS:
 * this function will play the corresponding notes 
 * to the user pressing the respective buttons
 * AND will STORE up to 16 consecutive notes in an array.
 * SO:you need read in the input from the analog input (linked to the button-resistor ladder combo)
 * AND - output the note to the buzzer using the tone() function
 * THEN store that note in the array  - BE CAREFUL - you can only allow for up to MAX_NOTES to be stored
**************************************************************************/
void record()
{
  // IMPLEMENT
  
  //take the value on the note in pin
  int val = analogRead(NOTE_IN_PIN);
  Serial.println(val);
  
  if (val > 15){
     tone(BUZZER_PIN, val);
     if (countNotes < MAX_NOTES){
      notes[countNotes] = val;
      countNotes += 1;
     } 
     delay(duration); 
  }
  
  if (val < 15){
    noTone(BUZZER_PIN);
  }
  //check what is in the array
//  for(int i = 0; i < 16; i++)
//{
//  Serial.println(notes[i]);
//}
//  
  
}
/******************PLAY(): IMPLEMENT ************************************
 * INSTRUCTIONS:
 * this function will playback any notes stored in the array that were recorded
 * in the previous mode
 * SO: you need to go through the array of values (be careful - the user may not have put in MAX_NOTES)
 * READ each value IN ORDER
 * AND output each note to the buzzer using the tone() function
 * ALSO: as long as we are in this mode, the notes are played over and over again
 * BE CAREFUL: make sure you allow for the user to get to another mode from the mode button...
**************************************************************************/
void play()
{
  // IMPLEMENT
Go trought the number of note we have and play them in loop
  for (int i =0; i < countNotes; i++){
   tone(BUZZER_PIN, notes[i]);
   delay(duration);
   // try to debug the button to get better input
   if (digitalRead(BUTTON_MODE_PIN) == HIGH){
    noTone(BUZZER_PIN);
    delay(duration);
    break;
  }
  }
  
  
}
/******************LOOPMODE(): IMPLEMENT *********************************
 * INSTRUCTIONS:
 * this function will playback any notes stored in the array that were recorded
 * in the previous mode
 * SO: you need to go through the array of values (be careful - the user may not have put in MAX_NOTES)
 * READ values IN ANY ORDERING (You MUST use the array - but you can determine your own sequence)
 * AND output each note to the buzzer using the tone() function
 * ALSO: as long as we are in this mode, the notes are played over and over again
 * BE CAREFUL: make sure you allow for the user to get to another mode from the mode button...
**************************************************************************/
void loopMode()
{
  //IMPLEMENT
  //create a random value that take into account the number of value we have(countNotes)
  int randomNotes = random(0, countNotes);
  //play tone randomly
  tone(BUZZER_PIN, notes[randomNotes]);
  delay(duration);
  
  
}

/******************************Comment on the Etude********************************************
I join the RGB wire differently than in the schematics where there miss a connection between the two part of the board
Then I first get some problem with the board, I realize I forgot to add connection to the A0 pin

The first things I programm was the colors I get struggle because I put the value as HIGH and LOW as we are in a situation (analog where a value between 0-255 was needed)
Second the Led was not showing the right color so I change the number of the pin connected so that my mode correspond to the right color

I tried different resistor, the best match Ifound was a mix of 10KOhm and 300Ohm with the 10K resistor the sound were really deep
and similar. Putting a higher resistance at the start change just the first sound to something deep and at the end all the button before the resistance
With the 100K Ohm the sound just could not get trought.
I get some struggle with the button in mode 3 and 4, I resolute it to a certain extend with the delay but it is not perfect.
/**************************************************************************/
