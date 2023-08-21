#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>
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
UBYTE *ImageCache;
bool menuOpen = false;
int menuSelection = 1;
int menuItemCount = 5;
bool firstBoot = true;
bool screenClear = false;
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

  DEV_Module_Init();
  EPD_2IN7_V2_Init();
  EPD_2IN7_V2_Clear();
  DEV_Delay_ms(500);
  UWORD Imagesize = ((EPD_2IN7_V2_WIDTH % 8 == 0)? (EPD_2IN7_V2_WIDTH / 8 ): (EPD_2IN7_V2_WIDTH / 8 + 1)) * EPD_2IN7_V2_HEIGHT;
  if ((ImageCache = (UBYTE *)malloc(Imagesize)) == NULL) {
      printf("Failed to apply for black memory...\r\n");
      while (1);
  }
  Paint_NewImage(ImageCache, EPD_2IN7_V2_WIDTH, EPD_2IN7_V2_HEIGHT, ROTATE_0, WHITE);

  CustomMelody();
  SwitchAllLED(LEDState::on);

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
  Serial.println("Button 1 Pressed");
  if (screenClear) {

  } else {
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
    if (screenClear) {

    } else {
      if(completed) {
        UncompleteSound();
        EntryView();
      } else {
        CompletedSound();
        Completed();
      }
    }
  }
  SwitchLED(blue, off);
  Serial.print("Button 4\n");
}

void GetTaskStrings() {
  taskTitle = S2C1_STRING_GET();
  taskTime = S2C2_STRING_GET();
}

void ResetImageCache() {
    Paint_NewImage(ImageCache, EPD_2IN7_V2_WIDTH, EPD_2IN7_V2_HEIGHT, ROTATE_0, WHITE);  	
    Paint_SelectImage(ImageCache);
    Paint_Clear(WHITE);
}

void PaintText(String labelString, int startVert, bool coloredFG, bool coloredBG) {
  const char* label = labelString.c_str();
  int labelLength = strlen(label);
  int startingHorizontalValue = 4;
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

  Paint_DrawString_EN(startingHorizontalValue, startVert, label, &Font12, coloredFG ? WHITE : BLACK, coloredBG ? WHITE : BLACK);
}

void PaintTextWithFont(String labelString, int startVert, bool coloredFG, bool coloredBG, sFONT *Font) {
  const char* label = labelString.c_str();
  int labelLength = strlen(label);
  int startingHorizontalValue = 4;
  int screenWidth = 176;
  int fontWidth = Font->Width;
  int fontHeight = Font->Height;
  if (labelLength <= 25) {
    int totalFontWidth = labelLength * fontWidth;
    int remainingValue = screenWidth - totalFontWidth;
    startingHorizontalValue = remainingValue / 2;
  } else {
    String newLabelString = labelString.substring(0,10);
    newLabelString = newLabelString + "...";
    label = newLabelString.c_str();
  }

  Paint_DrawString_EN(startingHorizontalValue, startVert, label, Font, coloredFG ? WHITE : BLACK, coloredBG ? WHITE : BLACK);
}

