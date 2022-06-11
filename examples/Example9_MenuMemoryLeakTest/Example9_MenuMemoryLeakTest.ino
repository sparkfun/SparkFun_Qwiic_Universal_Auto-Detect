/*
  SFE QUAD Menu Memory Leak Test
  By: Paul Clark
  SparkFun Electronics
  Date: June 2022
  
  This example checks for a memory leak by instantiating a SFE_QUAD_Menu inside the main loop.
  It should run forever, even on boards with limited RAM like the ATmega328P (Uno).
                                  
  License: MIT
  Please see LICENSE.md for more details
  
*/

#include "SFE_QUAD_Menus.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect

void setup()
{
  pinMode (LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("SparkFun Qwiic Universal Auto-Detect Example"));
  Serial.println();
}

void loop()
{
  SFE_QUAD_Menu theMenu;

  theMenu.setMenuPort(Serial);
  theMenu.setDebugPort(Serial);

  theMenu.setMenuTimeout(1000);

  theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("====", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("Toggle the LED", blink);
  theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("TEXT", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  theMenu.addMenuItem("BOOL", SFE_QUAD_MENU_VARIABLE_TYPE_BOOL);
  theMenu.addMenuItem("FLOAT", SFE_QUAD_MENU_VARIABLE_TYPE_FLOAT);
  theMenu.addMenuItem("DOUBLE", SFE_QUAD_MENU_VARIABLE_TYPE_DOUBLE);
  theMenu.addMenuItem("INT", SFE_QUAD_MENU_VARIABLE_TYPE_INT);
  theMenu.addMenuItem("ULONG", SFE_QUAD_MENU_VARIABLE_TYPE_ULONG);
  theMenu.addMenuItem("LONG", SFE_QUAD_MENU_VARIABLE_TYPE_LONG);
  theMenu.addMenuItem("UINT8_T", SFE_QUAD_MENU_VARIABLE_TYPE_UINT8_T);
  theMenu.addMenuItem("UINT16_T", SFE_QUAD_MENU_VARIABLE_TYPE_UINT16_T);
  theMenu.addMenuItem("UINT32_T", SFE_QUAD_MENU_VARIABLE_TYPE_UINT32_T);
  theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);

  SFE_QUAD_Menu_Every_Type_t defaultValue;
  defaultValue.BOOL = 0;
  defaultValue.FLOAT = 1.0;
  defaultValue.DOUBLE = -2.0;
  defaultValue.INT = -3;
  defaultValue.ULONG = 4;
  defaultValue.LONG = -5;
  defaultValue.UINT8_T = 6;
  defaultValue.UINT16_T = 7;
  defaultValue.UINT32_T = 8;

  theMenu.setMenuItemVariable("TEXT", "T-Rex has big teeth");
  theMenu.setMenuItemVariable("BOOL", &defaultValue);
  theMenu.setMenuItemVariable("FLOAT", &defaultValue);
  theMenu.setMenuItemVariable("DOUBLE", &defaultValue);
  theMenu.setMenuItemVariable("INT", &defaultValue);
  theMenu.setMenuItemVariable("ULONG", &defaultValue);
  theMenu.setMenuItemVariable("LONG", &defaultValue);
  theMenu.setMenuItemVariable("UINT8_T", &defaultValue);
  theMenu.setMenuItemVariable("UINT16_T", &defaultValue);
  theMenu.setMenuItemVariable("UINT32_T", &defaultValue);

  theMenu.openMenu();
}

void blink(void)
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
