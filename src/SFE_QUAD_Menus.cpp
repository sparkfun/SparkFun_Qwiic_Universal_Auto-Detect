#include "SFE_QUAD_Menus.h"

SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Item(void)
{
  _next = NULL;
  _itemName = NULL;
  _variableType = SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN;
  _theVariable = NULL;
  _minVal = NULL;
  _maxVal = NULL;
}

SFE_QUAD_Menu_Item::~SFE_QUAD_Menu_Item(void)
{
  deleteMenuItemStorage();
}

void SFE_QUAD_Menu_Item::deleteMenuItemStorage(void)
{
  if (_itemName != NULL)
  {
    delete[] _itemName;
    _itemName = NULL;
  }
  if (_theVariable != NULL)
  {
    delete _theVariable;
    _theVariable = NULL;
  }
  if (_minVal != NULL)
  {
    delete _minVal;
    _minVal = NULL;
  }
  if (_maxVal != NULL)
  {
    delete _maxVal;
    _maxVal = NULL;
  }
}

const char *SFE_QUAD_Menu_Item::getMenuItemName(void)
{
  return ((const char *)_itemName);
}

SFE_QUAD_Menu::SFE_QUAD_Menu(void)
{
  _head = NULL;
  _menuPort = NULL;
  _debugPort = NULL;
}

SFE_QUAD_Menu::~SFE_QUAD_Menu(void)
{
  while (_head != NULL) // Have we got any menu items?
  {
    if (_head->_next == NULL) // Is the the last / only menu item?
    {
      _head->deleteMenuItemStorage();
      delete _head;
      _head = NULL;
    }
    else
    {
      SFE_QUAD_Menu_Item *thisItem = _head;           // Start at the head
      SFE_QUAD_Menu_Item *nextItem = thisItem->_next; // Point to the next menu item
      while (nextItem->_next != NULL)                 // Keep going until we reach the end of the list
      {
        thisItem = nextItem;
        nextItem = nextItem->_next;
      }
      nextItem->deleteMenuItemStorage();
      delete nextItem; // Delete the menu item at the end of the list
      thisItem->_next = NULL;
    }
  }
}

void SFE_QUAD_Menu::setMenuPort(Stream &port)
{
  _menuPort = &port;
}

void SFE_QUAD_Menu::setDebugPort(Stream &port)
{
  _debugPort = &port;
}

bool SFE_QUAD_Menu::addMenuItem(const char *itemName, SFE_QUAD_Menu_Variable_Type_e variableType)
{
  if (variableType >= SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN)
    return (false);

  SFE_QUAD_Menu_Item *menuItemPtr;
  if (_head == NULL) // Is this the first menu item to be added?
  {
    _head = new SFE_QUAD_Menu_Item;
    menuItemPtr = _head;
    if (_debugPort != NULL)
      _debugPort->println(F("addMenuItem: starting at _head"));
  }
  else
  {
    menuItemPtr = _head;               // Start at the head
    while (menuItemPtr->_next != NULL) // Keep going until we reach the end of the list
    {
      menuItemPtr = menuItemPtr->_next;
    }
    menuItemPtr->_next = new SFE_QUAD_Menu_Item;
    menuItemPtr = menuItemPtr->_next;
  }

  if (menuItemPtr == NULL) // Check new was successful
  {
    if (_debugPort != NULL)
      _debugPort->println(F("addMenuItem: menuItemPtr is NULL"));
    return (false);
  }

  menuItemPtr->_itemName = new char[strlen(itemName) + 1]; // Add space for the null

  if (menuItemPtr->_itemName == NULL)
  {
    if (_debugPort != NULL)
      _debugPort->println(F("addMenuItem: _itemName new failed"));
    delete menuItemPtr;
    menuItemPtr = NULL;
    return (false);
  }

  memset(menuItemPtr->_itemName, 0, strlen(itemName) + 1);

  strcpy(menuItemPtr->_itemName, itemName); // Copy the name

  // if (_debugPort != NULL)
  // {
  //   _debugPort->print(F("addMenuItem: _itemName is "));
  //   _debugPort->println(menuItemPtr->_itemName);
  // }

  menuItemPtr->_variableType = variableType; // Record the type

  // if (_debugPort != NULL)
  // {
  //   _debugPort->print(F("addMenuItem: _variableType is "));
  //   _debugPort->println((int)menuItemPtr->_variableType);
  // }

  menuItemPtr->_theVariable = new SFE_QUAD_Menu_Every_Type_t; // Create storage for the variable

  if (menuItemPtr->_theVariable == NULL)
  {
    if (_debugPort != NULL)
      _debugPort->println(F("addMenuItem: _theVariable new failed"));
    delete[] menuItemPtr->_itemName;
    menuItemPtr->_itemName = NULL;
    delete menuItemPtr;
    menuItemPtr = NULL;
    return (false);
  }

  menuItemPtr->_theVariable->TEXT = NULL;
  menuItemPtr->_theVariable->CODE = NULL;

  return (true);
}

