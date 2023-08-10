#include <SPI.h>
#include "epd2in7_V2.h" // Waveshare 2.7" display library
#include "epdpaint.h"
#include "images.h"
#include <Preferences.h>
#include "SoundHandler.h"
#include "BTHandler.h"

#define COLORED     0
#define UNCOLORED   1

const int buttonPin = A0;
Epd epd;
unsigned char image[1024];
Paint paint(image, 176, 20);    //width should be the multiple of 8
bool menuOpen = false;
bool completed = false;
int menuSelection = 1;
int menuItemCount = 2;
bool firstBoot = true;
Preferences preferences;

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

  //StartUpSound();
  CustomMelody();

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
        UncompleteSound();
        EntryView();
      } else {
        CompletedSound();
        Completed();
      }
    }
    Serial.print("Button 4\n");
  }

}

void EntryView() {
  Serial.print("Running Entry View\n");
  completed = false;
  Set_pS2C3_ValueFromInt(0);
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

  //const char* taskLabel = S2C1_STRING.c_str();
  //const char* timeLabel = S2C2_STRING.c_str();
  const char* taskLabel = S2C1_STRING_GET().c_str();
  const char* timeLabel = S2C2_STRING_GET().c_str();

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(4, 8, taskLabel, &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 38, 0+paint.GetWidth(), 38+paint.GetHeight());

  paint.Clear(COLORED);
  paint.DrawStringAt(58, 8, "Time:", &Font18, UNCOLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 66, 0+paint.GetWidth(), 66+paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(45, 8, timeLabel, &Font18, COLORED);
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

  const char* taskLabel = S2C1_STRING_GET().c_str();
  const char* timeLabel = S2C2_STRING_GET().c_str();

  SetPaintSize(false);
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(4, 8, taskLabel, &Font18, COLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 37, 0+paint.GetWidth(), 37+paint.GetHeight());

  paint.Clear(COLORED);
  paint.DrawStringAt(58, 8, "Time:", &Font18, UNCOLORED);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 66, 0+paint.GetWidth(), 66+paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(45, 8, timeLabel, &Font18, COLORED);
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