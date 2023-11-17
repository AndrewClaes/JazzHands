/* JAZZHANDS Touch Sensors Test
 *  
 */

#include <Arduino.h>
// Octave sensor
int octaveHall = 23;
int octave2Pin = 12;
int octave3Pin = 14;


void setup() {
  Serial.begin(115200);
 

 //OCTAVES
 pinMode(octaveHall, INPUT_PULLUP);
 pinMode(octave2Pin, INPUT);
 pinMode(octave3Pin, INPUT);

 }



void loop() {
  if (digitalRead(octaveHall) == HIGH) {
      Serial.println("octave Hall is off");
      }
  if (digitalRead(octaveHall) == LOW) {
      Serial.println("octave Hall is on");
      }
  
  if (touchRead(octave2Pin) < 30) {
      Serial.println("touchsensor1 is working");
      }
  else if (touchRead(octave3Pin) < 30) {
      Serial.println("touchsensor2 is working");
      }
 
}

   
     
   
