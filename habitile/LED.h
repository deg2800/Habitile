#ifndef LED_h
#define LED_h

#include "Arduino.h"
enum LEDColor {red,yellow,green,blue};
enum LEDState {on,off};

void LEDPinModeSetup();
void SwitchLED(LEDColor color, LEDState state);
void SwitchAllLED(LEDState state);

#endif