bool SFE_QUAD_Menu::addMenuItem(const char *itemName, void (*codePointer)())
{
  SFE_QUAD_Menu_Item *menuItemPtr;
  if (_head == NULL) // Is this the first menu item to be added?
  {
    _head = new SFE_QUAD_Menu_Item;
    menuItemPtr = _head;
  }
  else
  {
    menuItemPtr = _head;               // Start at the head
    while (menuItemPtr->_next != NULL) // Keep going until we reach the end of the list
    {
      menuItemPtr = menuItemPtr->_next;
    }
    menuItemPtr->_next = new SFE_QUAD_Menu_Item;
    menuItemPtr = menuItemPtr->_next;
  }

  if (menuItemPtr == NULL) // Check new was successful
    return (false);

  menuItemPtr->_itemName = new char[strlen(itemName) + 1]; // Add space for the null

  if (menuItemPtr->_itemName == NULL)
  {
    delete menuItemPtr;
    menuItemPtr = NULL;
    return (false);
  }

  memset(menuItemPtr->_itemName, 0, strlen(itemName) + 1);

  strcpy(menuItemPtr->_itemName, itemName); // Copy the name

  // if (_debugPort != NULL)
  // {
  //   _debugPort->print(F("addMenuItem: _itemName is "));
  //   _debugPort->println(menuItemPtr->_itemName);
  // }

  menuItemPtr->_variableType = SFE_QUAD_MENU_VARIABLE_TYPE_CODE; // Record the type

  // if (_debugPort != NULL)
  // {
  //   _debugPort->print(F("addMenuItem: _variableType is "));
  //   _debugPort->println((int)menuItemPtr->_variableType);
  // }

  menuItemPtr->_theVariable = new SFE_QUAD_Menu_Every_Type_t; // Create storage for the variable

  if (menuItemPtr->_theVariable == NULL)
  {
    if (_debugPort != NULL)
      _debugPort->println(F("addMenuItem: _theVariable new failed"));
    delete[] menuItemPtr->_itemName;
    menuItemPtr->_itemName = NULL;
    delete menuItemPtr;
    menuItemPtr = NULL;
    return (false);
  }

  menuItemPtr->_theVariable->CODE = codePointer;

  menuItemPtr->_theVariable->TEXT = NULL;

  return (true);
}

SFE_QUAD_Menu_Variable_Type_e SFE_QUAD_Menu::getMenuItemVariableType(const char *itemName)
{
  SFE_QUAD_Menu_Item *itemExists = menuItemExists(itemName);

  if (itemExists == NULL)
    return (SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN);

  if (itemExists->_theVariable == NULL)
    return (SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN);

  return (itemExists->_variableType);
}

bool SFE_QUAD_Menu::getMenuItemVariable(const char *itemName, SFE_QUAD_Menu_Every_Type_t *theValue)
{
  SFE_QUAD_Menu_Item *itemExists = menuItemExists(itemName);

  if (itemExists == NULL)
    return (false);

  if (itemExists->_theVariable == NULL)
    return (false);

  switch (itemExists->_variableType)
  {
  case SFE_QUAD_MENU_VARIABLE_TYPE_NONE:
  case SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START:
  case SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END:
  case SFE_QUAD_MENU_VARIABLE_TYPE_CODE:
  case SFE_QUAD_MENU_VARIABLE_TYPE_TEXT:
    return (false);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_BOOL:
    theValue->BOOL = itemExists->_theVariable->BOOL;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_FLOAT:
    theValue->FLOAT = itemExists->_theVariable->FLOAT;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_DOUBLE:
    theValue->DOUBLE = itemExists->_theVariable->DOUBLE;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_INT:
    theValue->INT = itemExists->_theVariable->INT;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UINT8_T:
    theValue->UINT8_T = itemExists->_theVariable->UINT8_T;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UINT16_T:
    theValue->UINT16_T = itemExists->_theVariable->UINT16_T;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UINT32_T:
    theValue->UINT32_T = itemExists->_theVariable->UINT32_T;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_ULONG:
    theValue->ULONG = itemExists->_theVariable->ULONG;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN:
  default:
    return (false);
    break;
  }

  return (false);
}

bool SFE_QUAD_Menu::getMenuItemVariable(const char *itemName, char *theValue, size_t maxLen)
{
  SFE_QUAD_Menu_Item *itemExists = menuItemExists(itemName);

  if (itemExists == NULL)
    return (false);

  if (itemExists->_theVariable == NULL)
    return (false);

  switch (itemExists->_variableType)
  {
  case SFE_QUAD_MENU_VARIABLE_TYPE_TEXT:
    if (strlen(itemExists->_theVariable->TEXT) < maxLen) // Is theValue large enough (including the null)?
    {
      memset(theValue, 0, maxLen);
      strcpy(theValue, itemExists->_theVariable->TEXT);
      return (true);
    }
    else
      return (false);
    break;
  default:
    return (false);
    break;
  }

  return (false);
}

