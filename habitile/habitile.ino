/**
    @filename   :   epd2in7_V2-demo.ino
    @brief      :   2.7inch e-paper display demo
    @author     :   Waveshare

    Copyright (C) Waveshare     September 20 2022

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documnetation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to  whom the Software is
   furished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <SPI.h>
#include "epd2in7_V2.h"
#include "epdpaint.h"
#include "images.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE1_UUID "180A" // Device Information
#define S1C1_UUID "2A29" // Manufacturer Name String
#define S1C2_UUID "2A24" // Model Number String
#define S1C3_UUID "2A26" // Firmware Revision String
#define S1C4_UUID "2A27" // Hardware Revision String
#define S1C1_STRING "Derrick Goodfriend"
#define S1C2_STRING "Prototype A1"
#define S1C3_STRING "230805.0900"
#define S1C4_STRING "230730"

#define SERVICE2_UUID "181C"
#define S2C1_UUID "2B4E"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914c"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define COLORED     0
#define UNCOLORED   1

#define NAME "Habitile Prototype A1"

const int buttonPin = A0;
Epd epd;
unsigned char image[1024];
Paint paint(image, 176, 20);    //width should be the multiple of 8
bool menuOpen = false;
bool completed = false;
int menuSelection = 1;
int menuItemCount = 2;
bool firstBoot = true;
String S2C1_STRING = "Take out trash";

void setup() {
  Serial.begin(115200);
  // Button test 
  pinMode(buttonPin, INPUT);
  // put your setup code here, to run once:
  //Epd epd;
  setupBT();

  Serial.print("e-Paper init\r\n");
  if (epd.Init_Fast() != 0) {
    Serial.print("e-Paper init failed\r\n");
    return;
  }

  /* This clears the SRAM of the e-paper display */
  epd.Clear();

  /**
      Due to RAM not enough in Arduino UNO, a frame buffer is not allowed.
      In this case, a smaller image buffer is allocated and you have to
      update a partial display several times.
      1 byte = 8 pixels, therefore you have to set 8*N pixels at a time.
  */
  epd.Display_Base_color(0xff);

  EntryView();
  
}

void loop() {
  // read the state of the pushbutton value:
  int temp = analogRead(buttonPin);
  // check if the pushbutton is pressed. If it is, the buttonState is LOW:

  if (temp < 800) {

  }
  else if (temp > 800 && temp < 900) {
    OpenMenu();
    Serial.print("Button 1\n");
  }
  else if (temp > 1450 && temp < 1600) {
    Serial.print("Button 2\n");
    if(menuOpen) {

    } else {
      EntryView();
    }
  }
  else if (temp >= 2600 && temp < 2750) {
    if(menuOpen) {
      MenuScroll();
    } else {
      epd.Init();
      epd.Clear();
      epd.Sleep();
    }
    Serial.print("Button 3\n");
  }
  else if (temp >= 4000 && temp < 4200) {
    if(menuOpen) {
      BT_Advert();
    } else {
      if(completed) {
        EntryView();
      } else {
        Completed();
      }
    }
    Serial.print("Button 4\n");
  }

}

