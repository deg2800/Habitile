#include "DevicePrefs.h"
#include <Preferences.h>

Preferences preferences;

void Begin() {
    preferences.begin("DevicePrefs", false);
}

void End() {
    preferences.end();
}

void SetBoolPref(const char* name, bool value) {
    Begin();
    preferences.putBool(name, value);
    End();
}

bool GetBoolPref(const char* name, bool defaultBool) {
    Begin();
    bool value = preferences.getBool(name);
    End();
    return value;
}

void SetStringPref(const char* name, String value) {
    Begin();
    preferences.putString(name, value);
    Serial.println("Value is set to '" + value + "'");
    String newValue = preferences.getString(name);
    Serial.println("New value is set to '" + newValue + "'");
    End();
}

String GetStringPref(const char* name) {
    Begin();
    String value = preferences.getString(name);
    Serial.println("Get string pref value: " + value);
    End();
    return value;
}

void SetIntPref(const char* name, int32_t value) {
    Begin();
    preferences.putInt(name, value);
    End();
}

int32_t GetIntPref(const char* name, int32_t defaultValue) {
    Begin();
    int32_t value = preferences.getInt(name);
    End();
    return value;
}