bool SFE_QUAD_Menu::setMenuItemVariable(const char *itemName, const SFE_QUAD_Menu_Every_Type_t *theValue)
{
  SFE_QUAD_Menu_Item *itemExists = menuItemExists(itemName);

  if (itemExists == NULL)
  {
    if (_debugPort != NULL)
      _debugPort->println(F("setMenuItemVariable: item does not exist"));
    return (false);
  }

  if (itemExists->_theVariable == NULL)
  {
    if (_debugPort != NULL)
      _debugPort->println(F("setMenuItemVariable: _theVariable is NULL"));
    return (false);
  }

  switch (itemExists->_variableType)
  {
  case SFE_QUAD_MENU_VARIABLE_TYPE_NONE:
  case SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START:
  case SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END:
  case SFE_QUAD_MENU_VARIABLE_TYPE_CODE:
  case SFE_QUAD_MENU_VARIABLE_TYPE_TEXT:
    return (false);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_BOOL:
    itemExists->_theVariable->BOOL = theValue->BOOL;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_FLOAT:
    itemExists->_theVariable->FLOAT = theValue->FLOAT;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_DOUBLE:
    itemExists->_theVariable->DOUBLE = theValue->DOUBLE;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_INT:
    itemExists->_theVariable->INT = theValue->INT;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UINT8_T:
    itemExists->_theVariable->UINT8_T = theValue->UINT8_T;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UINT16_T:
    itemExists->_theVariable->UINT16_T = theValue->UINT16_T;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UINT32_T:
    itemExists->_theVariable->UINT32_T = theValue->UINT32_T;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_ULONG:
    itemExists->_theVariable->ULONG = theValue->ULONG;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN:
  default:
    return (false);
    break;
  }

  return (false);
}

bool SFE_QUAD_Menu::setMenuItemVariable(const char *itemName, const char *theValue)
{
  SFE_QUAD_Menu_Item *itemExists = menuItemExists(itemName);

  if (itemExists == NULL)
    return (false);

  if (itemExists->_theVariable == NULL)
    return (false);

  switch (itemExists->_variableType)
  {
  case SFE_QUAD_MENU_VARIABLE_TYPE_TEXT:
    if (itemExists->_theVariable->TEXT != NULL)
      delete[] itemExists->_theVariable->TEXT;
    itemExists->_theVariable->TEXT = new char[strlen(theValue) + 1]; // Add space for the null
    if (itemExists->_theVariable->TEXT == NULL)
      return (false);
    memset(itemExists->_theVariable->TEXT, 0, strlen(theValue) + 1);
    strcpy(itemExists->_theVariable->TEXT, theValue);
    return (true);
    break;
  default:
    return (false);
    break;
  }

  return (false);
}

bool SFE_QUAD_Menu::setMenuItemVariableMin(const char *itemName, const SFE_QUAD_Menu_Every_Type_t *minVal)
{
  SFE_QUAD_Menu_Item *itemExists = menuItemExists(itemName);

  if (itemExists == NULL)
    return (false);

  if (itemExists->_minVal == NULL)
    itemExists->_minVal = new SFE_QUAD_Menu_Every_Type_t;

  if (itemExists->_minVal == NULL)
    return (false);

  switch (itemExists->_variableType)
  {
  case SFE_QUAD_MENU_VARIABLE_TYPE_NONE:
  case SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START:
  case SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END:
  case SFE_QUAD_MENU_VARIABLE_TYPE_CODE:
  case SFE_QUAD_MENU_VARIABLE_TYPE_TEXT:
  case SFE_QUAD_MENU_VARIABLE_TYPE_BOOL:
    return (false);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_FLOAT:
    itemExists->_minVal->FLOAT = minVal->FLOAT;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_DOUBLE:
    itemExists->_minVal->DOUBLE = minVal->DOUBLE;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_INT:
    itemExists->_minVal->INT = minVal->INT;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UINT8_T:
    itemExists->_minVal->UINT8_T = minVal->UINT8_T;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UINT16_T:
    itemExists->_minVal->UINT16_T = minVal->UINT16_T;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UINT32_T:
    itemExists->_minVal->UINT32_T = minVal->UINT32_T;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_ULONG:
    itemExists->_minVal->ULONG = minVal->ULONG;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN:
  default:
    return (false);
    break;
  }

  return (false);
}

