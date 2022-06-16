# API Reference for the SFE_QUAD_Menu_Item class

## Brief Overview

The ```SFE_QUAD_Menu``` class allows the user to create a menu as a linked-list of ```SFE_QUAD_Menu_Item``` objects.

The ```SFE_QUAD_Menu``` ```_head``` points to the head (start) of the linked list.

The ```_next``` of the final ```SFE_QUAD_Menu_Item``` in the list is ```NULL```.

If the menu item is a variable (BOOL, INT, etc.), the value is stored in a ```SFE_QUAD_Menu_Every_Type_t```
pointed to by ```_theVariable```. ```_theVariable``` is ```NULL``` initially. The ```SFE_QUAD_Menu_Every_Type_t```
object is instantiated if needed and its address is stored in ```_theVariable```. This avoids wasting memory.

## Helper Methods

### const char *getMenuItemName(void)

Returns a pointer to the ```_itemName``` of the menu item so it can be printed etc..

```c++
const char *getMenuItemName(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `const char *` | A pointer to the ```_itemName``` of the menu item |

### deleteMenuItemStorage()

This method is responsible for deleting (freeing) all dynamic memory used by the ```SFE_QUAD_Menu_Item``` object. It is called by the destructor.

```c++
void deleteMenuItemStorage(void)
```

## Member Variables

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `_next` | `SFE_QUAD_Menu_Item *` | A pointer to the next menu item in the linked-list |
| `_itemName` | `char *` | A pointer to a dynamic char array which holds the menu item name (ASCII text, null-terminated) |
| `_variableType` | `SFE_QUAD_Menu_Variable_Type_e` | The menu item type (NONE, CODE, TEXT, BOOL, INT, etc.) |
| `_theVariable` | `SFE_QUAD_Menu_Every_Type_t *` | A pointer to a ```SFE_QUAD_Menu_Every_Type_t``` which will hold the menu item variable (if needed) |
| `_minVal` | `SFE_QUAD_Menu_Every_Type_t *` | A pointer to a ```SFE_QUAD_Menu_Every_Type_t``` which will hold the minimum value for the menu item (if needed) |
| `_maxVal` | `SFE_QUAD_Menu_Every_Type_t *` | A pointer to a ```SFE_QUAD_Menu_Every_Type_t``` which will hold the maximum value for the menu item (if needed) |
