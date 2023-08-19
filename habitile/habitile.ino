#include <SPI.h>
#include "epd2in7_V2.h" // Waveshare 2.7" display library
#include "epdpaint.h"
#include "images.h"
#include <Preferences.h>
#include "SoundHandler.h"
#include "BTHandler.h"
#include <cstring>
#include "LED.h"

#define BLACK 0
#define WHITE 1

enum DisplayedMenu {main,help, help_device, settings};

const int buttonPin = A0; // Analog in for all buttons
Epd epd;
unsigned char image[4096];
Paint paint(image, 176, 20);    //width should be the multiple of 8
bool menuOpen = false;
int menuSelection = 1;
int menuItemCount = 5;
bool firstBoot = true;
String taskTitle = "";
String taskLine2 = "";
String taskLine3 = "";
String taskTime = "";
DisplayedMenu menu;

void setup() {
  Serial.begin(115200);
  // Button test 
  pinMode(buttonPin, INPUT);
  LEDPinModeSetup();
  setupBT();
  
  SetSoundBoolToPrefValue();

  Serial.print("e-Paper init\r\n");
  if (epd.Init_Fast() != 0) {
    Serial.print("e-Paper init failed\r\n");
    return;
  }

  /* This clears the SRAM of the e-paper display */
  epd.Clear();

  //StartUpSound();
  CustomMelody();
  SwitchAllLED(LEDState::on);
  /**
      Due to RAM not enough in Arduino UNO, a frame buffer is not allowed.
      In this case, a smaller image buffer is allocated and you have to
      update a partial display several times.
      1 byte = 8 pixels, therefore you have to set 8*N pixels at a time.
  */
  epd.Display_Base_color(0xff);

  if(completed) {
    Completed();
  } else {
    EntryView();
  }
  SwitchAllLED(LEDState::off);
}

void loop() {
  if (taskUpdate) {
    BTReceiveSound();
    SwitchLED(yellow, on);
    SwitchLED(green, on);
    GetTaskStrings();
    if(completed) {
      Completed();
    } else {
      EntryView();
    }
    SwitchLED(yellow, off);
    SwitchLED(green, off);
    taskUpdate = false;
  }

  if(remoteButton1Press) {
    Button1Press();
    remoteButton1Press = false;
  }
  if(remoteButton2Press) {
    Button2Press();
    remoteButton2Press = false;
  }
  if(remoteButton3Press) {
    Button3Press();
    remoteButton3Press = false;
  }
  if(remoteButton4Press) {
    Button4Press();
    remoteButton4Press = false;
  }

  int buttonPinValue = analogRead(buttonPin);

  if (buttonPinValue < 800) {
    // Do nothing -> No button being pressed
  }
  else if (buttonPinValue > 800 && buttonPinValue < 900) {
    Button1Press();
  }
  else if (buttonPinValue > 1450 && buttonPinValue < 1600) {
    Button2Press();
  }
  else if (buttonPinValue >= 2600 && buttonPinValue < 2750) {
    Button3Press();
  }
  else if (buttonPinValue >= 4000 && buttonPinValue < 4200) {
    Button4Press();
  }

}

void Button1Press() {
  SwitchLED(red, on);
  if (menuOpen) {
    switch(menu) {
      case main:
        OpenMenu();
        SwitchLED(red, off);
        break;
      case help:
        ConfirmSound();
        DisplayMenuScreen();
        SwitchLED(red, off);
        break;
      case help_device:
        ConfirmSound();
        OpenHelpMenuScreen();
        SwitchLED(red, off);
        break;
      case settings:
        ConfirmSound();
        DisplayMenuScreen();
        SwitchLED(red, off);
      default:
        break;
    }
  } else {
    OpenMenu();
    SwitchLED(red, off);
  }
  Serial.print("Button 1\n");
}

