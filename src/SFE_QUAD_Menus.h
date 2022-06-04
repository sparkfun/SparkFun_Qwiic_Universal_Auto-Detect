#ifndef SPARKFUN_QUAD_MENUS_H
#define SPARKFUN_QUAD_MENUS_H

// SparkFun Qwiic Universal Auto-Detect Menus

#include "SFE_QUAD_Sensors.h"

// Enum for the different variable types
typedef enum
{
  SFE_QUAD_MENU_VARIABLE_TYPE_NONE = 0,
  SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START,
  SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END,
  SFE_QUAD_MENU_VARIABLE_TYPE_CODE,
  SFE_QUAD_MENU_VARIABLE_TYPE_TEXT,
  SFE_QUAD_MENU_VARIABLE_TYPE_BOOL,
  SFE_QUAD_MENU_VARIABLE_TYPE_FLOAT,
  SFE_QUAD_MENU_VARIABLE_TYPE_DOUBLE,
  SFE_QUAD_MENU_VARIABLE_TYPE_INT,
  SFE_QUAD_MENU_VARIABLE_TYPE_UINT8_T,
  SFE_QUAD_MENU_VARIABLE_TYPE_UINT16_T,
  SFE_QUAD_MENU_VARIABLE_TYPE_UINT32_T,
  SFE_QUAD_MENU_VARIABLE_TYPE_ULONG,
  SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN // Must be last
} SFE_QUAD_Menu_Variable_Type_e;

// Struct to hold "every type"
typedef struct
{
  void (*CODE)();
  char *TEXT;
  bool BOOL;
  float FLOAT;
  double DOUBLE;
  int INT;
  uint8_t UINT8_T;
  uint16_t UINT16_T;
  uint32_t UINT32_T;
  unsigned long ULONG;
} SFE_QUAD_Menu_Every_Type_t;

class SFE_QUAD_Menu_sprintf
{
public:
  void printDouble(double value, Print *pr);
  unsigned char _prec = 7;                                // precision
  void setPrecision(unsigned char prec) { _prec = prec; } // Call setPrecision to change the number of decimal places for the readings
  unsigned char getPrecision(void) { return (_prec); }
};

class SFE_QUAD_Menu_Item
{
public:
  SFE_QUAD_Menu_Item(void);
  ~SFE_QUAD_Menu_Item(void);

  SFE_QUAD_Menu_Item *_next;                   // Pointer to the next menu item in the linked list
  char *_itemName;                             // The name of this menu item - i.e. the text that will be displayed in the menu
  SFE_QUAD_Menu_Variable_Type_e _variableType; // Define the type of the variable associated with this menu item (if any)
  SFE_QUAD_Menu_Every_Type_t *_theVariable;    // Storage for the variable
  SFE_QUAD_Menu_Every_Type_t *_minVal;         // Storage for the min value
  SFE_QUAD_Menu_Every_Type_t *_maxVal;         // Storage for the max value

  void deleteMenuItemStorage(void); // Delete (deallocate / free) the _itemName etc.

  const char *getMenuItemName(void); // Return the _itemName as const char
};

class SFE_QUAD_Menu
{
public:
  SFE_QUAD_Menu(void);
  ~SFE_QUAD_Menu(void);

  void setMenuPort(Stream &port);
  void setDebugPort(Stream &port);
  bool addMenuItem(const char *itemName, SFE_QUAD_Menu_Variable_Type_e variableType);
  bool addMenuItem(const char *itemName, void (*codePointer)()); // CODE
  SFE_QUAD_Menu_Variable_Type_e getMenuItemVariableType(const char *itemName);
  bool getMenuItemVariable(const char *itemName, SFE_QUAD_Menu_Every_Type_t *theValue);
  bool getMenuItemVariable(const char *itemName, char *theValue, size_t maxLen); // TEXT
  bool setMenuItemVariable(const char *itemName, const SFE_QUAD_Menu_Every_Type_t *theValue);
  bool setMenuItemVariable(const char *itemName, const char *theValue); // TEXT
  bool setMenuItemVariableMin(const char *itemName, const SFE_QUAD_Menu_Every_Type_t *minVal);
  bool setMenuItemVariableMax(const char *itemName, const SFE_QUAD_Menu_Every_Type_t *maxVal);
  bool openMenu(SFE_QUAD_Menu_Item *start = NULL);
  uint32_t getMenuChoice(unsigned long timeout);
  bool getValueDouble(double *value, unsigned long timeout);
  bool getValueText(char **value, unsigned long timeout);
  bool writeMenuVariables(Print *pr);
  bool readMenuVariables(void); // FIX ME!
  size_t getMenuItemNameMaxLen(void);
  SFE_QUAD_Menu_Item *menuItemExists(const char *itemName);

  SFE_QUAD_Menu_Item *_head;          // The head of the linked list of sensors
  Stream *_menuPort;                  // The Serial port (Stream) used for the menu
  Stream *_debugPort;                 // The Serial port (Stream) used for the debug messages
  unsigned long _menuTimeout = 10000; // Default timeout for the menus
  void setMenuTimeout(unsigned long newTimeout) { _menuTimeout = newTimeout; }
  uint16_t _maxTextChars = 32; // Maximum number of chars that can be entered into a text field etc.
  void setMaxTextChars(uint16_t newMax) { _maxTextChars = newMax; }

  SFE_QUAD_Menu_sprintf _sprintf; // Provide access to the common sprintf(%f) and sprintf(%e) functions
};

#endif
