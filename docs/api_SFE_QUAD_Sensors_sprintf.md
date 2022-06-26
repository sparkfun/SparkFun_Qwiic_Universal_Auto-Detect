# API Reference for the SFE_QUAD_Sensors_sprintf class

Helper methods to support printing of double and exponent-format data.

## Helper Methods

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

### _etoa()

Convert double to ASCII text using exponent format.

It is the calling method's responsibility to ensure ```buffer``` is large enough to contain the complete string.

```c++
char *_etoa(double value, char *buffer)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `value` | `double` | The double to be converted to string |
| `buffer` | `char *` | A pointer to a char array to hold the text |
| return value | `char *` | A pointer to the char _after_ the final one printed |

### expStrToDouble()

Convert exponent-format string to double.

```c++
bool expStrToDouble(const char *str, double *value)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `str` | `const char *` | A pointer to the string to be converted |
| `value` | `double *` | A pointer to the double which will hold the result |
| return value | `bool` | ```true``` if the conversion was successful, ```false``` otherwise |

### setPrecision()

Set the precision (number of decimal places) to be used by ```_dtostrf``` and ```_etoa```.

Set this _before_ calling ```mySensors.detectSensors();```:

* ```mySensors._sprintf.setPrecision(5);```

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
