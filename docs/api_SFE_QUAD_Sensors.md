# API Reference for the SFE_QUAD_Sensors class

Methods to setup, configure and query the ```SFE_QUAD_Sensor``` object:

- Return a pointer to the specified sensor class so it can be added to the linked-list of sensors
- Define which Wire port will be used
- Define which Serial port will be used for the built-in menus
- Detect which sensors are attached
- Begin those sensors
- Initialize those sensors (if required)
- Get the sense readings from all enabled sensors
- Get the names of all enabled sensors
- Get the names of all enabled senses
- Define a custom initializer for a sensor (if required / desired)
- Determine if a sensor exists
- Open the logging menu
- Open the setting menu
- Get the sensor and menu configuration in text format
- Apply configuration settings to the sensors and menus

## Brief Overview

The ```SFE_QUAD_Sensors``` class allows a linked-list of individual ```SFE_QUAD_Sensor``` objects to be generated and maintained.

A ```SFE_QUAD_Sensors``` object contains a pointer to a ```SFE_QUAD_Sensor``` named ```_head```.
```_head``` the the head (start) of a linked-list of ```SFE_QUAD_Sensor``` objects.

```_head``` is ```NULL``` initially. After calling ```detectSensors```, it contains the address of the first ```SFE_QUAD_Sensor``` in the linked-list.

Each ```SFE_QUAD_Sensor``` contains a pointer named ```_next``` which points to the next ```SFE_QUAD_Sensor``` in the linked-list.

The ```_next``` of the final ```SFE_QUAD_Sensor``` in the list is ```NULL```.

The possible (known) sensor types are defined in the ```enum SFEQUADSensorType```.

```detectSensors``` discovers which individual sensors are attached on the selected Wire port and adds them to the linked-list.
```detectSensors``` has built-in Qwiic Mux support and will discover all muxes, and all sensors connected to the ports on those muxes.

```beginSensors``` will call the ```.begin``` method of each attached sensor in turn.

```initializeSensors``` will perform any additional initialization (if any) required by those sensors.

It is possible to override the initialization code for each sensor type, or individual sensor objects, by calling ```setCustomInitialize```.
(You must call ```setCustomInitialize``` _before_ ```initialzeSensors```.)

```loggingMenu``` and ```settingMenu``` are methods which open built-in menus to:

- Configure which senses on each sensor are enabled for logging
- Call any settings methods the sensors may have (if any)

The sense readings are stored in a dynamic char array named ```readings```. The enabled senses are read with ```getSensorReadings```.
Memory for ```readings``` is allocated dynamically (using ```new```). The memory is freed automatically by the class destructor.
The class has been checked for memory leaks - see Example5.

The names of all enabled sensors, together with their I2C and Mux addresses (if any), can be read with ```getSensorNames```. The names are returned in ```readings```.

The names of all enabled senses can be read with ```getSenseNames```. The names are returned in ```readings```.

The sensor and menu configuration can be stored temporarily in a dynamic char array named ```configuration```.
The text CSV ```configuration``` is written to storage and read from storage by the individual classes for EEPROM, LittleFS, SD and SdFat:

- ```SFE_QUAD_Sensors__EEPROM```
- ```SFE_QUAD_Sensors__LittleFS```
- ```SFE_QUAD_Sensors__SD```
- ```SFE_QUAD_Sensors__SdFat```

Those classes provide additional methods named ```writeConfigurationToStorage```, ```readConfigurationFromStorage``` etc. which write and read
```configuration``` to and from the appropriate storage medium.

## Initialization / Configuration

### setWirePort()

This method is called to set the I2C Wire (```TwoWire```) port to which the sensors are connected.

```c++
void setWirePort(TwoWire &port)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `port` | `TwoWire &` | The Wire port |

### enableDebugging()

This method is called to enable debugging messages on the chosen Stream (usually a Serial port).

```c++
void enableDebugging(Stream &port)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `port` | `Stream &` | The Stream (Serial port) |

There is no method to disable the debug messages. The messages can be disabled by setting ```_printDebug``` to false:

```c++
mySensors._printDebug = false;
```

This method also enables debug messages on the ```theMenu``` object. The menu debug messages can be changed / disabled with the ```theMenu.setDebugPort``` method.

### setMenuPort()

This method sets the Stream (usually a Serial port) for the built-in menus.

