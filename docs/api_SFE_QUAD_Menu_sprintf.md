# API Reference for the SFE_QUAD_Menu_sprintf class

Helper methods to support printing of double variables.

## Helper Methods

### printDouble()

Print a double (included because not all platforms support dtostrf or sprintf correctly).

```c++
void printDouble(double value, Print *pr)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `value` | `double` | The double to be printed |
| `pr` | `Print *` | The ```Stream```, ```File``` etc. that the double will be printed to |

### _dtostrf()

Convert double to string (included because not all platforms support dtostrf correctly).

It is the calling method's responsibility to ensure ```buffer``` is large enough to contain the complete string.

```c++
char *_dtostrf(double value, char *buffer)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `value` | `double` | The double to be converted to string |
| `buffer` | `char *` | A pointer to a char array to hold the text |
| return value | `char *` | A pointer to the char _after_ the final one printed |

### setPrecision()

Set the precision (number of decimal places) to be used by ```_dtostrf``` and ```printDouble```.

```c++
void setPrecision(unsigned char prec)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `prec` | `unsigned char` | The precision |

### getPrecision()

Returns the current precision.

```c++
unsigned char getPrecision(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `unsigned char` | The precision |

## Member Variables

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `_prec` | `unsigned char` | The precision (number of decimal places) to be printed |
