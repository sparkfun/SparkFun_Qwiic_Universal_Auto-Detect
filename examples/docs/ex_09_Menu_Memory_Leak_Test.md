# Example 9 - Menu Memory Leak Test
# How to test for a menu memory leak

In [Example5](ex_05_Memory_Leak_Test.md), we learned how to test the ```SFE_QUAD_Sensors``` class for a memory leak, by declaring
the SparkFun Qwiic Universal Auto-Detect object ```mySensors``` _inside_ the ```loop()```.

This example does the same thing but for the ```SFE_QUAD_Menu``` class.
We run it on a RedBoard to make sure the library menus have no memory leaks. (Actually, we did find one in the TEXT variable - and we fixed it!)

The code only includes the ```SFE_QUAD_Menus``` header file, not the full ```SFE_QUAD_Sensors``` header:

```C++
#include "SFE_QUAD_Menus.h"
```

In this example, the ```theMenu``` object is destructed and (re)instantiated each time the code goes around the ```loop()```:

```C++
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
```