bool SFE_QUAD_Menu::setMenuItemVariableMax(const char *itemName, const SFE_QUAD_Menu_Every_Type_t *maxVal)
{
  SFE_QUAD_Menu_Item *itemExists = menuItemExists(itemName);

  if (itemExists == NULL)
    return (false);

  if (itemExists->_maxVal == NULL)
    itemExists->_maxVal = new SFE_QUAD_Menu_Every_Type_t;

  if (itemExists->_maxVal == NULL)
    return (false);

  switch (itemExists->_variableType)
  {
  case SFE_QUAD_MENU_VARIABLE_TYPE_NONE:
  case SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START:
  case SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END:
  case SFE_QUAD_MENU_VARIABLE_TYPE_CODE:
  case SFE_QUAD_MENU_VARIABLE_TYPE_TEXT:
  case SFE_QUAD_MENU_VARIABLE_TYPE_BOOL:
    return (false);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_FLOAT:
    itemExists->_maxVal->FLOAT = maxVal->FLOAT;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_DOUBLE:
    itemExists->_maxVal->DOUBLE = maxVal->DOUBLE;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_INT:
    itemExists->_maxVal->INT = maxVal->INT;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UINT8_T:
    itemExists->_maxVal->UINT8_T = maxVal->UINT8_T;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UINT16_T:
    itemExists->_maxVal->UINT16_T = maxVal->UINT16_T;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UINT32_T:
    itemExists->_maxVal->UINT32_T = maxVal->UINT32_T;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_ULONG:
    itemExists->_maxVal->ULONG = maxVal->ULONG;
    return (true);
    break;
  case SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN:
  default:
    return (false);
    break;
  }

  return (false);
}

