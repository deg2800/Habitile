#include "Time.h"

String rawTime = "1600";
String formattedTime = "";
bool amPM = true;

void SetFormattedTime() {
    if (amPM) {
        String hourString = rawTime.substring(0,2);
        Serial.println("Hour String: " + hourString);
        String minuteString = rawTime.substring(2,4);
        Serial.println("Minute String: " + minuteString);
        bool setToPM = false;
        String AM = " AM";
        String PM = " PM";
        int hourInt = hourString.toInt();
        Serial.println("HourInt converted back into a string: " + String(hourInt));
        if (hourInt > 12) {
            hourInt = hourInt - 12;
            setToPM = true;
            Serial.println("Set to PM, new HourInt: " + String(hourInt));
        }
        if (hourInt == 0) {
            hourString = "12";
        } else {
            hourString = String(hourInt);
        }
        Serial.println("New hour string: " + hourString);
        formattedTime = hourString;
        formattedTime = formattedTime + ":";
        formattedTime = formattedTime + minuteString;
        if (setToPM) {
            formattedTime = formattedTime + PM;
        } else {
            formattedTime = formattedTime + AM;
        }
        Serial.println("Formatted Time: " + formattedTime);
    } else {
        String hourString = rawTime.substring(0,2);
        Serial.println("Hour String: " + hourString);
        String minuteString = rawTime.substring(2,4);
        Serial.println("Minute String: " + minuteString);
        int hourInt = hourString.toInt();
        Serial.println("HourInt converted back into a string: " + String(hourInt));
        hourString = String(hourInt);
        Serial.println("New hour string: " + hourString);
        formattedTime = hourString;
        formattedTime = formattedTime + ":";
        formattedTime = formattedTime + minuteString;
        Serial.println("Formatted Time: " + formattedTime);
    }
}

void SetRawTime(String rawTimeValue) {
    rawTime = rawTimeValue;
}

String GetFormattedTime() {
    SetFormattedTime();
    return formattedTime;
}

void SetAMPM(bool value) {
    SetBoolPref("amPM", value);
    amPM = value;
}

bool GetAMPM() {
    SetAMPMFromPrefValue();
    return amPM;
}

void SetAMPMFromPrefValue() {
    amPM = GetBoolPref("amPM", true);
}

void ToggleAMPM() {
    bool amPMValue = GetAMPM();
    amPMValue = !amPMValue;
    SetAMPM(amPMValue);
}