#ifndef Time_h
#define Time_h

#include "Arduino.h"
#include "DevicePrefs.h"

// Function declarations
void SetFormattedTime();
void SetRawTime(String rawTimeValue);
String GetFormattedTime();
void SetAMPM(bool value);
bool GetAMPM();
void SetAMPMFromPrefValue();
void ToggleAMPM();
#endif