bool SFE_QUAD_Menu::openMenu(SFE_QUAD_Menu_Item *start)
{
  if (_menuPort == NULL) // Check the menu port has been defined
    return (false);

  while (_menuPort->available()) // Clear the menu serial buffer
    _menuPort->read();

  SFE_QUAD_Menu_Item *menuItemPtr; // This will hold a copy of start

  while (1)
  {
    menuItemPtr = start;     // Go back to the start
    if (menuItemPtr == NULL) // Default to _head
      menuItemPtr = _head;

    if (menuItemPtr == NULL)
      return (false);

    size_t maxItemNameLen = getMenuItemNameMaxLen();
    uint32_t menuItems = 1;
    int subMenuLevel = 0;
    bool keepGoing = true;
    bool menuEndSeen = false;

    while (keepGoing) // Print the menu
    {
      // if (_debugPort != NULL)
      // {
      //   _debugPort->print(F("openMenu: _variableType is "));
      //   _debugPort->println((int)menuItemPtr->_variableType);
      // }
      if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN) // This should be impossible?
      {
        // Do nothing
        if (_debugPort != NULL)
          _debugPort->println(F("openMenu: _variableType is UNKNOWN"));
      }
      else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_NONE) // Not a menu item. Just text
      {
        if ((subMenuLevel == 0) and (!menuEndSeen)) // Stay on this menu level
          _menuPort->println(menuItemPtr->_itemName);
      }
      else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END) // Back out of a sub menu
      {
        // If we are in the main menu, there should be as many SUB_MENU_STARTs as there are SUB_MENU_ENDs
        // and subMenuLevel should never go below zero. But if it does, we know we are in a recursive call
        // to a sub menu and have backed out to the menu above. menuEndSeen stops the next sub menu from
        // being printed (when subMenuLevel is zero again).
        subMenuLevel--;
        if (subMenuLevel < 0)
          menuEndSeen = true;
      }
      else if ((subMenuLevel == 0) and (!menuEndSeen)) // Stay on this menu level
      {
        _menuPort->print(menuItems);
        _menuPort->print(F("\t: "));
        _menuPort->print(menuItemPtr->_itemName);
        if (maxItemNameLen > strlen(menuItemPtr->_itemName))
        {
          for (size_t i = 0; i < (maxItemNameLen - strlen(menuItemPtr->_itemName)); i++)
            _menuPort->print(F(" "));
        }
        if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START)
        {
          subMenuLevel++;
          _menuPort->println();
        }
        else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_CODE)
        {
          _menuPort->println();
        }
        else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_TEXT)
        {
          _menuPort->print(F(" : "));
          if (menuItemPtr->_theVariable->TEXT != NULL)
            _menuPort->println(menuItemPtr->_theVariable->TEXT);
          else
            _menuPort->println();
        }
        else
        {
          _menuPort->print(F(" : "));
          if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_BOOL)
            _menuPort->println(menuItemPtr->_theVariable->BOOL == true ? F("Yes") : F("No")); // TO DO: provide a way to let "Enabled" / "Disabled" to be used instead?
          else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_FLOAT)
            _menuPort->println(menuItemPtr->_theVariable->FLOAT);
          else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_DOUBLE)
          {
            _sprintf.printDouble(menuItemPtr->_theVariable->DOUBLE, _menuPort);
            _menuPort->println();
          }
          else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_INT)
            _menuPort->println(menuItemPtr->_theVariable->INT);
          else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_UINT8_T)
            _menuPort->println(menuItemPtr->_theVariable->UINT8_T);
          else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_UINT16_T)
            _menuPort->println(menuItemPtr->_theVariable->UINT16_T);
          else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_UINT32_T)
            _menuPort->println(menuItemPtr->_theVariable->UINT32_T);
          else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_ULONG)
            _menuPort->println(menuItemPtr->_theVariable->ULONG);
          else
            _menuPort->println(F("???")); // This should never happen...
        }

        menuItems++;
      }
      else // Keep track of sub menu levels
      {
        if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START)
          subMenuLevel++;
      }

      if (menuItemPtr->_next == NULL) // Have we reached the end of the menu?
        keepGoing = false;
      else
        menuItemPtr = menuItemPtr->_next; // Point to the next menu item
    }

    _menuPort->println(F("Enter a number, or enter 0 to exit:"));

    uint32_t menuChoice = getMenuChoice(_menuTimeout); // Get menu choice with timeout
    _menuPort->println();

    if (menuChoice == 0)
      return (false);

    menuItemPtr = start;     // Go back to the start
    if (menuItemPtr == NULL) // Default to _head if required
      menuItemPtr = _head;

    menuItems = 1;
    subMenuLevel = 0;
    keepGoing = true;
    menuEndSeen = false;

    while (keepGoing)
    {
      if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN) // This should be impossible?
      {
        // Do nothing
      }
      else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_NONE) // Not a menu item. Just text
      {
        // Do nothing
      }
      else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END)
      {
        subMenuLevel--;
        if (subMenuLevel < 0)
          menuEndSeen = true;
      }
      else if ((subMenuLevel == 0) and (!menuEndSeen)) // Stay on this menu level
      {
        if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START)
        {
          if (menuItems == menuChoice)
          {
            if (menuItemPtr->_next == NULL) // Sanity check
              return (false);
            openMenu(menuItemPtr->_next); // Point to the next item and open the sub menu as a recursive call to this function
            keepGoing = false;
          }
          else
            subMenuLevel++;
        }
        else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_CODE)
        {
          if (menuItems == menuChoice)
          {
            if (menuItemPtr->_theVariable->CODE == NULL) // Sanity check
              return (false);
            menuItemPtr->_theVariable->CODE(); // Run the code
          }
        }
        else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_TEXT)
        {
          if (menuItems == menuChoice)
          {
            if (menuItemPtr->_theVariable->TEXT == NULL) // Sanity check
              return (false);
            _menuPort->println(F("Enter the value (text):"));
            getValueText(&menuItemPtr->_theVariable->TEXT, _menuTimeout); // Do this on a new line - to support deletes etc.
          }
        }
        else
        {
          if (menuItems == menuChoice)
          {
            if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_BOOL)
              menuItemPtr->_theVariable->BOOL ^= 1; // Toggle the bool
            else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_FLOAT)
            {
              _menuPort->print(F("Enter the value (float)"));
              if (menuItemPtr->_minVal != NULL)
              {
                _menuPort->print(F(" (Min: "));
                _menuPort->print(menuItemPtr->_minVal->FLOAT);
                _menuPort->print(F(")"));
              }
              if (menuItemPtr->_maxVal != NULL)
              {
                _menuPort->print(F(" (Max: "));
                _menuPort->print(menuItemPtr->_maxVal->FLOAT);
                _menuPort->print(F(")"));
              }
              _menuPort->println(F(" : "));
              double tempDbl;
              bool success = getValueDouble(&tempDbl, _menuTimeout);
              _menuPort->println();
              if (success)
              {
                bool valOK = true;
                if (menuItemPtr->_minVal != NULL)
                  valOK &= tempDbl >= (double)menuItemPtr->_minVal->FLOAT;
                if (menuItemPtr->_maxVal != NULL)
                  valOK &= tempDbl <= (double)menuItemPtr->_maxVal->FLOAT;
                if (valOK)
                  menuItemPtr->_theVariable->FLOAT = (float)tempDbl;
                else
                  _menuPort->println(F("Invalid value"));
              }
              else
                _menuPort->println(F("Invalid value"));
            }
            else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_DOUBLE)
            {
              _menuPort->print(F("Enter the value (double)"));
              if (menuItemPtr->_minVal != NULL)
              {
                _menuPort->print(F(" (Min: "));
                _sprintf.printDouble(menuItemPtr->_minVal->DOUBLE, _menuPort);
                _menuPort->print(F(")"));
              }
              if (menuItemPtr->_maxVal != NULL)
              {
                _menuPort->print(F(" (Max: "));
                _sprintf.printDouble(menuItemPtr->_maxVal->DOUBLE, _menuPort);
                _menuPort->print(F(")"));
              }
              _menuPort->println(F(" : "));
              double tempDbl;
              bool success = getValueDouble(&tempDbl, _menuTimeout);
              _menuPort->println();
              if (success)
              {
                bool valOK = true;
                if (menuItemPtr->_minVal != NULL)
                  valOK &= tempDbl >= menuItemPtr->_minVal->DOUBLE;
                if (menuItemPtr->_maxVal != NULL)
                  valOK &= tempDbl <= menuItemPtr->_maxVal->DOUBLE;
                if (valOK)
                  menuItemPtr->_theVariable->DOUBLE = tempDbl;
                else
                  _menuPort->println(F("Invalid value"));
              }
              else
                _menuPort->println(F("Invalid value"));
            }
            else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_INT)
            {
              _menuPort->print(F("Enter the value (int)"));
              if (menuItemPtr->_minVal != NULL)
              {
                _menuPort->print(F(" (Min: "));
                _menuPort->print(menuItemPtr->_minVal->INT);
                _menuPort->print(F(")"));
              }
              if (menuItemPtr->_maxVal != NULL)
              {
                _menuPort->print(F(" (Max: "));
                _menuPort->print(menuItemPtr->_maxVal->INT);
                _menuPort->print(F(")"));
              }
              _menuPort->println(F(" : "));
              double tempDbl;
              bool success = getValueDouble(&tempDbl, _menuTimeout);
              _menuPort->println();
              if (success)
              {
                bool valOK = true;
                if (menuItemPtr->_minVal != NULL)
                  valOK &= tempDbl >= (double)menuItemPtr->_minVal->INT;
                if (menuItemPtr->_maxVal != NULL)
                  valOK &= tempDbl <= (double)menuItemPtr->_maxVal->INT;
                if (valOK)
                  menuItemPtr->_theVariable->INT = (int)tempDbl;
                else
                  _menuPort->println(F("Invalid value"));
              }
              else
                _menuPort->println(F("Invalid value"));
            }
            else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_UINT8_T)
            {
              _menuPort->print(F("Enter the value (uint8_t)"));
              if (menuItemPtr->_minVal != NULL)
              {
                _menuPort->print(F(" (Min: "));
                _menuPort->print(menuItemPtr->_minVal->UINT8_T);
                _menuPort->print(F(")"));
              }
              if (menuItemPtr->_maxVal != NULL)
              {
                _menuPort->print(F(" (Max: "));
                _menuPort->print(menuItemPtr->_maxVal->UINT8_T);
                _menuPort->print(F(")"));
              }
              _menuPort->println(F(" : "));
              double tempDbl;
              bool success = getValueDouble(&tempDbl, _menuTimeout);
              _menuPort->println();
              if (success)
              {
                bool valOK = true;
                if (menuItemPtr->_minVal != NULL)
                  valOK &= tempDbl >= (double)menuItemPtr->_minVal->UINT8_T;
                if (menuItemPtr->_maxVal != NULL)
                  valOK &= tempDbl <= (double)menuItemPtr->_maxVal->UINT8_T;
                if (valOK)
                  menuItemPtr->_theVariable->UINT8_T = (uint8_t)tempDbl;
                else
                  _menuPort->println(F("Invalid value"));
              }
              else
                _menuPort->println(F("Invalid value"));
            }
            else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_UINT16_T)
            {
              _menuPort->print(F("Enter the value (uint16_t)"));
              if (menuItemPtr->_minVal != NULL)
              {
                _menuPort->print(F(" (Min: "));
                _menuPort->print(menuItemPtr->_minVal->UINT16_T);
                _menuPort->print(F(")"));
              }
              if (menuItemPtr->_maxVal != NULL)
              {
                _menuPort->print(F(" (Max: "));
                _menuPort->print(menuItemPtr->_maxVal->UINT16_T);
                _menuPort->print(F(")"));
              }
              _menuPort->println(F(" : "));
              double tempDbl;
              bool success = getValueDouble(&tempDbl, _menuTimeout);
              _menuPort->println();
              if (success)
              {
                bool valOK = true;
                if (menuItemPtr->_minVal != NULL)
                  valOK &= tempDbl >= (double)menuItemPtr->_minVal->UINT16_T;
                if (menuItemPtr->_maxVal != NULL)
                  valOK &= tempDbl <= (double)menuItemPtr->_maxVal->UINT16_T;
                if (valOK)
                  menuItemPtr->_theVariable->UINT16_T = (uint16_t)tempDbl;
                else
                  _menuPort->println(F("Invalid value"));
              }
              else
                _menuPort->println(F("Invalid value"));
            }
            else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_UINT32_T)
            {
              _menuPort->print(F("Enter the value (uint32_t)"));
              if (menuItemPtr->_minVal != NULL)
              {
                _menuPort->print(F(" (Min: "));
                _menuPort->print(menuItemPtr->_minVal->UINT32_T);
                _menuPort->print(F(")"));
              }
              if (menuItemPtr->_maxVal != NULL)
              {
                _menuPort->print(F(" (Max: "));
                _menuPort->print(menuItemPtr->_maxVal->UINT32_T);
                _menuPort->print(F(")"));
              }
              _menuPort->println(F(" : "));
              double tempDbl;
              bool success = getValueDouble(&tempDbl, _menuTimeout);
              _menuPort->println();
              if (success)
              {
                bool valOK = true;
                if (menuItemPtr->_minVal != NULL)
                  valOK &= tempDbl >= (double)menuItemPtr->_minVal->UINT32_T;
                if (menuItemPtr->_maxVal != NULL)
                  valOK &= tempDbl <= (double)menuItemPtr->_maxVal->UINT32_T;
                if (valOK)
                  menuItemPtr->_theVariable->UINT32_T = (uint32_t)tempDbl;
                else
                  _menuPort->println(F("Invalid value"));
              }
              else
                _menuPort->println(F("Invalid value"));
            }
            else if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_ULONG)
            {
              _menuPort->print(F("Enter the value (unsigned long)"));
              if (menuItemPtr->_minVal != NULL)
              {
                _menuPort->print(F(" (Min: "));
                _menuPort->print(menuItemPtr->_minVal->ULONG);
                _menuPort->print(F(")"));
              }
              if (menuItemPtr->_maxVal != NULL)
              {
                _menuPort->print(F(" (Max: "));
                _menuPort->print(menuItemPtr->_maxVal->ULONG);
                _menuPort->print(F(")"));
              }
              _menuPort->println(F(" : "));
              double tempDbl;
              bool success = getValueDouble(&tempDbl, _menuTimeout);
              _menuPort->println();
              if (success)
              {
                bool valOK = true;
                if (menuItemPtr->_minVal != NULL)
                  valOK &= tempDbl >= (double)menuItemPtr->_minVal->ULONG;
                if (menuItemPtr->_maxVal != NULL)
                  valOK &= tempDbl <= (double)menuItemPtr->_maxVal->ULONG;
                if (valOK)
                  menuItemPtr->_theVariable->ULONG = (unsigned long)tempDbl;
                else
                  _menuPort->println(F("Invalid value"));
              }
              else
                _menuPort->println(F("Invalid value"));
            }
          }
        }

        menuItems++;
      }
      else // if (subMenuLevel > 0) // Keep track of sub menu levels
      {
        if (menuItemPtr->_variableType == SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START)
          subMenuLevel++;
      }

      if (menuItemPtr->_next == NULL) // Have we reached the end of the menu?
        keepGoing = false;
      else
        menuItemPtr = menuItemPtr->_next; // Point to the next menu item
    }
  }
  return (false); // This is just to keep the compiler happy. The while (1) prevents the code from getting here
}

