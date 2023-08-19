#include "BTHandler.h"
#include "DevicePrefs.h"

#define SERVICE1_UUID "180A" // Device Information
#define S1C1_UUID "2A29" // Manufacturer Name String
#define S1C2_UUID "2A24" // Model Number String
#define S1C3_UUID "2A26" // Firmware Revision String
#define S1C4_UUID "2A27" // Hardware Revision String
#define S1C1_STRING "Derrick Goodfriend"
#define S1C2_STRING "Prototype A1"
#define S1C3_STRING "230816.1840"
#define S1C4_STRING "230814"

#define SERVICE2_UUID "181C" // User Data
#define S2C1_UUID "2B4E" // Activity Goal
#define S2C2_UUID "2A14" // Reference Time Information
#define S2C3_UUID "2AE2" // Boolean (completed)

#define SERVICE3_UUID "084cb3d8-51f6-468a-8231-12617222e87f" // Settings & Remote Control Flags
#define S3C1_UUID "cbe8c508-5e87-45b6-b32d-e78aaf13e1b1" // Toggle amPM
#define S3C2_UUID "41b8a051-d831-480e-bf64-bedc42c5d3ef" // Toggle sound
#define S3C3_UUID "63218fa9-acd8-4f7f-a975-de8dacbaff6c" // Trigger device reset
#define S3C4_UUID "9dba5820-36dc-4cac-bd7d-d4ea92473fdb" // Trigger factory reset
#define S3C5_UUID "db5dd014-1e7d-40d4-9dff-9d9746c915a3" // Remote Press Button 1
#define S3C6_UUID "c45c008d-d29d-4635-a9da-4b56e7107a1c" // Remote Press Button 2
#define S3C7_UUID "2a347faa-e1ab-408f-a06f-1094e54ba507" // Remote Press Button 3
#define S3C8_UUID "11665d83-4761-4c4a-96f8-ee4ce6faf8fd" // Remote Press Button 4

#define NAME "Habitile Prototype A1"

String S2C1_STRING = "Take out trash";
String S2C2_STRING = "1600";
BLECharacteristic *pS2C1 = NULL;
BLECharacteristic *pS2C2 = NULL; 
BLECharacteristic *pS2C3 = NULL;

BLECharacteristic *pS3C1 = NULL;
BLECharacteristic *pS3C2 = NULL; 
BLECharacteristic *pS3C3 = NULL;

BLECharacteristic *pS3C5 = NULL;
BLECharacteristic *pS3C6 = NULL; 
BLECharacteristic *pS3C7 = NULL;
BLECharacteristic *pS3C8 = NULL;

bool completed = false;
bool taskUpdate = false;
bool remoteButton1Press = false;
bool remoteButton2Press = false;
bool remoteButton3Press = false;
bool remoteButton4Press = false;

class S2C1_CallBack: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    std::string pChar2_value_stdstr = pChar -> getValue();
    S2C1_STRING_SET(String(pChar2_value_stdstr.c_str()));
    Serial.println(S2C1_STRING);

    taskUpdate = true;
  }
};

class S2C2_CallBack: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    std::string pChar2_value_stdstr = pChar -> getValue();
    S2C2_STRING_SET(String(pChar2_value_stdstr.c_str()));
    Serial.println(S2C2_STRING);

    taskUpdate = true;
  }
};

class CompletedCallBack: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    std::string completedTx = pChar -> getValue();
    int completedInt = std::stoi(completedTx);
    Serial.println(completedInt);

    if (completedInt == 1) {
      completed = true;
    } else {
      completed = false;
    }

    taskUpdate = true;
    
  }
};

class RemoteButton1CallBack: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    std::string completedTx = pChar -> getValue();
    int completedInt = std::stoi(completedTx);

    if (completedInt == 1) {
      remoteButton1Press = true;
    }
    pS3C5->setValue("0");
  }
};

class RemoteButton2CallBack: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    std::string completedTx = pChar -> getValue();
    int completedInt = std::stoi(completedTx);

    if (completedInt == 1) {
      remoteButton2Press = true;
    } 
    pS3C6->setValue("0");
  }
};