void EntryView() {
  Serial.print("Running Entry View\n");
  completed = false;
  menuOpen = false;
  //if (firstBoot) {
  //  firstBoot = false;
  //  Serial.println("firstBoot set to false");
  //} else {
  epd.Init_Fast();
  //}
  epd.Clear();
  epd.Display_Base_color(0xff);
  epd.Display(habitile_bg);
  SetPaintSize(false);

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(4, 8, "Take out trash", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 38, 0+paint.GetWidth(), 38+paint.GetHeight());

  paint.Clear(COLORED);
  paint.DrawStringAt(58, 8, "Time:", &Font18, UNCOLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 66, 0+paint.GetWidth(), 66+paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(45, 8, "3:00 PM", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 95, 0+paint.GetWidth(), 95+paint.GetHeight());

  paint.Clear(COLORED);
  paint.DrawStringAt(45, 8, "Status:", &Font18, UNCOLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 124, 0+paint.GetWidth(), 124+paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(9, 8, "NOT COMPLETED", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 153, 0+paint.GetWidth(), 153+paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(45, 8, "Derrick", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 182, 0+paint.GetWidth(), 182+paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(27, 8, "Goodfriend", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 198, 0+paint.GetWidth(), 198+paint.GetHeight());

  SetupIcons();

  paint.Clear(UNCOLORED);
  epd.Display_Partial(paint.GetImage(), 90, 264, 90+paint.GetWidth(), 264+paint.GetHeight());  
  
  /* Deep sleep */
  Serial.print("sleep...");
  epd.Sleep();
}

void Completed() {
  Serial.print(("\nRunning Completed\n"));
  completed = true;
  menuOpen = false;
  Serial.print(("Completed/Turn on display\n"));
  epd.Reset();
  Serial.print(("Completed/Clear\n"));
  epd.Init();
  Serial.print(("Completed/Init Fast\n"));
  epd.Clear();
  epd.Display_Base_color(0xff);
  epd.Display(habitile_bg);

  SetPaintSize(false);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(4, 8, "Take out trash", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 37, 0+paint.GetWidth(), 37+paint.GetHeight());

  paint.Clear(COLORED);
  paint.DrawStringAt(58, 8, "Time:", &Font18, UNCOLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 66, 0+paint.GetWidth(), 66+paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(45, 8, "3:00 PM", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 95, 0+paint.GetWidth(), 95+paint.GetHeight());

  paint.Clear(COLORED);
  paint.DrawStringAt(45, 8, "Status:", &Font18, UNCOLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 124, 0+paint.GetWidth(), 124+paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(34, 8, "COMPLETED", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 153, 0+paint.GetWidth(), 153+paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(45, 8, "Derrick", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 182, 0+paint.GetWidth(), 182+paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(27, 8, "Goodfriend", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 198, 0+paint.GetWidth(), 198+paint.GetHeight());

  SetupIcons();

  paint.Clear(UNCOLORED);
  epd.Display_Partial(paint.GetImage(), 90, 264, 90+paint.GetWidth(), 264+paint.GetHeight());

  epd.Sleep();
}

void OpenMenu() {
  Serial.print("Menu button pressed\n");
  if (menuOpen) {
    Serial.print("Menu open bool true\n");
    if (completed) {
      Serial.print("Completed bool true\n");
      Completed();
      menuOpen = false;
    }
    else {
      Serial.print("Completed bool fasle\n");
      EntryView();
      menuOpen = false;
    }
  } else {
    Serial.print("Menu open bool false\n");
  epd.Init_Fast();
  epd.Clear();
  epd.Display(habitile_bg);

  menuOpen = true;

  SetPaintSize(false);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(40, 8, "[ Menu ]", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 37, 0+paint.GetWidth(), 37+paint.GetHeight());

  paint.Clear(COLORED);
  paint.DrawStringAt(40, 8, "Settings", &Font18, UNCOLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 61, 0+paint.GetWidth(), 61+paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(60, 8, "Help", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 85, 0+paint.GetWidth(), 85+paint.GetHeight());

  SetupIcons();

  paint.Clear(UNCOLORED);
  epd.Display_Partial(paint.GetImage(), 90, 264, 90+paint.GetWidth(), 264+paint.GetHeight());

  epd.Sleep();
  }
}

void MenuScroll() {
  int newScroll = menuSelection + 1;
  if (newScroll > menuItemCount) {
    newScroll = 1;
  }
  menuSelection = newScroll;
  MenuScrollView();
}

void MenuScrollView() {
  epd.Init();
  epd.Clear();
  epd.Display_Fast(habitile_bg);
  Serial.print("Menu Scroll/menuSelection = ");
  Serial.print(menuSelection);
  Serial.print("\n");

  SetPaintSize(false);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(40, 8, "[ Menu ]", &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 37, 0+paint.GetWidth(), 37+paint.GetHeight());

  paint.Clear((menuSelection == 1) ? COLORED : UNCOLORED);
  paint.DrawStringAt(40, 8, "Settings", &Font18, (menuSelection == 1) ? UNCOLORED : COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 61, 0+paint.GetWidth(), 61+paint.GetHeight());

  paint.Clear((menuSelection == 2) ? COLORED : UNCOLORED);
  paint.DrawStringAt(60, 8, "Help", &Font18, (menuSelection == 2) ? UNCOLORED : COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 85, 0+paint.GetWidth(), 85+paint.GetHeight());

  SetupIcons();

  paint.Clear(UNCOLORED);
  epd.Display_Partial(paint.GetImage(), 90, 264, 90+paint.GetWidth(), 264+paint.GetHeight());

  epd.Sleep();
}

void SetPaintSize(bool isIcon) {
  if(isIcon) {
    paint.SetHeight(40);
    paint.SetWidth(40);
  } else {
    paint.SetWidth(176);
    paint.SetHeight(24);
  }
}

void SetupIcons() {
  SetPaintSize(true);
  PaintMenuIcon();
  PaintCheckmarkIcon();
  PaintSelectIcon();

  // MUST COME LAST
  SetPaintSize(false);
}

void PaintMenuIcon() {
  if (menuOpen) {
    paint.Clear(UNCOLORED);
    paint.DrawFilledCircle(20, 10, 10, COLORED);
    paint.DrawHorizontalLine(15, 5, 10, UNCOLORED);
    paint.DrawHorizontalLine(15, 10, 10, UNCOLORED);
    paint.DrawHorizontalLine(15, 15, 10, UNCOLORED);
    epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 235, 0+paint.GetWidth(), 235+paint.GetHeight());

    PaintMenuArrows();
  } else {
    paint.Clear(UNCOLORED);
    paint.DrawCircle(20, 10, 10, COLORED);
    paint.DrawHorizontalLine(15, 5, 10, COLORED);
    paint.DrawHorizontalLine(15, 10, 10, COLORED);
    paint.DrawHorizontalLine(15, 15, 10, COLORED);
    epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 235, 0+paint.GetWidth(), 235+paint.GetHeight());
  }
}

void PaintMenuArrows() {
    paint.Clear(UNCOLORED);
    paint.DrawVerticalLine(18, 10, 20, COLORED);
    paint.DrawLine(18, 10, 25, 20, COLORED);
    paint.DrawLine(18, 10, 11, 20, COLORED);
    epd.Display_Partial_Not_refresh(paint.GetImage(), 48, 225, 48+paint.GetWidth(), 225+paint.GetHeight());

    paint.Clear(UNCOLORED);
    paint.DrawVerticalLine(18, 10, 20, COLORED);
    paint.DrawLine(18, 30, 25, 20, COLORED);
    paint.DrawLine(18, 30, 11, 20, COLORED);
    epd.Display_Partial_Not_refresh(paint.GetImage(), 96, 225, 96+paint.GetWidth(), 225+paint.GetHeight());
}

void PaintCheckmarkIcon() {
  if(!menuOpen) {
    if(completed) {
      paint.Clear(UNCOLORED);
      paint.DrawFilledCircle(20, 10, 10, COLORED);
      paint.DrawLine(19, 17, 26, 4, UNCOLORED);
      paint.DrawLine(19, 17, 15, 10, UNCOLORED);
      epd.Display_Partial_Not_refresh(paint.GetImage(), 136, 235, 136+paint.GetWidth(), 235+paint.GetHeight());
    } else {
      paint.Clear(UNCOLORED);
      paint.DrawCircle(20, 10, 10, COLORED);
      paint.DrawLine(19, 17, 26, 4, COLORED);
      paint.DrawLine(19, 17, 15, 10, COLORED);
      epd.Display_Partial_Not_refresh(paint.GetImage(), 136, 235, 136+paint.GetWidth(), 235+paint.GetHeight());
    }
  }
}

void PaintSelectIcon() {
  if(menuOpen) {
    paint.Clear(UNCOLORED);
      paint.DrawCircle(20, 10, 10, COLORED);
      paint.DrawStringAt(11, 7, "OK", &Font18, COLORED);
      epd.Display_Partial_Not_refresh(paint.GetImage(), 136, 235, 136+paint.GetWidth(), 235+paint.GetHeight());
  }
}

void setupBT() {
  Serial.println("Starting BLE work!");

  BLEDevice::init(NAME);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService1 = pServer->createService(SERVICE1_UUID);
  BLECharacteristic *pS1C1 = pService1->createCharacteristic(
                                         S1C1_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pS1C1->setValue(S1C1_STRING);
  BLECharacteristic *pS1C2 = pService1->createCharacteristic(
                                         S1C2_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pS1C2->setValue(S1C2_STRING);
  BLECharacteristic *pS1C3 = pService1->createCharacteristic(
                                         S1C3_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pS1C3->setValue(S1C3_STRING);
  BLECharacteristic *pS1C4 = pService1->createCharacteristic(
                                         S1C4_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
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