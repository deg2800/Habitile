#include "BTHandler.h"

#define SERVICE1_UUID "180A" // Device Information
#define S1C1_UUID "2A29" // Manufacturer Name String
#define S1C2_UUID "2A24" // Model Number String
#define S1C3_UUID "2A26" // Firmware Revision String
#define S1C4_UUID "2A27" // Hardware Revision String
#define S1C1_STRING "Derrick Goodfriend"
#define S1C2_STRING "Prototype A1"
#define S1C3_STRING "230812.0220"
#define S1C4_STRING "230730"

#define SERVICE2_UUID "181C" // User Data
#define S2C1_UUID "2B4E" // Activity Goal
#define S2C2_UUID "2A08" // Date Time
#define S2C3_UUID "2AE2" // Boolean (completed)

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914c"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define NAME "Habitile Prototype A1"

String S2C1_STRING = "Take out trash";
String S2C2_STRING = "4:00 PM";
BLECharacteristic *pS2C3 = NULL;

class CharacteristicCallBack: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    std::string pChar2_value_stdstr = pChar -> getValue();
    S2C1_STRING = String(pChar2_value_stdstr.c_str());
    Serial.println(S2C1_STRING);

    /*
    if (completed) {
      Completed();
    } else {
      EntryView();
    }
    */
  }
};

class CompletedCallBack: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    std::string completedTx = pChar -> getValue();
    int completedInt = std::stoi(completedTx);
    Serial.println(completedInt);

    /*
    if (completedInt == 1) {
      completed = true;
    } else {
      completed = false;
    }

    if (completed) {
      Completed();
    } else {
      EntryView();
    }
    */
  }
};

void setupBT() {
  Serial.println("Starting BLE work!");

  BLEDevice::init(NAME);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService1 = pServer->createService(SERVICE1_UUID);
  BLECharacteristic *pS1C1 = pService1->createCharacteristic(
                                         S1C1_UUID,
                                         BLECharacteristic::PROPERTY_READ
                                       );

  pS1C1->setValue(S1C1_STRING);
  BLECharacteristic *pS1C2 = pService1->createCharacteristic(
                                         S1C2_UUID,
                                         BLECharacteristic::PROPERTY_READ
                                       );

  pS1C2->setValue(S1C2_STRING);
  BLECharacteristic *pS1C3 = pService1->createCharacteristic(
                                         S1C3_UUID,
                                         BLECharacteristic::PROPERTY_READ
                                       );

  pS1C3->setValue(S1C3_STRING);
  BLECharacteristic *pS1C4 = pService1->createCharacteristic(
                                         S1C4_UUID,
                                         BLECharacteristic::PROPERTY_READ
                                       );

  pS1C4->setValue(S1C4_STRING);
  pService1->start();

  BLEService *pService2 = pServer->createService(SERVICE2_UUID);
  BLECharacteristic *pS2C1 = pService2->createCharacteristic(
                                         S2C1_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  const char* S2C1_Value = S2C1_STRING.c_str();
  pS2C1->setValue(S2C1_Value);
  BLECharacteristic *pS2C2 = pService2->createCharacteristic(
                                         S2C2_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  const char* S2C2_Value = S2C2_STRING.c_str();
  pS2C2->setValue(S2C2_Value);
  pS2C3 = pService2->createCharacteristic(
                                         S2C3_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  std::string completedInt = "0";
  /*
  if (completed) {
    completedInt = "1";
  }
  */
  pS2C3->setValue(completedInt);
  pS2C1->setCallbacks(new CharacteristicCallBack());
  pS2C3->setCallbacks(new CompletedCallBack());
// Future callbacks for two new characteristics

  pService2->start();

  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE1_UUID);
  pAdvertising->addServiceUUID(SERVICE2_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void BT_Advert() {
  BLEDevice::startAdvertising();
}

String S2C1_STRING_GET() {
  return S2C1_STRING;
}

String S2C2_STRING_GET() {
  return S2C2_STRING;
}

void Set_pS2C3_ValueFromBool (bool value) {
  if (value) {
    pS2C3->setValue("1");
    //completed = true;
  } else {
    pS2C3->setValue("0");
    //completed = false;
  }
}

void Set_pS2C3_ValueFromInt (int value) {
  if (value == 1) {
  pS2C3->setValue("1");
  //completed = true;
  } else if (value == 0) {
    pS2C3->setValue("0");
    //completed = false;
  }
}