void PaintTextWithPaddedBorder(String labelString, int startTextVert, bool coloredFG, bool coloredBG) {
  PaintTextWithFont(labelString, startTextVert, coloredFG, coloredBG, &Font16);
  Paint_DrawRectangle(1, startTextVert-4, 176, startTextVert+20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
}

void InitNewScreen() {
    screenClear = false;
    EPD_2IN7_V2_Init_Fast();
	  Paint_NewImage(ImageCache, EPD_2IN7_V2_WIDTH, EPD_2IN7_V2_HEIGHT, ROTATE_0, WHITE);  	
    Paint_SelectImage(ImageCache);
    Paint_Clear(WHITE);
    Paint_DrawBitMap(habitile_bg);
    EPD_2IN7_V2_Display_Fast(ImageCache);
}

void InitScreen() {
  EPD_2IN7_V2_Init_Fast();
  Paint_NewImage(ImageCache, EPD_2IN7_V2_WIDTH, EPD_2IN7_V2_HEIGHT, ROTATE_0, WHITE);  	
  Paint_SelectImage(ImageCache);
  Paint_Clear(WHITE);
}

void BuildTaskInfoView() {
  PaintText(taskTitle, 52, true, false);
  PaintText("Line 2", 72, true, false);
  PaintText("Line 3", 92, true, false);
  Paint_DrawRectangle(0, 118, 176, 138, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  PaintText("Time:", 122, false, true);
  PaintText(taskTime, 142, true, false);
  Paint_DrawRectangle(0, 158, 176, 178, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  PaintText("Status:", 162, false, true);
  PaintText(completed ? "COMPLETED" : "NOT COMPLETED", 182, true, false);
}

void UpdateDisplay() {
  EPD_2IN7_V2_Display_Fast(ImageCache);
}

void RefreshAndSleep() {
  EPD_2IN7_V2_Init();
  EPD_2IN7_V2_Sleep();
}

void EntryView() {
  Serial.print("Running Entry View\n");
  SetCompleted(false);
  menuOpen = false;
  InitNewScreen();
  GetTaskStrings();
  BuildTaskInfoView();
  SetupIcons();
  UpdateDisplay();
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
  UpdateDisplay();
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
  int firstMenuItemLocation = 72;
  int amountToAdd = 24;
  int startingLocation = firstMenuItemLocation + (amountToAdd * (selection - 1));
  if (MenuSelectionBool(selection)) {
    Paint_DrawRectangle(0, startingLocation-4, 176, startingLocation+16, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  }
  if (MenuSelectionBool(selection)) {
    PaintText("                        >", startingLocation, !MenuSelectionBool(selection), MenuSelectionBool(selection));
  }
  PaintText(labelText, startingLocation, !MenuSelectionBool(selection), MenuSelectionBool(selection));
}

void SetupMenuScreen() {
  PaintTextWithPaddedBorder("Menu", 42, true, false);
  menuItemCount = 5; // Sets number of menu items to display
  PaintMenuText("Settings", 1);
  PaintMenuText("Help", 2);
  PaintMenuText(GetSoundBool() ? "Sounds: on" : "Sounds: off", 3);
  PaintMenuText("BLE Advertise", 4);
  PaintMenuText("Shut Down", 5);
}

void DisplayMenuScreen() {
  Serial.println("Running DisplayMenuScreen");
  menu = DisplayedMenu::main;
  menuSelection = 1;
  menuOpen = true;
  InitNewScreen();
  Serial.println("Init New Screen complete");
  SetupMenuScreen();
  SetupIcons();
  UpdateDisplay();
  RefreshAndSleep();
}

void UpdateMenuScreen() {
  InitNewScreen();
  SetupMenuScreen();
  SetupIcons();
  UpdateDisplay();
  RefreshAndSleep();
}

void OpenHelpMenuScreen() {
  menu = DisplayedMenu::help;
  menuSelection = 1;
  InitNewScreen();
  SetupHelpMenuScreen();
  SetupIcons();
  UpdateDisplay();
  RefreshAndSleep();
}

void UpdateHelpMenuScreen() {
  InitNewScreen();
  SetupHelpMenuScreen();
  SetupIcons();
  UpdateDisplay();
  RefreshAndSleep();
}

void SetupHelpMenuScreen() {
  PaintTextWithPaddedBorder("Help", 42, true, false);

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
  UpdateDisplay();
  RefreshAndSleep();
}

void SetupHelpMenuDeviceScreen() {
  PaintTextWithPaddedBorder("Device Info", 42, true, false);
  PaintText("Zync", 72, true, false);
  PaintText("Prototype A1", 92, true, false);
  PaintText("Firmware Version", 112, true, false);
  PaintText("230820.1930", 132, true, false);
  PaintText("Hardware Version", 152, true, false);
  PaintText("230814", 172, true, false);
  PaintText("(c) 2023 Neurozync LLC", 192, true, false);

  menuItemCount = 1; // Sets number of menu items to display
  //PaintMenuText("<- Back", 6);
}

void SetupSettingsMenuScreen() {
  PaintTextWithPaddedBorder("Settings", 42, true, false);

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
  UpdateDisplay();
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
  EPD_2IN7_V2_Init_Fast();
	EPD_2IN7_V2_Clear();
  RefreshAndSleep();
  menuOpen = false;
  screenClear = true;
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

void SetupIcons() {
  PaintMenuIcon();
  PaintCheckmarkIcon();
  //EPD_2IN7_V2_Display_Fast(ImageCache);
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
  if (filled) {
    Paint_DrawImage(habitile_menuSelected, 2, 222, 40, 40);
  } else {
    Paint_DrawImage(habitile_menuUnselected, 2, 222, 40, 40);
  }
}

void PaintMenuArrows() {
    Paint_DrawImage(habitile_upArrow, 48, 222, 40, 40);
    Paint_DrawImage(habitile_downArrow, 92, 222, 40, 40);
    Paint_DrawLine(89, 222, 89, 264, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
}

void PaintBackArrow() {
    Paint_DrawImage(habitile_backArrow, 2, 222, 40, 40);
}

void PaintCheckmarkIcon() {
  if (menuOpen) {
    PaintSelectIcon();
  } else {
    if (completed) {
      Paint_DrawImage(habitile_filledCheckmark, 136, 222, 40, 40);
    } else {
      Paint_DrawImage(habitile_unfilledCheckmark, 136, 222, 40, 40);
    }
  }
}

void PaintSelectIcon() {
  Paint_DrawImage(habitile_forwardArrow, 136, 222, 40, 40);
}