```c++
void setMenuPort(Stream &port)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `port` | `Stream &` | The Stream (Serial port) |

## Sensor Factory

### sensorFactory()

This method is used internally by the ```detectSensors``` method. It returns a pointer to a new instance of a ```SFE_QUAD_Sensor``` for the selected type.

```c++
SFE_QUAD_Sensor *sensorFactory(SFEQUADSensorType type)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `type` | `SFEQUADSensorType` | The enumerated type of the sensor |
| return value | `SFE_QUAD_Sensor *` | A pointer to the new sensor instance |

## Sensor Detection / Initialization

### detectSensors()

This method discovers which individual sensors are attached on the selected Wire port.
It has built-in Qwiic Mux support and will discover all muxes, and all sensors connected to the ports on those muxes.

The detected sensors are stored internally as a linked-list, pointed to by ```_head```.

```c++
bool detectSensors(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```false``` if the Wire port is not defined or a memory-allocation error occurred, otherwise ```true``` |

### beginSensors()

This method begins all detected sensors using each sensor's individual ```.begin``` method.

```c++
bool beginSensors(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```false``` if no sensors have been detected or a memory-allocation error occurred, otherwise ```true``` |

### initializeSensors()

This method initializes any detected sensors if required:

- only if the library contains initialization code for that sensor type
- and/or a custom initializer has been defined for that individual sensor or sensor type

```c++
bool initializeSensors(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```false``` if no sensors have been detected or a memory-allocation error occurred, otherwise ```true``` |

### setCustomInitialize()

This method defines custom initialization code for all instances of ```sensorName```.

```c++
bool setCustomInitialize(void (*pointer)(uint8_t sensorAddress, TwoWire &port, void *_classPtr), const char *sensorName)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `pointer` | `void (*)()` | The address of the custom initialization method |
| `sensorName` | `const char *` | The name of the sensor type |
| return value | `bool` | ```false``` if no sensors have been detected, otherwise ```true``` |

The parameters for the custom initializer method are:

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `sensorAddress` | `uint8_t` | The I2C address of this sensor |
| `port` | `TwoWire &` | The Wire port this sensor is connected to |
| `_classPtr` | `void *` | A pointer to the class for this sensor type |
| return value | `bool` | ```false``` if no sensors have been detected, otherwise ```true``` |

Please see Example4_CustomInitialization for more details.

### setCustomInitialize()

This method defines custom initialization code for the instance of ```sensorName``` at the specified mux address and port.

```c++
bool setCustomInitialize(void (*pointer)(uint8_t sensorAddress, TwoWire &port, void *_classPtr), const char *sensorName, uint8_t i2cAddress, uint8_t muxAddress, uint8_t muxPort)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `pointer` | `void (*)()` | The address of the custom initialization method |
| `sensorName` | `const char *` | The name of the sensor type |
| `i2cAddress` | `uint8_t` | The I2C address of the target sensor |
| `muxAddress` | `uint8_t` | The I2C address of the mux the sensor is connected to. The default value is 0 (no mux) |
| `muxPort` | `uint8_t` | The mux port the sensor is connected to. The default value is 0 (no mux) |
| return value | `bool` | ```false``` if no sensors have been detected, otherwise ```true``` |

The parameters for the custom initializer method are:

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `sensorAddress` | `uint8_t` | The I2C address of this sensor |
| `port` | `TwoWire &` | The Wire port this sensor is connected to |
| `_classPtr` | `void *` | A pointer to the class for this sensor type |

Please see Example4_CustomInitialization for more details.

## Sensor Names and Readings

### getSensorReadings()

This method collects the readings from all enabled senses on all enabled sensors. The readings are returned in the dynamic
char array ```readings``` in CSV format.

```c++
bool getSensorReadings(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```false``` if no sensors have been detected or a memory-allocation error occurred, otherwise ```true``` |

### getSensorNames()

This method collects the names of all enabled sensors (for all enabled senses). The names are returned in the dynamic
char array ```readings``` in CSV format.

```c++
bool getSensorNames(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```false``` if no sensors have been detected or a memory-allocation error occurred, otherwise ```true``` |

### getSenseNames()

This method collects the names of all enabled senses (for all enabled sensors). The names are returned in the dynamic
char array ```readings``` in CSV format.

```c++
bool getSenseNames(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```false``` if no sensors have been detected or a memory-allocation error occurred, otherwise ```true``` |

## Menus

### loggingMenu()

This method opens the logging menu on the specified Stream (Serial port) to set the configuration of which sensors and senses are
enabled or disabled for logging.

```c++
bool loggingMenu(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```false``` if no sensors have been detected or the Stream is undefined, otherwise ```true``` (when the menu is closed) |

### settingMenu()

This method opens the setting menu on the specified Stream (Serial port) to apply settings to any sensors which have them.

```c++
bool settingMenu(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```false``` if no sensors have been detected or the Stream is undefined or the detected sensors have no settings, otherwise ```true``` (when the menu is closed) |

## Sensor and Menu Configuration

### getSensorAndMenuConfiguration()

This method assembles the combined sensor and menu configuration in text CSV format so it can be written to storage media by the appropriate class.

The configuration is returned in the dynamic char array ```configuration```.

```c++
bool getSensorAndMenuConfiguration(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```false``` if no sensors or menu items are found or a memory-allocation error occurred, otherwise ```true``` |

### applySensorAndMenuConfiguration()

This method applies the configuration in the dynamic char array ```configuration``` to the sensors and menu linked-list.

The configuration must be read from storage media by the appropriate class before ```applySensorAndMenuConfiguration``` is called.

The configuration is stored in media in text CSV format.

```c++
bool applySensorAndMenuConfiguration(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```false``` if no sensors are found or a memory-allocation error occurred, otherwise ```true``` |

## Helper Methods

### sensorExists()

This method steps through the sensor linked-list, starting at ```_head```, checking if the specified sensor exists.
If it does, it returns a pointer to its ```SFE_QUAD_Sensor``` instance.

```c++
SFE_QUAD_Sensor *sensorExists(const char *sensorName, uint8_t i2cAddress, uint8_t muxAddress, uint8_t muxPort)
```
| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `sensorName` | `const char *` | The name of the sensor type |
| `i2cAddress` | `uint8_t` | The I2C address of the target sensor |
| `muxAddress` | `uint8_t` | The I2C address of the mux the sensor is connected to. The default value is 0 (no mux) |
| `muxPort` | `uint8_t` | The mux port the sensor is connected to. The default value is 0 (no mux) |
| return value | `SFE_QUAD_Sensor *` | A pointer to the ```SFE_QUAD_Sensor``` instance, ```NULL``` otherwise |

### getMenuChoice()

This method is used by ```settingMenu``` and ```loggingMenu``` to select one of the menu items. 

```c++
uint32_t getMenuChoice(unsigned long timeout)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `timeout` | `unsigned long` | The menu timeout in milliseconds |
| return value | `uint32_t` | The menu choice (>= 1). 0 if the timeout expires |

### getSettingValueDouble()

This method is used by ```settingMenu``` and ```loggingMenu```. The user enters a ```double``` value. Exponent-format entries are accepted.
(```settingMenu``` and ```loggingMenu``` will cast the ```double``` to the required type.)

```c++
bool getSettingValueDouble(double *value, unsigned long timeout)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `value` | `double *` | A pointer to the ```double``` to hold the value |
| `timeout` | `unsigned long` | The menu timeout in milliseconds |
| return value | `bool` | ```false``` if no input is received or the Stream is undefined, otherwise ```true``` |

## Member Variables

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `_head` | `SFE_QUAD_Sensor *` | The head (start) of the linked-list of ```SFE_QUAD_Sensor``` objects |
| `readings` | `char *` | Pointer to a dynamic char array which holds the sensor readings, names or sense names in CSV format |
| `configuration` | `char *` | Pointer to a dynamic char array which holds the sensor and menu configuration |
| `_printDebug` | `bool` | ```true``` is debug messages are to be printed to ```_debugPort```, ```false``` otherwise |
| `_i2cPort` | `TwoWire *` | Pointer to the Wire port for I2C communication |
| `_menuPort` | `Stream *` | Pointer to the Stream (Serial port) for the built-in menus |
| `_debugPort` | `Stream *` | Pointer to the Stream (Serial port) for the debug messages (if enabled) |
| `theMenu` | `SFE_QUAD_Menu` | Instance of ```SFE_QUAD_Menu``` which can be used to create additional menus |
| `_sprintf` | `SFE_QUAD_Sensors_sprintf` | Instance of ```SFE_QUAD_Sensors_sprintf``` to aid printing of doubles and exponent-format data |
