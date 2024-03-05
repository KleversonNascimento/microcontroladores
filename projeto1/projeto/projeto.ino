#include "SevSeg.h"
const SevSeg sevseg;

#include  <Chrono.h>
const Chrono chrono;

const int BUTTON_INCREASE_TIME = 2; 
const int BUTTON_DECREASE_TIME = 3;
const int BUTTON_START = 5;
const int BUTTON_FREEZE_COUNT = 4;

int LAST_STATE_BUTTON_INCREASE_TIME = 1;
int LAST_STATE_BUTTON_DECREASE_TIME = 1;
int LAST_STATE_BUTTON_START = 1;
int LAST_STATE_BUTTON_FREEZE_COUNT = 1;

enum : byte { COUNTING_TIME, FROZEN, AWAITING_TIME_SELECTION, TIME_FINISHED } internalState = AWAITING_TIME_SELECTION;

int selectedTime = 0;
int remainingTime = 0;

void configureButtons() {
  pinMode(BUTTON_INCREASE_TIME, INPUT_PULLUP);
  pinMode(BUTTON_DECREASE_TIME, INPUT_PULLUP);
  pinMode(BUTTON_START, INPUT_PULLUP);
  pinMode(BUTTON_FREEZE_COUNT, INPUT_PULLUP); 

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

void saveAllButtonsState() {
  LAST_STATE_BUTTON_INCREASE_TIME = digitalRead(BUTTON_INCREASE_TIME);
  LAST_STATE_BUTTON_DECREASE_TIME = digitalRead(BUTTON_DECREASE_TIME);
  LAST_STATE_BUTTON_START = digitalRead(BUTTON_START);
  LAST_STATE_BUTTON_FREEZE_COUNT = digitalRead(BUTTON_FREEZE_COUNT);
}

bool buttonWasPressed(int button, int lastStateButton) {
  if (digitalRead(button) == 1 && lastStateButton == 0) {
    return true;
  }

  return false;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Start setup");
  configureButtons();
  chrono.stop();
}

void startCount() {
  internalState = COUNTING_TIME;
  remainingTime = selectedTime;
  chrono.restart();
}

void timeSelectionState() {
  if (buttonWasPressed(BUTTON_INCREASE_TIME, LAST_STATE_BUTTON_INCREASE_TIME)) {
    if (selectedTime < 995) {
      selectedTime = selectedTime + 5;
    }
  }

  if (buttonWasPressed(BUTTON_DECREASE_TIME, LAST_STATE_BUTTON_DECREASE_TIME)) {
    if (selectedTime > 0) {
      selectedTime = selectedTime - 5;
    }
  }

  if (buttonWasPressed(BUTTON_START, LAST_STATE_BUTTON_START)) {
    startCount();
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

  if(buttonWasPressed(BUTTON_FREEZE_COUNT, LAST_STATE_BUTTON_FREEZE_COUNT)) {
    chrono.stop();
    internalState = FROZEN;
  } 

  if (remainingTime < 0) {
    timeFinished();
  }
}

void frozenTimeState() {
  if(buttonWasPressed(BUTTON_FREEZE_COUNT, LAST_STATE_BUTTON_FREEZE_COUNT)) {
    chrono.resume();
    internalState = COUNTING_TIME;
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
  saveAllButtonsState();
}