void Button2Press() {
  Serial.print("Button 2\n");
  SwitchLED(yellow, on);
  if(menuOpen) {
    MenuScrollUp();
  } else {
    EntryView();
  }
  SwitchLED(yellow, off);
}

void Button3Press() {
  SwitchLED(green, on);
  if(menuOpen) {
    MenuScrollDown();
  } else {

  }
  SwitchLED(green, off);
  Serial.print("Button 3\n");
}

void Button4Press() {
  SwitchLED(blue, on);
  if(menuOpen) {
    HandleMenuSelection();
  } else {
    if(completed) {
      UncompleteSound();
      EntryView();
    } else {
      CompletedSound();
      Completed();
    }
  }
  SwitchLED(blue, off);
  Serial.print("Button 4\n");
}

void GetTaskStrings() {
  taskTitle = S2C1_STRING_GET();
  taskTime = S2C2_STRING_GET();
}

void PaintText(String labelString, int startVert, bool coloredFG, bool coloredBG) {
  const char* label = labelString.c_str();
  int labelLength = strlen(label);
  int startingHorizontalValue = 4;
  int startingVerticalValue = 8;
  int screenWidth = 176;
  int fontWidth = 7;
  int fontHeight = 12;
  if (labelLength <= 25) {
    int totalFontWidth = labelLength * fontWidth;
    int remainingValue = screenWidth - totalFontWidth;
    startingHorizontalValue = remainingValue / 2;
  } else {
    String newLabelString = labelString.substring(0,10);
    newLabelString = newLabelString + "...";
    label = newLabelString.c_str();
  }
  paint.Clear(coloredBG ? BLACK : WHITE);
  paint.DrawStringAt(startingHorizontalValue, startingVerticalValue, label, &Font12, coloredFG ? BLACK : WHITE);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, startVert, 0+paint.GetWidth(), startVert+paint.GetHeight());
}

void ClearPaintArea(int startVert, bool coloredBG) {
  paint.Clear(coloredBG ? BLACK : WHITE);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, startVert, 0+paint.GetWidth(), startVert+paint.GetHeight());
}

void InitNewScreen() {
  epd.Init();
  epd.Clear();
  //epd.Display_Base_color(0xff);
  epd.Display(habitile_bg);
  SetPaintSize(false);
}

void InitScreen() {
  epd.Init_Fast();
  SetPaintSize(false);
}

void BuildTaskInfoView() {
  PaintText(taskTitle, 38, true, false);
  PaintText("Line 2", 58, true, false);
  PaintText("Line 3", 78, true, false);
  PaintText("Time:", 102, false, true);
  PaintText(taskTime, 131, true, false);
  PaintText("Status:", 160, false, true);
  PaintText(completed ? "COMPLETED" : "NOT COMPLETED", 189, true, false);
}

void RefreshAndSleep() {
  paint.Clear(WHITE);
  epd.Display_Partial(paint.GetImage(), 90, 264, 90+paint.GetWidth(), 264+paint.GetHeight());  
  epd.Sleep();
}

void Refresh() {
  PaintText("", 66, true, false);
  PaintText("", 95, true, false);
  PaintText("", 124, true, false);
  PaintText("", 153, true, false);
  PaintText("", 182, true, false);
  PaintText("", 198, true, false);
  epd.Display_Partial(paint.GetImage(), 90, 264, 90+paint.GetWidth(), 264+paint.GetHeight()); 
}

void EntryView() {
  Serial.print("Running Entry View\n");
  SetCompleted(false);
  menuOpen = false;
  InitNewScreen();
  GetTaskStrings();
  BuildTaskInfoView();
  SetupIcons();
  RefreshAndSleep();
}


void Completed() {
  Serial.print(("\nRunning Completed\n"));
  SetCompleted(true);
  menuOpen = false;
  InitNewScreen();
  GetTaskStrings();
  BuildTaskInfoView();
  SetupIcons();
  RefreshAndSleep();
}