// Get a positive integer. Return zero if unsuccessful. (Menus always start at 1.)
uint32_t SFE_QUAD_Menu::getMenuChoice(unsigned long timeout)
{
  if (_menuPort == NULL)
    return (0);

  char *tempValue = NULL; // getValueText will create memory for tempValue

  if (!getValueText(&tempValue, timeout))
  {
    delete[] tempValue;
    return (0);
  }

  uint32_t choice = 0;

  for (size_t i = 0; i < strlen(tempValue); i++)
  {
    char c = tempValue[i];

    if ((c >= '0') && (c <= '9'))
    {
      choice *= 10;
      choice += c - '0';
    }
  }

  delete[] tempValue;

  return (choice);
}

bool SFE_QUAD_Menu::getValueDouble(double *value, unsigned long timeout)
{
  if (_menuPort == NULL)
    return (false);

  char *tempValue = NULL; // getValueText will create memory for tempValue

  if (!getValueText(&tempValue, timeout))
  {
    delete[] tempValue;
    return (false);
  }

  *value = 0.0;
  int dp = 0;
  double posNeg = 1.0;
  int exp = 0;
  int expPosNeg = 1;
  bool expSeen = false;

  for (size_t i = 0; i < strlen(tempValue); i++)
  {
    char c = tempValue[i];

    if ((dp == 0) && (expSeen == false)) // If a decimal point has not been seen and an 'e' has not been seen
    {
      if (c == '-')
      {
        posNeg = -1.0;
      }
      else if ((c >= '0') && (c <= '9'))
      {
        *value = *value * 10.0;
        *value = *value + (posNeg * (double)(c - '0'));
      }
      else if (c == '.')
      {
        dp = -1;
      }
      else if ((c == 'e') || (c == 'E'))
      {
        expSeen = true;
      }
    }
    else if ((dp != 0) && (expSeen == false)) // If a decimal point has been seen and an 'e' has not been seen
    {
      if ((c >= '0') && (c <= '9'))
      {
        *value = *value + (posNeg * ((double)(c - '0')) * pow(10, dp));
        dp -= 1;
      }
      else if ((c == 'e') || (c == 'E'))
      {
        expSeen = true;
      }
    }
    else // if (expSeen == true) // If an 'e' has been seen
    {
      if (c == '-')
      {
        expPosNeg = -1;
      }
      else if ((c >= '0') && (c <= '9'))
      {
        exp *= 10;
        exp += expPosNeg * ((int)(c - '0'));
      }
    }
  }

  if (expSeen == true)
    *value = (*value) * pow(10, exp);

  delete[] tempValue;

  return (true);
}

