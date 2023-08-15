#ifndef DevicePrefs_h
#define DevicePrefs_h

#include "Arduino.h"
#include <Preferences.h>

void Begin();
void End();
void SetBoolPref(const char* name, bool value);
bool GetBoolPref(const char* name, bool defaultBool);
void SetStringPref(const char* name, String value);
String GetStringPref(const char* name);
void SetIntPref(const char* name, int32_t value);
int32_t GetIntPref(const char* name, int32_t defaultValue);

#endif

extern Preferences preferences;