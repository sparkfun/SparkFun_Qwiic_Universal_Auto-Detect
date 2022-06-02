#include "SFE_QUAD_Menus.h"

SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Item(void)
{
  _next = NULL;
  _itemName = NULL;
  _theVariable = NULL;
  _hasMinMax = false;
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

bool SFE_QUAD_Menu::addMenuItem(const char *itemName, SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Variable_Type_e variableType)
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
    menuItemPtr = new SFE_QUAD_Menu_Item;
  }

  if (menuItemPtr == NULL) // Check new was successful
    return (false);

  menuItemPtr->_itemName = new char[strlen(itemName + 1)]; // Add space for the null

  if (menuItemPtr->_itemName == NULL)
  {
    delete menuItemPtr;
    menuItemPtr = NULL;
    return (false);
  }

  strcpy(menuItemPtr->_itemName, itemName);                // Copy the name

  menuItemPtr->_variableType = variableType; // Record the type

  menuItemPtr->_theVariable = new SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Every_Type_t; // Create storage for the variable

  if (menuItemPtr->_theVariable == NULL)
  {
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
    menuItemPtr = new SFE_QUAD_Menu_Item;
  }

  if (menuItemPtr == NULL) // Check new was successful
    return (false);

  menuItemPtr->_itemName = new char[strlen(itemName + 1)]; // Add space for the null

  if (menuItemPtr->_itemName == NULL)
  {
    delete menuItemPtr;
    menuItemPtr = NULL;
    return (false);
  }

  strcpy(menuItemPtr->_itemName, itemName);                // Copy the name

  menuItemPtr->_variableType = SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_CODE; // Record the type

  menuItemPtr->_theVariable = new SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Every_Type_t; // Create storage for the variable

  if (menuItemPtr->_theVariable == NULL)
  {
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

bool SFE_QUAD_Menu::getMenuItemVariable(const char *itemName, SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Every_Type_t *theValue)
{
  SFE_QUAD_Menu_Item *itemExists = menuItemExists(itemName);

  if (itemExists == NULL)
    return (false);

  if (itemExists->_theVariable == NULL)
    return (false);

  switch (itemExists->_variableType)
  {
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_NONE:
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START:
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END:
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_CODE:
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_TEXT:
      return (false);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_BOOL:
      theValue->BOOL = itemExists->_theVariable->BOOL;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_FLOAT:
      theValue->FLOAT = itemExists->_theVariable->FLOAT;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_DOUBLE:
      theValue->DOUBLE = itemExists->_theVariable->DOUBLE;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_INT:
      theValue->INT = itemExists->_theVariable->INT;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_UINT8_T:
      theValue->UINT8_T = itemExists->_theVariable->UINT8_T;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_UINT16_T:
      theValue->UINT16_T = itemExists->_theVariable->UINT16_T;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_UINT32_T:
      theValue->UINT32_T = itemExists->_theVariable->UINT32_T;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_ULONG:
      theValue->ULONG = itemExists->_theVariable->ULONG;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN:
    default:
      return (false);
      break;
  }

  return (false);
}

bool SFE_QUAD_Menu::getMenuItemVariable(const char *itemName, char *theValue)
{
  SFE_QUAD_Menu_Item *itemExists = menuItemExists(itemName);

  if (itemExists == NULL)
    return (false);

  if (itemExists->_theVariable == NULL)
    return (false);

  switch (itemExists->_variableType)
  {
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_TEXT:
      theValue = itemExists->_theVariable->TEXT;
      return (true);
      break;
    default:
      return (false);
      break;
  }

  return (false);
}

bool SFE_QUAD_Menu::setMenuItemVariable(const char *itemName, const SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Every_Type_t *theValue)
{
  SFE_QUAD_Menu_Item *itemExists = menuItemExists(itemName);

  if (itemExists == NULL)
    return (false);

  if (itemExists->_theVariable == NULL)
    return (false);

  switch (itemExists->_variableType)
  {
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_NONE:
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START:
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END:
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_CODE:
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_TEXT:
      return (false);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_BOOL:
      itemExists->_theVariable->BOOL = theValue->BOOL;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_FLOAT:
      itemExists->_theVariable->FLOAT = theValue->FLOAT;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_DOUBLE:
      itemExists->_theVariable->DOUBLE = theValue->DOUBLE;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_INT:
      itemExists->_theVariable->INT = theValue->INT;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_UINT8_T:
      itemExists->_theVariable->UINT8_T = theValue->UINT8_T;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_UINT16_T:
      itemExists->_theVariable->UINT16_T = theValue->UINT16_T;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_UINT32_T:
      itemExists->_theVariable->UINT32_T = theValue->UINT32_T;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_ULONG:
      itemExists->_theVariable->ULONG = theValue->ULONG;
      return (true);
      break;
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_UNKNOWN:
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
    case SFE_QUAD_Menu_Item::SFE_QUAD_MENU_VARIABLE_TYPE_TEXT:
      if (itemExists->_theVariable->TEXT != NULL)
        delete[] itemExists->_theVariable->TEXT;
      itemExists->_theVariable->TEXT = new char[strlen(theValue) + 1]; // Add space for the null
      strcpy(itemExists->_theVariable->TEXT, theValue);
      return (true);
      break;
    default:
      return (false);
      break;
  }

  return (false);
}

bool SFE_QUAD_Menu::setMenuItemVariableMin(const char *itemName, const SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Every_Type_t *minVal)
{
  return (false);
}

bool SFE_QUAD_Menu::setMenuItemVariableMax(const char *itemName, const SFE_QUAD_Menu_Item::SFE_QUAD_Menu_Every_Type_t *maxVal)
{
  return (false);
}

bool SFE_QUAD_Menu::openMenu(void)
{
  return (false);
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
  return (0);
}

SFE_QUAD_Menu_Item *SFE_QUAD_Menu::menuItemExists(const char *itemName)
{
  return (NULL);
}

