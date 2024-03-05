/* SevSeg Counter Example
 
 Copyright 2020 Dean Reading
 
 This example demonstrates a very simple use of the SevSeg library with a 4
 digit display. It displays a counter that counts up, showing deci-seconds.
 */

#include "SevSeg.h"
SevSeg sevseg; //Instantiate a seven segment controller object

void setup() {
  byte numDigits = 3;
  byte digitPins[] = {47, 33, 39};
  byte segmentPins[] = {43, 35, 27, 49, 45, 41, 25, 29};
  bool resistorsOnSegments = true; // 'false' means resistors are on digit pins
  byte hardwareConfig = N_TRANSISTORS; // See README.md for options
  bool updateWithDelays = true; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(100);
  Serial.begin(9600);
}

void loop() {
  static unsigned long timer = millis();
  static int deciSeconds = 0;
  
  if (millis() - timer >= 1000) {
    timer += 1000;
    deciSeconds++; // 100 milliSeconds is equal to 1 deciSecond
    
    if (deciSeconds == 100000) { // Reset to 0 after counting for 1000 seconds.
      deciSeconds=0;
    }
    Serial.println (deciSeconds);
    sevseg.setNumber(25, 0);
  }

  sevseg.refreshDisplay(); // Must run repeatedly
}

/// END ///