bool MenuSelectionBool(int selection) {
  if (selection == menuSelection) {
    return true;
  } else {
    return false;
  }
}

void PaintMenuText(String labelText, int selection) {
  int firstMenuItemLocation = 61;
  int amountToAdd = 24;
  int startingLocation = firstMenuItemLocation + (amountToAdd * (selection - 1));
  PaintText(labelText, startingLocation, !MenuSelectionBool(selection), MenuSelectionBool(selection));
}

void SetupMenuScreen() {
  SetPaintSize(false);
  paint.Clear(WHITE);
  paint.DrawStringAt(40, 8, "[ Menu ]", &Font12, BLACK);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 37, 0+paint.GetWidth(), 37+paint.GetHeight());

  menuItemCount = 5; // Sets number of menu items to display
  PaintMenuText("Settings", 1);
  PaintMenuText("Help", 2);
  PaintMenuText(GetSoundBool() ? "Sounds: on" : "Sounds: off", 3);
  PaintMenuText("BLE Advertise", 4);
  PaintMenuText("Shut Down", 5);
}

void DisplayMenuScreen() {
  menu = DisplayedMenu::main;
  menuSelection = 1;
  menuOpen = true;
  InitNewScreen();
  SetupMenuScreen();
  SetupIcons();
  RefreshAndSleep();
}

void UpdateMenuScreen() {
  InitNewScreen();
  SetupMenuScreen();
  SetupIcons();
  RefreshAndSleep();
}

void OpenHelpMenuScreen() {
  menu = DisplayedMenu::help;
  menuSelection = 1;
  InitNewScreen();
  SetupHelpMenuScreen();
  SetupIcons();
  RefreshAndSleep();
}

void UpdateHelpMenuScreen() {
  InitNewScreen();
  SetupHelpMenuScreen();
  SetupIcons();
  RefreshAndSleep();
}

void SetupHelpMenuScreen() {
  SetPaintSize(false);
  paint.Clear(WHITE);
  paint.DrawStringAt(40, 8, "[ Help ]", &Font12, BLACK);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 37, 0+paint.GetWidth(), 37+paint.GetHeight());

  menuItemCount = 5; // Sets number of menu items to display
  PaintMenuText("Get Started", 1);
  PaintMenuText("Device Info", 2);
  PaintMenuText("Solve Problems", 3);
  PaintMenuText("Support Info", 4);
  PaintMenuText("<- Back", 5);
}


void OpenHelpMenuDeviceScreen() {
  menu = DisplayedMenu::help_device;
  menuSelection = 1;
  InitNewScreen();
  SetupHelpMenuDeviceScreen();
  SetupIcons();
  RefreshAndSleep();
}

void SetupHelpMenuDeviceScreen() {
  SetPaintSize(false);
  paint.Clear(WHITE);
  paint.DrawStringAt(10, 8, "[Device Info]", &Font12, BLACK);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 37, 0+paint.GetWidth(), 37+paint.GetHeight());
  PaintText("Zync", 58, true, false);
  PaintText("Prototype A1", 78, true, false);
  PaintText("Firmware Ver", 98, true, false);
  PaintText("230816.1840", 118, true, false);
  PaintText("Hardware Ver", 138, true, false);
  PaintText("230814", 158, true, false);
  PaintText("Neurozync LLC", 178, true, false);

  menuItemCount = 1; // Sets number of menu items to display
  //PaintMenuText("<- Back", 6);
}

void SetupSettingsMenuScreen() {
  SetPaintSize(false);
  paint.Clear(WHITE);
  paint.DrawStringAt(16, 8, "[ Settings ]", &Font12, BLACK);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 37, 0+paint.GetWidth(), 37+paint.GetHeight());

  menuItemCount = 2; // Sets number of menu items to display
  bool amPM = GetAMPM();
  PaintMenuText(amPM ? "12 hr (AM/PM)" : "24 hr", 1);
  PaintMenuText("<- Back", 2);
}


