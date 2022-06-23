# API Reference for the SFE_QUAD_Sensor class

Methods to allow the sensor's underlying Arduino Library be accessed in a homogeneous way:

- Return the sensor's name
- Return which I2C addresses are valid for this sensor
- Detect the sensor
- Begin the sensor (usually by calling the underlying library's ```begin``` method)
- Initialize the sensor (if required)
- Set a pointer to custom initialization code for this sensor
- Return the number of senses this sensor has
- Return the names of the senses
- Return the maximum length of the sense names (to aid menu formatting)
- Return a sense reading
- Return a count of the number of settings this sensor has (if any)
- Return the names of the settings
- Return the maximum length of the settings names (to aid menu formatting)
- Return the setting type
- Set a setting
- Return a count of the number of things which can be configured on this sensor
- Return the names of the things which can be configured on this sensor
- Return the maximum length of the configuration item names
- Return the configuration item type
- Return the configuration item
- Set the configuration item

## Brief Overview

The ```SFE_QUAD_Sensor``` class allows that sensor's Arduino Library to be accessed in a homogeneous way.
It provides common methods for all sensor types, to allow ```SFE_QUAD_Sensors``` to read and configure the
sensor without needing to know the underlying library's methods.

Think of ```SFE_QUAD_Sensor``` as a thin layer which sits on top of the Arduino Library and which provides common access methods for it.

A **sensor** can have multiple **senses**. E.g. the BME280 has three : pressure, temperature and humidity.

A sensor _may_ have several things which can be set. These are the **settings**.
E.g. the NAU7802 scale has two : zero offset (```calculateZeroOffset```) and calibration factor (```calculateCalibrationFactor```).

**Configuration items** can be written to and read from storage (SD, EEPROM, LittleFS).
They are used to record the sensor configuration and restore the configuration.
Note: configuration items _may_ or _**may not**_ be the same as the settings.

Individual ```SFE_QUAD_Sensor``` instances are connected in a linked-list. The ```SFE_QUAD_Sensor``` object contains a pointer to the
```_next``` object in the list. The ```_next``` of the final ```SFE_QUAD_Sensor``` in the list is ```NULL```.

The ```SFE_QUAD_Sensors``` object is responsible for creating the linked-list.

The possible (known) sensor types are defined in the ```enum SFEQUADSensorType``` in the ```SFE_QUAD_Sensors``` class.

Sensor detection and initialization will typically be performed as follows:

- For each sensor type in ```enum SFEQUADSensorType```:
    - If any Qwiic Muxes are detected, sensor detection is performed on all ports of all muxes.
    - The possible I2C addresses for the sensor are requested using ```getNumI2cAddresses``` and ```getI2cAddress```.
    - The sensor is detected using ```detectSensor```. This _usually_ calls the sensor's ```begin``` method, but not always.
    - If the sensor is detected, it is added to the linked-list of ```SFE_QUAD_Sensor``` objects. Its I2C address is recorded. If it is connected through a Mux, the Mux address and port are recorded.
    - Once detection is complete, each sensor is initialized using ```initializeSensor```.
    - If a custom initializer has been defined, that is used in place of the standard initializer.

Reading the senses will typically be performed as follows:

- For each sensor in the linked-list:
    - ```getSenseCount``` returns the number of senses this sensor has
    - ```_logSense``` (an array of ```bool```) records if each individual sense is enabled for logging
    - All enabled senses are read using ```getSenseReading```

The sense names can be read using ```getSenseName```. ```getSenseNameMaxLen``` aids menu formatting (space padding).

The class destructor ensures that all memory used by the linked-list is deleted (freed) correctly. This has been fully tested - see Example5 for details.

Most ```SFE_QUAD_Sensor``` methods are ```virtual``` as they need to be redefined by the methods of the individual sensors.
The individual sensor classes (e.g. ```SFE_QUAD_Sensor_ADS122C04```) are derived. They inherit ```SFE_QUAD_Sensor``` and then redefine each method with their own.

## Initialization / Configuration

### getSensorName()

This method returns a pointer to the sensor's name.

```c++
virtual const char *getSensorName(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `const char *` | A pointer to the sensor's name (ASCII text, null-terminated) |

### getNumI2cAddresses()

This method returns the number of I2C addresses this sensor supports.

```c++
virtual uint8_t getNumI2cAddresses(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `uint8_t` | The number of addresses |

### getI2cAddress()

This method returns the I2C address for the index ```address```.

```c++
virtual uint8_t getI2cAddress(uint8_t address)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `address` | `uint8_t` | The address index |
| return value | `uint8_t` | The I2C address |

### detectSensor()

This method attempts to detect the sensor at address ```sensorAddress``` using Wire port ```port```.
This method _usually_, but not always, calls the Arduino Library's ```begin``` method.

```c++
virtual bool detectSensor(uint8_t sensorAddress, TwoWire &port)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `sensorAddress` | `uint8_t` | The sensor's I2C address (may be required by ```begin```) |
| `port` | `TwoWire &` | The Wire port the sensor is connected to (may be required by ```begin```) |
| return value | `bool` | ```true``` if the sensor is detected, ```false``` otherwise |

### beginSensor()

This method calls the sensor's Arduino Library ```begin``` method.

```c++
virtual bool beginSensor(uint8_t sensorAddress, TwoWire &port)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `sensorAddress` | `uint8_t` | The sensor's I2C address (may be required by ```begin```) |
| `port` | `TwoWire &` | The Wire port the sensor is connected to (may be required by ```begin```) |
| return value | `bool` | ```true``` if the sensor is begun successfully, ```false``` otherwise |

### initializeSensor()

Perform any initialization methods this sensor requires. If a custom initializer has been defined, perform that instead.

```c++
virtual bool initializeSensor(uint8_t sensorAddress, TwoWire &port)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `sensorAddress` | `uint8_t` | The sensor's I2C address |
| `port` | `TwoWire &` | The Wire port the sensor is connected to |
| return value | `bool` | ```true``` if the sensor is initialized successfully, ```false``` otherwise |

### setCustomInitializePtr()

Set a pointer to the custom initializer for this sensor.

```c++
void setCustomInitializePtr(void (*pointer)(uint8_t sensorAddress, TwoWire &port, void *_classPtr))
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `pointer` | `void (*)()` | The address of the custom initializer |

The parameters for the custom initializer method are:

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `sensorAddress` | `uint8_t` | The sensor's I2C address (may be required) |
| `port` | `TwoWire &` | The Wire port the sensor is connected to (may be required) |
| `_classPtr` | `void *` | A pointer to the Arduino Library class instance for this sensor (required to access the class methods) |

### getSenseCount()

Return the number of senses this sensor has.

```c++
virtual bool getSenseCount(uint8_t *count)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `count` | `uint8_t` | Pointer to the ```uint8_t``` which will hold the count |
| return value | `bool` | ```true``` if the sensor exists, ```false``` otherwise |

### getSenseName()

This method returns a pointer to the name of the sense with index ```sense```.

```c++
virtual const char *getSenseName(uint8_t sense)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `sense` | `uint8_t` | The sense index |
| return value | `const char *` | A pointer to the sense's name (ASCII text, null-terminated) |

### getSenseNameMaxLen()

Return the maximum length of all sense names for this sensor. Useful for menu formatting (space padding).

```c++
virtual uint8_t getSenseNameMaxLen()
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `uint8_t` | The maximum name length |

### getSenseReading()

Read the sense with index ```sense```. Return the reading as ASCII text in the char array ```reading```.

The calling method is responsible for ensuring ```reading``` can hold the entire reading.

```c++
virtual bool getSenseReading(uint8_t sense, char *reading)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `sense` | `uint8_t` | The sense index |
| `reading` | `char *` | A pointer to a char array to hold the reading (ASCII text, null-terminated) |
| return value | `bool` | ```true``` is the read is successful, ```false``` otherwise |

### getSettingCount()

Return the count of the number of settings this sensor has.

```c++
virtual bool getSettingCount(uint8_t *count)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `count` | `uint8_t` | Pointer to the ```uint8_t``` which will hold the count |
| return value | `bool` | ```true``` if the sensor exists, ```false``` otherwise |

### getSettingName()

This method returns a pointer to the name of the setting with index ```sense```.

```c++
virtual const char *getSettingName(uint8_t sense)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `sense` | `uint8_t` | The setting index |
| return value | `const char *` | A pointer to the setting name (ASCII text, null-terminated) |

### getSettingNameMaxLen()

Return the maximum length of all settings names for this sensor. Useful for menu formatting (space padding).

```c++
virtual uint8_t getSettingNameMaxLen()
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `uint8_t` | The maximum name length |

### getSettingType()

Return the setting type (```SFE_QUAD_Sensor_Setting_Type_e```) (BOOL, FLOAT, INT, etc.) for this setting.

```c++
virtual bool getSettingType(uint8_t setting, SFE_QUAD_Sensor_Setting_Type_e *type)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `setting` | `uint8_t` | The setting index |
| `type` | `SFE_QUAD_Sensor_Setting_Type_e *` | Pointer to the enum which will hold the setting type |
| return value | `bool` | ```true``` if the sensor exists, otherwise ```false``` |

### setSetting()

Apply the value in ```value``` to the sensor setting with index ```setting```.

```c++
virtual bool setSetting(uint8_t setting, SFE_QUAD_Sensor_Every_Type_t *value)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `setting` | `uint8_t` | The setting index |
| `value` | `SFE_QUAD_Sensor_Setting_Every_Type_t *` | Pointer to the struct containing the setting |
| return value | `bool` | ```true``` if successful, otherwise ```false``` |

### getConfigurationItemCount()

Return the count of the number of configuration items this sensor has.
Configuration items are settings which need to be written to and read from storage media.

```c++
virtual bool getConfigurationItemCount(uint8_t *count)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `count` | `uint8_t` | Pointer to the ```uint8_t``` which will hold the count |
| return value | `bool` | ```true``` if the sensor exists, ```false``` otherwise |

### getConfigurationItemName()

This method returns a pointer to the name of the configuration item with index ```configItem```.

```c++
virtual const char *getConfigurationItemName(uint8_t configItem)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `configItem` | `uint8_t` | The configuration item index |
| return value | `const char *` | A pointer to the configuration item name (ASCII text, null-terminated) |

### getConfigurationItemNameMaxLen()

Return the maximum length of all configuration item names for this sensor. Useful for menu formatting (space padding).

```c++
virtual uint8_t getConfigurationItemNameMaxLen()
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `uint8_t` | The maximum name length |

### getConfigurationItemType()

Return the type (```SFE_QUAD_Sensor_Setting_Type_e```) (BOOL, FLOAT, INT, etc.) for this configuration item.

```c++
virtual bool getConfigurationItemType(uint8_t configItem, SFE_QUAD_Sensor_Setting_Type_e *type)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `configItem` | `uint8_t` | The configuration item index |
| `type` | `SFE_QUAD_Sensor_Setting_Type_e *` | Pointer to the enum which will hold the configuration type |
| return value | `bool` | ```true``` if the sensor exists, otherwise ```false``` |

### getConfigurationItem()

Read the sensor configuration item with index ```configItem``` and store it in ```value```.

```c++
virtual bool getConfigurationItem(uint8_t configItem, SFE_QUAD_Sensor_Every_Type_t *value)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `configItem` | `uint8_t` | The configuration item index |
| `value` | `SFE_QUAD_Sensor_Setting_Every_Type_t *` | Pointer to the struct which will hold the value |
| return value | `bool` | ```true``` if successful, otherwise ```false``` |

### setConfigurationItem()

Apply the value in ```value``` to the sensor configuration item with index ```configItem```.

```c++
virtual bool setConfigurationItem(uint8_t configItem, SFE_QUAD_Sensor_Every_Type_t *value)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `configItem` | `uint8_t` | The configuration item index |
| `value` | `SFE_QUAD_Sensor_Setting_Every_Type_t *` | Pointer to the struct containing the value |
| return value | `bool` | ```true``` if successful, otherwise ```false``` |

## Helper Methods

### deleteSensorStorage()

This method is responsible for deleting (freeing) all dynamic memory used by the ```SFE_QUAD_Sensor``` object. It is called by the destructor.

```c++
virtual void deleteSensorStorage(void)
```

## Member Variables

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `_sensorAddress` | `uint8_t` | The I2C address of this sensor |
| `_muxAddress` | `uint8_t` | The I2C address of the mux this sensor is connected to, 0 is none |
| `_muxPort` | `uint8_t` | The mux port number this sensor is connected to, 0 if none |
| `_classPtr` | `void *` | A pointer to the Arduino Library class instance for this sensor |
| `_next` | `SFE_QUAD_Sensor *` | A pointer to the next sensor in the linked-list |
| `_logSense` | `bool *` | A dynamic array of ```bool``` indicating if individual senses are enabled for logging |
| `_customInitializePtr` | `void (*)()` | A pointer to the custom initializer for this sensor, ```NULL``` if none |
| `_sprintf` | `SFE_QUAD_Sensors_sprintf` | An instance of ```SFE_QUAD_Sensors_sprintf``` to aid printing of double and exponent data |

## Data Types

### SFE_QUAD_Sensor_Every_Type_t

As the name suggests, this structure contains one of every data type used by the underlying Arduino Libraries.
It allows values to be passed between classes in a homogeneous, non-type-specific way.
The data type is defined by the associated ```SFE_QUAD_Sensor_Setting_Type_e``` enum.

```c++
    typedef struct
    {
        bool BOOL;
        float FLOAT;
        double DOUBLE;
        int INT;
        uint8_t UINT8_T;
        uint16_t UINT16_T;
        uint32_t UINT32_T;
    } SFE_QUAD_Sensor_Every_Type_t;
```

| Member | Type | Description |
| :-------- | :--- | :---------- |
| `BOOL` | `bool` | A ```bool``` |
| `FLOAT` | `float` | A ```float``` |
| `DOUBLE` | `double` | A ```double``` |
| `INT` | `int` | An ```int``` |
| `UINT8_T` | `uint8_t` | A ```uint8_t``` |
| `UINT16_T` | `uint16_t` | A ```uint16_t``` |
| `UINT32_T` | `uint32_t` | A ```uint32_t``` |

