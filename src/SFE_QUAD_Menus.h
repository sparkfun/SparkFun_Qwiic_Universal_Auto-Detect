#ifndef SPARKFUN_QUAD_MENUS_H
#define SPARKFUN_QUAD_MENUS_H

// SparkFun Qwiic Universal Auto-Detect Menus

#include "SFE_QUAD_Sensors.h"

class SFE_QUAD_Menu_Item
{
public:
  
  SFE_QUAD_Menu_Item(void);
  ~SFE_QUAD_Menu_Item(void);

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

  SFE_QUAD_Menu_Item *_next; // Pointer to the next menu item in the linked list
  char *_itemName; // The name of this menu item - i.e. the text that will be displayed in the menu
  SFE_QUAD_Menu_Variable_Type_e _variableType; // Define the type of the variable associated with this menu item (if any)
  SFE_QUAD_Menu_Every_Type_t *_theVariable; // Storage for the variable
  bool _hasMinMax; // Flag to indicate if this variable has min and max values
  SFE_QUAD_Menu_Every_Type_t *_minVal; // Storage for the min value
  SFE_QUAD_Menu_Every_Type_t *_maxVal; // Storage for the max value

  void deleteMenuItemStorage(void); // Delete (deallocate / free) the _itemName etc.

  const char *getMenuItemName(void); // Return the _itemName as const char

};

class SFE_QUAD_Menu
{
public:
  SFE_QUAD_Menu(void);
  ~SFE_QUAD_Menu(void);

  void setMenuPort(Stream &port);
  bool addMenuItem(const char *itemName, SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Variable_Type_e variableType);
  bool addMenuItem(const char *itemName, void (*codePointer)()); // CODE
  bool getMenuItemVariable(const char *itemName, SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Every_Type_t *theValue);
  bool getMenuItemVariable(const char *itemName, char *theValue); // TEXT
  bool setMenuItemVariable(const char *itemName, const SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Every_Type_t *theValue);
  bool setMenuItemVariable(const char *itemName, const char *theValue); // TEXT
  bool setMenuItemVariableMin(const char *itemName, const SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Every_Type_t *minVal);
  bool setMenuItemVariableMax(const char *itemName, const SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Every_Type_t *maxVal);
  bool openMenu(void);
  bool writeMenuVariables(Print *pr);
  bool readMenuVariables(void); // FIX ME!
  size_t getMenuItemNameMaxLen(void);
  SFE_QUAD_Menu_Item *menuItemExists(const char *itemName);

  SFE_QUAD_Menu_Item *_head; // The head of the linked list of sensors
  Stream *_menuPort; // The Serial port (Stream) used for the menu

  SFE_QUAD_Sensors_sprintf _sprintf; // Provide access to the common sprintf(%f) and sprintf(%e) functions
};

#endif