void OpenSettingsMenuScreen() {
  menu = DisplayedMenu::settings;
  menuSelection = 1;
  InitNewScreen();
  SetupSettingsMenuScreen();
  SetupIcons();
  RefreshAndSleep();
}

void Settings_ToggleAMPM() {
  ToggleAMPM();
  OpenSettingsMenuScreen();
}

void HandleMenuSelection() {
  switch(menuSelection) {
    case 1:
      switch(menu) {
        case main:
          ConfirmSound();
          OpenSettingsMenuScreen();
          break;
        case help:
          break;
        case help_device:
          ConfirmSound();
          OpenHelpMenuScreen();
          break;
        case settings:
          // Set AMPM Value
          ConfirmSound();
          Settings_ToggleAMPM();
          break;
        default:
          break;
      }
      break;
    case 2:
      switch(menu) {
        case main:
          ConfirmSound();
          OpenHelpMenuScreen();
          break;
        case help:
          ConfirmSound();
          OpenHelpMenuDeviceScreen();
          break;
        case help_device:
          break;
        case settings:
          ConfirmSound();
          DisplayMenuScreen();
          break;
        default:
          break;
      }
      break;
    case 3:
      switch(menu) {
        case main:
          ConfirmSound();
          ToggleSoundBool();
          UpdateMenuScreen();
          break;
        case help:
          break;
        default:
          break;
      }
      break;
    case 4:
      switch(menu) {
        case main:
          BT_Advert();
          ConfirmSound();
          break;
        case help:
          break;
        default:
          break;
      }
      break;
    case 5:
      switch(menu) {
        case main:
          ConfirmSound();
          ClearScreen();
          break;
        case help:
          ConfirmSound();
          DisplayMenuScreen();
          break;
        default:
          break;
      }
    default:
      break;
  }
}

void ClearScreen() {
  epd.Init();
  epd.Clear();
  epd.Sleep();
}

void OpenMenu() {
  ConfirmSound();
  Serial.print("Menu button pressed\n");
  
  if (menuOpen) {
    Serial.print("Menu open bool true\n");
    if (completed) {
      Serial.print("Completed bool true\n");
      Completed();
    }
    else {
      Serial.print("Completed bool false\n");
      EntryView();
    }
    menuOpen = false; // Set the menuOpen flag to false after transitioning to another view
  } 
  else {
    Serial.print("Menu open bool false\n");
    DisplayMenuScreen();
    menuOpen = true; // Set the menuOpen flag to true after displaying the menu
  }
}

void MenuScrollDisplayRefresh() {
  switch(menu) {
    case main:
      UpdateMenuScreen();
      break;
    case help:
      UpdateHelpMenuScreen();
      break;
    default:
      break;
  }
}

void MenuScrollDown() {
  ScrollDownSound();
  int newScroll = menuSelection + 1;
  if (newScroll > menuItemCount) {
    newScroll = 1;
  }
  menuSelection = newScroll;
  MenuScrollDisplayRefresh();
}

void MenuScrollUp() {
  ScrollUpSound();
  int newScroll = menuSelection - 1;
  if (newScroll == 0) {
    newScroll = menuItemCount;
  }
  menuSelection = newScroll;
  MenuScrollDisplayRefresh();
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
    switch(menu) {
      case main:
        DrawMenuIcon(true);
        break;
      default:
        PaintBackArrow();
        break;
    }
    PaintMenuArrows();
  } else {
    DrawMenuIcon(false);
  }
}

void DrawMenuIcon(bool filled) {
  paint.Clear(WHITE);
  if (filled) {
    paint.DrawFilledCircle(20, 10, 10, BLACK);
  } else {
    paint.DrawCircle(20, 10, 10, BLACK);
  }
  paint.DrawHorizontalLine(15, 5, 10, filled ? WHITE : BLACK);
  paint.DrawHorizontalLine(15, 10, 10, filled ? WHITE : BLACK);
  paint.DrawHorizontalLine(15, 15, 10, filled ? WHITE : BLACK);
  epd.Display_Partial_Not_refresh(paint.GetImage(), 0, 235, 0+paint.GetWidth(), 235+paint.GetHeight());
}

