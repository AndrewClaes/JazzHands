/* JAZZHANDS Firmware tests
 *  
 */

#include <Arduino.h>

//PEDAL
#define INTERVAL 6   // time between reads
unsigned long lastRead = 0;
int pedalPin = 13;
int pedalValue = 0;
int pedalCC = 0;



void setup() {
  Serial.begin(115200);
  
 //PEDAL
  pinMode(pedalPin, INPUT);
  
 }


void loop() {
  
 
  //PEDAL
  if (millis() - lastRead >= INTERVAL){  // if INTERVAL has passed
      pedalValue = analogRead(pedalPin);
      lastRead = millis(); 
      pedalCC = map(pedalValue,0,4096,0,127);
      Serial.println(pedalCC);
      
  }
  
  
 
}

   
     
   
