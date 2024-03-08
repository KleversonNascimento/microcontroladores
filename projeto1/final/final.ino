#include <ezButton.h>

#include "SevSeg.h"
const SevSeg sevseg;

#include  <Chrono.h>
const Chrono chrono;
const Chrono chronoLastButtonPress;

ezButton BUTTON_INCREASE_TIME(2); 
ezButton BUTTON_DECREASE_TIME(3);
ezButton BUTTON_START(4);
ezButton BUTTON_FREEZE_COUNT(5);

int LAST_STATE_BUTTON_INCREASE_TIME = 1;
int LAST_STATE_BUTTON_DECREASE_TIME = 1;
int LAST_STATE_BUTTON_START = 1;
int LAST_STATE_BUTTON_FREEZE_COUNT = 1;

enum : byte { COUNTING_TIME, FROZEN, AWAITING_TIME_SELECTION, TIME_FINISHED } internalState = AWAITING_TIME_SELECTION;

int selectedTime = 0;
int remainingTime = 0;

void configureButtons() {
  BUTTON_INCREASE_TIME.setDebounceTime(150);
  BUTTON_INCREASE_TIME.setCountMode(COUNT_FALLING);

  BUTTON_START.setDebounceTime(150);
  BUTTON_START.setCountMode(COUNT_FALLING);
  
  BUTTON_DECREASE_TIME.setDebounceTime(150);
  BUTTON_DECREASE_TIME.setCountMode(COUNT_FALLING);
  
  BUTTON_FREEZE_COUNT.setDebounceTime(150);
  BUTTON_FREEZE_COUNT.setCountMode(COUNT_FALLING);
  

  Serial.println("Configured buttons");
}

void configureDisplay() {
  const byte DISPLAY_NUM_DIGITS = 3;
  const byte DISPLAY_DIGIT_PINS[] = {47, 33, 39};
  const byte DISPLAY_SEGMENT_PINS[] = {43, 35, 27, 49, 45, 41, 25, 29};
  const bool DISPLAY_RESISTOR_ON_SEGMENTS = true;
  const byte DISPLAY_HARDWARE_CONFIG = N_TRANSISTORS;
  const bool DISPLAY_UPDATE_WITH_DELAYS = true; // Default 'false' is Recommended
  const bool DISPLAY_LEADING_ZEROS = false;
  const bool DISPLAY_DISABLED_DECIMAL_POINT = false;
  
  sevseg.begin(DISPLAY_HARDWARE_CONFIG, DISPLAY_NUM_DIGITS, DISPLAY_DIGIT_PINS, DISPLAY_SEGMENT_PINS, DISPLAY_RESISTOR_ON_SEGMENTS,
  DISPLAY_UPDATE_WITH_DELAYS, DISPLAY_LEADING_ZEROS, DISPLAY_DISABLED_DECIMAL_POINT);
  sevseg.setBrightness(100);

  Serial.println("Configured display");
}

void increaseTimeTest() {
  Serial.println("Increase time called");
  if (internalState == AWAITING_TIME_SELECTION) {
    if (selectedTime < 995) {
      selectedTime = selectedTime + 5;
    }
  }
}

void decreaseTimeTest() {
  Serial.println("|Decrease time called");
  if (internalState == AWAITING_TIME_SELECTION) {
    if (selectedTime > 0) {
      selectedTime = selectedTime - 5;
    }
  }
}

void startTime() {
  Serial.println("Start time called");
  if (internalState == AWAITING_TIME_SELECTION) {
    startCount();
  }
}

void frozeTimeTest() {
  Serial.println("Frozen time called");
  if (internalState == COUNTING_TIME) {
    chrono.stop();
    internalState = FROZEN;
  } else if (internalState == FROZEN) {
    chrono.resume();
    internalState = COUNTING_TIME;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Start setup");
  configureButtons();
  configureDisplay();
  chrono.stop();
}

void startCount() {
  internalState = COUNTING_TIME;
  remainingTime = selectedTime;
  chrono.restart();
}

void timeSelectionState() {
  if (BUTTON_INCREASE_TIME.isPressed()) {
    increaseTimeTest();
  }

  if(BUTTON_DECREASE_TIME.isPressed()) {
    decreaseTimeTest();
  }

  if(BUTTON_START.isPressed()) {
    startTime();
  }

  sevseg.setNumber(selectedTime);
}

void updateRemainingTime() {
  remainingTime = selectedTime - (chrono.elapsed() / 1000);
  sevseg.setNumber(remainingTime);
}

void timeFinished() {
  internalState = TIME_FINISHED;
  selectedTime = 0;
  remainingTime = 0;
  chrono.restart();
}

void countingTimeState() {
  updateRemainingTime();

  if (remainingTime < 0) {
    timeFinished();
  }

  if (BUTTON_FREEZE_COUNT.isPressed()) {
    frozeTimeTest();
  }
}

void frozenTimeState() {
  if(BUTTON_FREEZE_COUNT.isPressed()) {
    frozeTimeTest();
  }
}

void finishedTimeState() {
  if (chrono.hasPassed(5000)) {
    internalState = AWAITING_TIME_SELECTION;
    chrono.restart();
    chrono.stop();
  } else {
    sevseg.setNumber(999);
  }
}

void loop() {
  const byte initialState = internalState;
  BUTTON_INCREASE_TIME.loop();
  BUTTON_START.loop();
  BUTTON_DECREASE_TIME.loop();
  BUTTON_FREEZE_COUNT.loop();

  switch(internalState) {
    case AWAITING_TIME_SELECTION:
      timeSelectionState();
      break;
    case COUNTING_TIME:
      countingTimeState();
      break;
    case FROZEN:
      frozenTimeState();
      break;
    case TIME_FINISHED:
      finishedTimeState();
      break;
  }

  if (initialState != internalState) {
    Serial.print("Internal state update from ");
    Serial.print(initialState);
    Serial.print(" to ");
    Serial.print(internalState);
  }

  sevseg.refreshDisplay();
}