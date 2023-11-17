/* JAZZHANDS Firmware v0.2
 *  
 */

#include <Arduino.h>
#include <BLEMidi.h>
#include <MIDI.h>

struct Serial2MIDISettings : public midi::DefaultSettings
{
  static const long BaudRate = 31250;
  static const int8_t RxPin  = 16;
  static const int8_t TxPin  = 17;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial2, DIN_MIDI, Serial2MIDISettings);

int myResult[24]; // Array with digital code of the sensorvalues

int noteSounding; // Current note
int getNote; // Is there a new note?

int programChange = 0;


//Note ON from PEDAL
int envValue;
int state;
unsigned long breath_on_time = 0L;
int initial_breath_value;
int atVal;
unsigned long atSendTime = 0L;
//PEDAL sound detector
#define NOTE_ON_THRESHOLD 80
#define MAX_PRESSURE 1000
#define NOTE_OFF 1
#define RISE_WAIT 2
#define NOTE_ON 3
#define AT_INTERVAL 70
#define RISE_TIME 2

// Octave sensor
int octaveKey = 23; 
int octave2Pin = 14;
int octave3Pin = 12;

int octaveValue = 0;



//Shift Register
const int dataPin = 22;   /* Shift Register Q7 */
const int clockPin = 19;  /* Shift Register CP */
const int latchPin = 21;  /* Shift RegisterPL */
const int numBits = 24;   /* Set to 8 * number of shift registers */

//PEDAL
#define INTERVAL 6   // time between reads
unsigned long lastRead = 0;
int pedalPin = 13;
int pedalValue = 0;
int pedalCC = 0;



