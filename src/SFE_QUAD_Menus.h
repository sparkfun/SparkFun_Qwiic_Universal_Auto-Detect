#ifndef SPARKFUN_QUAD_MENUS_H
#define SPARKFUN_QUAD_MENUS_H

// SparkFun Qwiic Universal Auto-Detect Menus

#include "Arduino.h"

// Enum for the different variable types
typedef enum
{
  SFE_QUAD_MENU_VARIABLE_TYPE_NONE = 0,
  SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START,
  SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END,
  SFE_QUAD_MENU_VARIABLE_TYPE_CODE,
  SFE_QUAD_MENU_VARIABLE_TYPE_TEXT,
  SFE_QUAD_MENU_VARIABLE_TYPE_TEXT_EDIT,
  SFE_QUAD_MENU_VARIABLE_TYPE_BOOL,
  SFE_QUAD_MENU_VARIABLE_TYPE_FLOAT,
  SFE_QUAD_MENU_VARIABLE_TYPE_DOUBLE,
  SFE_QUAD_MENU_VARIABLE_TYPE_INT,
  SFE_QUAD_MENU_VARIABLE_TYPE_UINT8_T,
  SFE_QUAD_MENU_VARIABLE_TYPE_UINT16_T,
  SFE_QUAD_MENU_VARIABLE_TYPE_UINT32_T,
  SFE_QUAD_MENU_VARIABLE_TYPE_ULONG,
  SFE_QUAD_MENU_VARIABLE_TYPE_LONG,
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
  long LONG;
} SFE_QUAD_Menu_Every_Type_t;

class SFE_QUAD_Menu_sprintf
{
public:
  void printDouble(double value, Print *pr);
  char *_dtostrf(double value, char *buffer);
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

  void setMenuPort(Stream &port, bool supportsBackspace = false);                              // Define which serial port the menus will use
  void setDebugPort(Stream &port);                                                             // Define which serial port menu debug messages will be output on
  bool addMenuItem(const char *itemName, SFE_QUAD_Menu_Variable_Type_e variableType);          // Add an item of variableType to the menu
  bool addMenuItem(const char *itemName, void (*codePointer)());                               // Add a CODE item to the menu
  SFE_QUAD_Menu_Variable_Type_e getMenuItemVariableType(const char *itemName);                 // Return the variable type for menu itemName
  bool getMenuItemVariable(const char *itemName, SFE_QUAD_Menu_Every_Type_t *theValue);        // Get the menu item variable - returned in the Every_Type
  bool getMenuItemVariable(const char *itemName, char *theValue, size_t maxLen);               // Get the menu item TEXT variable - maxLen defines how many chars theValue can hold
  bool setMenuItemVariable(const char *itemName, const SFE_QUAD_Menu_Every_Type_t *theValue);  // Set the menu variable - the value is passed in the Every_Type
  bool setMenuItemVariable(const char *itemName, const char *theValue);                        // Set the menu variable TEXT
  bool setMenuItemVariableMin(const char *itemName, const SFE_QUAD_Menu_Every_Type_t *minVal); // Define a minimum value for the menu item value - type is the same as the item value
  bool setMenuItemVariableMax(const char *itemName, const SFE_QUAD_Menu_Every_Type_t *maxVal); // Define a maximum value for the menu item value - type is the same as the item value
  bool openMenu(SFE_QUAD_Menu_Item *start = NULL);                                             // Open the menu, using _menuPort, starting at start (too allow for recursive sub-menus)
  uint32_t getMenuChoice(unsigned long timeout);                                               // Ask the user to a menu choice using _menuPort
  bool getValueDouble(double *value, unsigned long timeout);                                   // Ask the user for a double value using _menuPort
  bool getValueText(char **value, unsigned long timeout);                                      // Ask the user for a text value using _menuPort
  uint16_t getNumMenuVariables(void);                                                          // Return the number of menu variables - that could be stored in file / EEPROM etc.
  bool getMenuVariableAsCSV(uint16_t num, char *var, size_t maxLen);                           // Return the menu variable in CSV format, ready to be written to storage - maxLen defines how many chars var can hold
  bool updateMenuVariableFromCSV(char *line);                                                  // Parse and update the menu item variable from a line of CSV (from storage)
  size_t getMenuItemNameMaxLen(void);                                                          // Returns the maximum length of all the menu itemNames
  size_t getMenuVariablesMaxLen(void);                                                         // Returns the likely combined maximum length of a menu variable
  SFE_QUAD_Menu_Item *menuItemExists(const char *itemName);                                    // Return a pointer to the menu item if it exists, otherwise NULL

  SFE_QUAD_Menu_Item *_head;          // The head of the linked list of sensors
  Stream *_menuPort;                  // The Serial port (Stream) used for the menu
  Stream *_debugPort;                 // The Serial port (Stream) used for the debug messages
  unsigned long _menuTimeout = 10000; // Default timeout for the menus (millis)
  void setMenuTimeout(unsigned long newTimeout)
  {
    if (newTimeout > 0)
    {
      _menuTimeout = newTimeout;
    }
  }
  uint16_t _maxTextChars = 32; // Maximum number of chars that can be entered into a text field etc.
  void setMaxTextChars(uint16_t newMax)
  {
    if (newMax >= 32)
    {
      _maxTextChars = newMax;
    }
  }
  bool _supportsBackspace = false; // Flag to indicate if the serial menu supports backspace and so can edit existing text
  void setSupportsBackspace(bool support) { _supportsBackspace = support; }

  SFE_QUAD_Menu_sprintf _sprintf; // Provide access to the common sprintf(%f) and sprintf(%e) functions
};

#endif
