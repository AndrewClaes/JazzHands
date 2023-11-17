/* JAZZHANDS Firmware tests
 *  
 */

#include <Arduino.h>

int myResult[24]; // Array with digital code of the sensorvalues

//Shift Register
const int dataPin = 22;   /* Shift Register Q7 */
const int clockPin = 19;  /* Shift Register CP */
const int latchPin = 21;  /* Shift RegisterPL */
const int numBits = 24;   /* Set to 8 * number of shift registers */


void setup() {
  Serial.begin(115200);
  //shift register
  pinMode(dataPin, INPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
}

void loop() {
  
\ 
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
 // Serial.println(value);
   for (int i = 0;i < 24;i++){
    Serial.print(myResult[i]);
  }
  Serial.println(); 
  

}

   
     
   