bool SFE_QUAD_Menu::getValueText(char **value, unsigned long timeout)
{
  if (_menuPort == NULL)
    return (false);

  unsigned long startTime = millis();
  bool keepGoing = true;
  uint16_t numChars = 0;
  char c[2];
  c[0] = 0;
  c[1] = 0;
  char *tempValue = new char[_maxTextChars + 1]; // Include space for the null

  if (tempValue == NULL)
    return (false);

  memset(tempValue, 0, _maxTextChars + 1);

  // If *value is not NULL, copy the existing text into tempValue so we can edit it
  if (*value != NULL)
  {
    strcpy(tempValue, *value);
    _menuPort->print(tempValue);
    numChars = strlen(tempValue);
  }

  while (_menuPort->available()) // Clear the menu serial buffer
    _menuPort->read();

  while (keepGoing && (millis() < (startTime + timeout)))
  {
    if (_menuPort->available())
    {
      startTime = millis(); // Update startTime each time we get a character
      c[0] = _menuPort->read();

      if ((c[0] == '\r') || (c[0] == '\n'))
        keepGoing = false;
      else if (c[0] == 8) // Check for BS
      {
        if (numChars > 0)
        {
          numChars--;
          tempValue[numChars] = 0;
          // Is there a truly universal way to handle backspaces? Let's try...
          _menuPort->write('\r');
          for (uint16_t i = 0; i < numChars + 1; i++)
            _menuPort->write(' ');
          _menuPort->write('\r');
          for (uint16_t i = 0; i < numChars; i++)
            _menuPort->write(tempValue[i]);
        }
      }
      else if ((c[0] >= ' ') && (c[0] <= '~') && (numChars < _maxTextChars))
      {
        _menuPort->write(c[0]); // Print the character so the user can see it
        strcat(tempValue, (const char *)c);
        numChars++;
      }
    }
  }

  while (_menuPort->available()) // Clear the menu serial buffer
    _menuPort->read();

  if (keepGoing) // Did we time out?
  {
    delete[] tempValue;
    return (false);
  }

  if (*value != NULL)
    delete[] *value;

  *value = new char[numChars + 1];

  if (*value == NULL)
    return (false);

  memset(*value, 0, numChars + 1);

  strcpy(*value, tempValue);

  delete[] tempValue;

  return (true);
}