void PaintMenuArrows() {
    paint.Clear(WHITE);
    paint.DrawVerticalLine(18, 10, 20, BLACK);
    paint.DrawLine(18, 10, 25, 20, BLACK);
    paint.DrawLine(18, 10, 11, 20, BLACK);
    epd.Display_Partial_Not_refresh(paint.GetImage(), 48, 225, 48+paint.GetWidth(), 225+paint.GetHeight());

    paint.Clear(WHITE);
    paint.DrawVerticalLine(18, 10, 20, BLACK);
    paint.DrawLine(18, 30, 25, 20, BLACK);
    paint.DrawLine(18, 30, 11, 20, BLACK);
    epd.Display_Partial_Not_refresh(paint.GetImage(), 96, 225, 96+paint.GetWidth(), 225+paint.GetHeight());
}

void PaintBackArrow() {
    // Define the size and positions
    const int arrowSize = 40; 
    int x = 2;  // Starting position for the 44x44 area of position 1
    int y = 2 + (44 - arrowSize) / 2;  // Vertically centering the arrow in the 44x44 area

    // Clear the 40x40 paint area
    paint.Clear(WHITE);
    
    // Define the arrow shaft's start and end positions
    int shaftStartX = x + arrowSize - 10;  // 10 pixels from the right edge
    int shaftEndX = x + 10;  // 10 pixels from the left edge
    int shaftY = y + arrowSize / 2;  // Middle of the arrow area
    
    // Draw the horizontal shaft of the arrow
    paint.DrawLine(shaftStartX, shaftY, shaftEndX, shaftY, BLACK);
    
    // Draw the two diagonal lines for the arrowhead (pointing left)
    int arrowheadLength = 7;  // Define the length of the arrowhead lines
    paint.DrawLine(shaftEndX, shaftY, shaftEndX + arrowheadLength, shaftY - arrowheadLength, BLACK);  // Upper diagonal
    paint.DrawLine(shaftEndX, shaftY, shaftEndX + arrowheadLength, shaftY + arrowheadLength, BLACK);  // Lower diagonal
    
    // Refresh the section of the display where the arrow is painted
    epd.Display_Partial_Not_refresh(paint.GetImage(), x, 225, x + arrowSize, 225 + arrowSize);
}

void PaintCheckmarkIcon() {
  if(!menuOpen) {
    if(completed) {
      paint.Clear(WHITE);
      paint.DrawFilledCircle(20, 10, 10, BLACK);
      paint.DrawLine(19, 17, 26, 4, WHITE);
      paint.DrawLine(19, 17, 15, 10, WHITE);
      epd.Display_Partial_Not_refresh(paint.GetImage(), 136, 235, 136+paint.GetWidth(), 235+paint.GetHeight());
    } else {
      paint.Clear(WHITE);
      paint.DrawCircle(20, 10, 10, BLACK);
      paint.DrawLine(19, 17, 26, 4, BLACK);
      paint.DrawLine(19, 17, 15, 10, BLACK);
      epd.Display_Partial_Not_refresh(paint.GetImage(), 136, 235, 136+paint.GetWidth(), 235+paint.GetHeight());
    }
  }
}

void PaintSelectIcon() {
  if(menuOpen) {
    paint.Clear(WHITE);
      paint.DrawCircle(20, 10, 10, BLACK);
      paint.DrawStringAt(11, 7, "OK", &Font12, BLACK);
      epd.Display_Partial_Not_refresh(paint.GetImage(), 136, 235, 136+paint.GetWidth(), 235+paint.GetHeight());
  }
}