void setup() {
  //Serial.begin(115200);
  //AD attenuation
  analogSetAttenuation(ADC_0db);
  //shift register
  pinMode(dataPin, INPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  // BLE
  //Serial.println("Initializing bluetooth");
  BLEMidiServer.begin("HybridSax");
  //Serial.println("Waiting for connections...");

 //DIN_MIDI
  DIN_MIDI.begin(MIDI_CHANNEL_OMNI);
  
 //PEDAL
  pinMode(pedalPin, INPUT);
  

 //OCTAVES
 pinMode(octaveKey, INPUT_PULLUP);
 pinMode(octave2Pin, INPUT);
 pinMode(octave3Pin, INPUT);

 state = NOTE_OFF;
}
 
 int get_velocity(int initial, int final, unsigned long time_delta) {
     return map(final, NOTE_ON_THRESHOLD, MAX_PRESSURE,0,127);
     } 
 
 


 


void loop() {
  
  // Octaves
    // Check if octave hall switch is touched
  if (digitalRead(octaveKey) == HIGH) {
    octaveValue = 12;
  //  Serial.println(octaveValue);
  }
  if (digitalRead(octaveKey) == LOW) {
    octaveValue = 0;
    }
 
  // Check if octave2 sensor is touched
  if (touchRead(octave2Pin) < 30 && digitalRead(octaveKey) == LOW) {
      octaveValue = -12;
      }
  // Check if octave2 and octave3 sensors are touched
  if (touchRead(octave2Pin) < 30 && touchRead(octave3Pin) < 30) {
      octaveValue = -24;
      }
  // Check if octave3 sensor is touched
  else if (touchRead(octave3Pin) < 30 && digitalRead(octaveKey) == LOW) {
      octaveValue = -36;
      }
 
 
  //PEDAL
  if (millis() - lastRead >= INTERVAL){  // if INTERVAL has passed
      pedalValue = analogRead(pedalPin);
      lastRead = millis(); 
      pedalCC = map(pedalValue,0,4096,0,127);
      //Serial.println(pedalCC);
      DIN_MIDI.sendControlChange(2, pedalCC, 1);
      if(BLEMidiServer.isConnected()) {
         BLEMidiServer.controlChange(0, 2, pedalCC);
         }
  }
  
  
  
  // Shift Register Step 1: Sample
  digitalWrite(latchPin, LOW);
  digitalWrite(latchPin, HIGH);

  // Shift Register Step 2: Shift
  for (int i = 0; i < numBits; i++) {
    int bit = digitalRead(dataPin);
    if (bit == HIGH) {
      myResult[i] = 0;
      
    } else {
      myResult[i] = 1;
    }
    digitalWrite(clockPin, HIGH); // Shift out the next bit
    digitalWrite(clockPin, LOW);
  }
   // Construct a 24 bit value from the sensors
   uint32_t value = (myResult[0] << 23) +
                      (myResult[1] << 22) +
                      (myResult[2] << 21) +
                      (myResult[3] << 20) +
                      (myResult[4] << 19) +
                      (myResult[5] << 18) +
                      (myResult[6] << 17) +
                      (myResult[7] << 16) +
                      (myResult[8] << 15) +
                      (myResult[9] << 14) +
                      (myResult[10] << 13) +
                      (myResult[11] << 12) +
                      (myResult[12] << 11) +
                      (myResult[13] << 10) +
                      (myResult[14] << 9) +
                       (myResult[15] << 8) + 
                       (myResult[16] << 7) +
                       (myResult[17] << 6) +
                       (myResult[18] << 5) +
                       (myResult[19] << 4) +
                       (myResult[20] << 3) +
                       (myResult[21] << 2) +
                       (myResult[22] << 1) +
                       myResult[23];

// DEBUG: print all sensor states                    
//  Serial.println(value);
//   for (int i = 0;i < 24;i++){
//    Serial.print(myResult[i]);
//  }
//  Serial.println(); 
  

  // Define the notes from the sensors
  #define BASE 73 + octaveValue
  switch (value) {
    case 492984: //C
      getNote = BASE + 11;
      break;

    case 501176: //B
      getNote = BASE + 10;
      break;

    case 505272: //Bb
       getNote = BASE + 9;
       break;

    case 505784: //A
      getNote = BASE + 8;
      break;

    case 261448: //G#
      getNote = BASE + 7;
      break;

    case 440648: //G
      getNote = BASE + 6;
      break;

    case 458568: //F#
       getNote = BASE + 5;
       break;

    case 196936: //F
      getNote = BASE + 4;
      break;

    case 229704: //E
      getNote = BASE + 3;
      break;

    case 491848: //D#
      getNote = BASE + 2;
      break;

    case 500040: //D
       getNote = BASE + 1;
       break;

    case 504136: //C# BASE
       getNote = BASE;
       break;

    case 505672: //C
      getNote = BASE - 1;
      break;
      
    case 506184: //B
      getNote = BASE - 2;
      break;
    
    case 507208: //Bb
      getNote = BASE - 3;
      break;

    case 442184: //Bb alternate
      getNote = BASE - 3;
      break;
    
    case 507720: //A
      getNote = BASE - 4;
      break;

    case 507592: //G#
      getNote = BASE - 5;
      break;
    
    case 507848: //G
      getNote = BASE - 6;
      break;
    
    case 507880: //F#
      getNote = BASE - 7;
      break;
    
    case 524232: //F
      getNote = BASE - 8;
      break;
    
    case 524264: //E
      getNote = BASE - 9;
      break;

    case 524216: //Eb
      getNote = BASE - 10;
      break;

    case 524280: //D
      getNote = BASE - 11;
      break;

    case 524276: //C#
       getNote = BASE - 12;
       break;

    case 524284: //C
      getNote = BASE - 13;
      break;

    case 524286: //B
      getNote = BASE - 14;
      break;

    case 524287: //Bb
      getNote = BASE - 15;
      break;

    case 373064: // Program Change UP
      programChange = programChange + 1;
      DIN_MIDI.sendProgramChange(programChange, 1);
      delay(500);
      break;

    case 438600: // Program Change DOWN
      programChange = programChange - 1;
      if(programChange < 0){
        programChange = 0;
      }
      DIN_MIDI.sendProgramChange(programChange, 1);
      delay(500);
      break;
      
   
    default:
      getNote = noteSounding;
  }

  
  //  send MIDI
   envValue = pedalValue;
   if (state == NOTE_OFF) {
       if (envValue > NOTE_ON_THRESHOLD) {
       // Value has risen above threshold. Move to the RISE_TIME
       // state. Record time and initial breath value.
          breath_on_time = millis();
          initial_breath_value = envValue;
          state = RISE_WAIT;  // Go to next state
          }
  } else if (state == RISE_WAIT) {
    if (envValue > NOTE_ON_THRESHOLD) {
      // Has enough time passed for us to collect our second
      // sample?
      if (millis() - breath_on_time > RISE_TIME) {
        // Yes, so calculate MIDI note and velocity, then send a note on event
        noteSounding = getNote;
        int velocity = get_velocity(initial_breath_value, envValue, RISE_TIME);
        DIN_MIDI.sendNoteOn(noteSounding,velocity*5,1);
        if(BLEMidiServer.isConnected()) {               
          BLEMidiServer.noteOn(0, noteSounding, velocity); 
          }
        state = NOTE_ON;
      }
    } else {
      // Value fell below threshold before RISE_TIME passed. Return to
      // NOTE_OFF state (e.g. we're ignoring a short blip of breath)
      state = NOTE_OFF;
    }
  } else if (state == NOTE_ON) {
    if (envValue < NOTE_ON_THRESHOLD) {
      // Value has fallen below threshold - turn the note off
      DIN_MIDI.sendNoteOff(noteSounding,0,1);  
      if(BLEMidiServer.isConnected()) {
           BLEMidiServer.noteOff(0,noteSounding, 0);
           }
       
      state = NOTE_OFF;
    } else {
      // Is it time to send more aftertouch data?
      if (millis() - atSendTime > AT_INTERVAL) {
        // Map the sensor value to the aftertouch range 0-127
        atVal = map(envValue, NOTE_ON_THRESHOLD, 4096, 0, 127);
        DIN_MIDI.sendControlChange(2, atVal, 1);
        if(BLEMidiServer.isConnected()) {
         BLEMidiServer.controlChange(0, 2, atVal);
         }          
        atSendTime = millis();
      }
    }
  
  if (millis() - lastRead >= INTERVAL){  // if INTERVAL has passed
      lastRead = millis(); 
      int newNote = getNote;
      if (newNote != noteSounding){
          //Serial.println("noteoff:");
          //Serial.println(noteSounding);
          DIN_MIDI.sendNoteOff(noteSounding,0,1); 
          if(BLEMidiServer.isConnected()) {
             BLEMidiServer.noteOff(0,noteSounding, 127);
             }
          noteSounding = newNote;
          //Serial.println("noteon:");
          //Serial.println(noteSounding);
          DIN_MIDI.sendNoteOn(noteSounding, 127,1);
          if(BLEMidiServer.isConnected()) {               
             BLEMidiServer.noteOn(0, noteSounding, 127); 
             }
          
           
  }
 }
 }
} 



   
     
   