class RemoteButton3CallBack: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    std::string completedTx = pChar -> getValue();
    int completedInt = std::stoi(completedTx);

    if (completedInt == 1) {
      remoteButton3Press = true;
    } 
    pS3C7->setValue("0");
  }
};

class RemoteButton4CallBack: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    std::string completedTx = pChar -> getValue();
    int completedInt = std::stoi(completedTx);

    if (completedInt == 1) {
      remoteButton4Press = true;
    }
    pS3C8->setValue("0");
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
  pS2C1 = pService2->createCharacteristic(
                                         S2C1_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  String taskTitleFromPref = GetStringPref("taskTitle");
  if (taskTitleFromPref == "") {
    taskTitleFromPref = S2C1_STRING;
  }
  S2C1_STRING_SET(taskTitleFromPref);
  pS2C2 = pService2->createCharacteristic(
                                         S2C2_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  String taskTimeFromPref = GetStringPref("taskTime");
  if (taskTimeFromPref == "") {
    taskTimeFromPref = S2C2_STRING;
  }
  S2C2_STRING_SET(taskTimeFromPref);
  pS2C3 = pService2->createCharacteristic(
                                         S2C3_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  GetCompletedBoolFromPrefs();
  std::string completedInt = "0";
  if (completed) {
    completedInt = "1";
  }
  pS2C3->setValue(completedInt);
  pS2C1->setCallbacks(new S2C1_CallBack());
  pS2C2->setCallbacks(new S2C2_CallBack());
  pS2C3->setCallbacks(new CompletedCallBack());
// Future callbacks for two new characteristics

  pService2->start();

  BLEService *pService3 = pServer->createService(SERVICE3_UUID);

  pS3C5 = pService3->createCharacteristic(
                                         S3C5_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pS3C5->setValue("0");

  pS3C6 = pService3->createCharacteristic(
                                         S3C6_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pS3C6->setValue("0");

  pS3C7 = pService3->createCharacteristic(
                                         S3C7_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pS3C7->setValue("0");

  pS3C8 = pService3->createCharacteristic(
                                         S3C8_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pS3C8->setValue("0");

  pS3C5->setCallbacks(new RemoteButton1CallBack());
  pS3C6->setCallbacks(new RemoteButton2CallBack());
  pS3C7->setCallbacks(new RemoteButton3CallBack());
  pS3C8->setCallbacks(new RemoteButton4CallBack());

  pService3->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE1_UUID);
  pAdvertising->addServiceUUID(SERVICE2_UUID);
  pAdvertising->addServiceUUID(SERVICE3_UUID);
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
  S2C1_STRING = GetStringPref("taskTitle");
  return S2C1_STRING;
}

String S2C2_STRING_GET() {
  S2C2_STRING = GetStringPref("taskTime");
  SetAMPMFromPrefValue();
  String formattedTime = GetFormattedTime();
  return formattedTime;
}

void S2C1_STRING_SET(String value) {
  const char* S2C1_Value = value.c_str();
  pS2C1->setValue(S2C1_Value);
  S2C1_STRING = value;
  SetStringPref("taskTitle", value);
}

void S2C2_STRING_SET(String value) {
  const char* S2C2_Value = value.c_str();
  pS2C2->setValue(S2C2_Value);
  S2C2_STRING = value;
  SetRawTime(value);
  SetStringPref("taskTime", value);
}

void Set_pS2C3_ValueFromBool (bool value) {
  if (value) {
    pS2C3->setValue("1");
  } else {
    pS2C3->setValue("0");
  }
}

void Set_pS2C3_ValueFromInt (int value) {
  if (value == 1) {
  pS2C3->setValue("1");
  } else if (value == 0) {
    pS2C3->setValue("0");
  }
}

void SetCompleted(bool isCompleted) {
  completed = isCompleted;
  Set_pS2C3_ValueFromBool(completed);
  SetBoolPref("completed", isCompleted);
}

void GetCompletedBoolFromPrefs() {
  completed = GetBoolPref("completed", false);
}