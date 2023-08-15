#include "LED.h"

const int blueLEDPin = A6;
const int greenLEDPin = 2;
const int yellowLEDPin = A4;
const int redLEDPin = A2;

void LEDPinModeSetup() {
    pinMode(blueLEDPin, OUTPUT);
    pinMode(greenLEDPin, OUTPUT);
    pinMode(yellowLEDPin, OUTPUT);
    pinMode(redLEDPin, OUTPUT);
}

void SwitchLED(LEDColor color, LEDState state) {
  switch(color) {
    case blue:
      switch(state) {
        case on:
          digitalWrite(blueLEDPin, HIGH);
          break;
        case off:
          digitalWrite(blueLEDPin, LOW);
          break;
        default:
          break;
      }
      break;
    case green:
      switch(state) {
        case on:
          digitalWrite(greenLEDPin, HIGH);
          break;
        case off:
          digitalWrite(greenLEDPin, LOW);
          break;
        default:
          break;
      }
      break;
    case yellow:
      switch(state) {
        case on:
          digitalWrite(yellowLEDPin, HIGH);
          break;
        case off:
          digitalWrite(yellowLEDPin, LOW);
          break;
        default:
          break;
      }
      break;
    case red:
      switch(state) {
        case on:
          digitalWrite(redLEDPin, HIGH);
          break;
        case off:
          digitalWrite(redLEDPin, LOW);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void SwitchAllLED(LEDState state) {
  SwitchLED(blue, state);
  SwitchLED(green, state);
  SwitchLED(yellow, state);
  SwitchLED(red, state);
}