bool SFE_QUAD_Menu::writeMenuVariables(Print *pr)
{
  return (false);
}

bool SFE_QUAD_Menu::readMenuVariables(void)
{
  return (false);
}

size_t SFE_QUAD_Menu::getMenuItemNameMaxLen(void)
{
  size_t maxLen = 0;
  SFE_QUAD_Menu_Item *menuItemPtr = _head; // Start at the head
  while (menuItemPtr->_next != NULL)       // Keep going until we reach the end of the list
  {
    if (strlen(menuItemPtr->_itemName) > maxLen)
      maxLen = strlen(menuItemPtr->_itemName);
    menuItemPtr = menuItemPtr->_next;
  }
  return (maxLen);
}

SFE_QUAD_Menu_Item *SFE_QUAD_Menu::menuItemExists(const char *itemName)
{
  SFE_QUAD_Menu_Item *menuItemPtr = _head; // Start at the head

  if (menuItemPtr == NULL)
    return (NULL);

  while (1) // Keep going until we reach the end of the list
  {
    if (strcmp(menuItemPtr->_itemName, itemName) == 0)
      return (menuItemPtr);
    if (menuItemPtr->_next != NULL)
      menuItemPtr = menuItemPtr->_next;
    else
      return (NULL);
  }

  return (NULL); // Keep the compiler happy
}

void SFE_QUAD_Menu_sprintf::printDouble(double value, Print *pr)
{
  bool negative = false;

  if (isnan(value))
  {
    pr->print(F("nan"));
  }
  if (isinf(value))
  {
    pr->print(F("inf"));
  }

  // Handle negative numbers
  if (value < 0.0)
  {
    negative = true;
    value = -value;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  // I optimized out most of the divisions
  double rounding = 2.0;
  for (uint8_t i = 0; i < _prec; ++i)
    rounding *= 10.0;
  rounding = 1.0 / rounding;

  value += rounding;

  // Figure out how big our number really is
  double tenpow = 1.0;
  int digitcount = 1;
  while (value >= 10.0 * tenpow)
  {
    tenpow *= 10.0;
    digitcount++;
  }

  value /= tenpow;

  // Handle negative sign
  if (negative)
    pr->write('-');

  // Print the digits, and if necessary, the decimal point
  digitcount += _prec;
  int8_t digit = 0;
  while (digitcount-- > 0)
  {
    digit = (int8_t)value;
    if (digit > 9)
      digit = 9; // insurance
    pr->write((char)('0' | digit));
    if ((digitcount == _prec) && (_prec > 0))
    {
      pr->write('.');
    }
    value -= digit;
    value *= 10.0;
  }
}
