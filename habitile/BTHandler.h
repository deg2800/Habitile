#ifndef BTHandler_h
#define BTHandler_h

#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// Class and function declarations

void setupBT();
void BT_Advert();
String S2C1_STRING_GET();
String S2C2_STRING_GET();
void Set_pS2C3_ValueFromBool(bool value);
void Set_pS2C3_ValueFromInt(int value);

#endif

extern BLECharacteristic *pS1C1;
extern BLECharacteristic *pS1C2;
extern BLECharacteristic *pS1C3;
extern BLECharacteristic *pS1C4;
extern BLECharacteristic *pS2C1;
extern BLECharacteristic *pS2C2;
extern BLECharacteristic *pS2C3;