# Adding a New Sensor

Adding a new sensor is easy! Here is a summary of the steps:

* Create an _**SFE_QUAD_Sensor_NewSensorName.h**_ file for it, using the existing files as a template
  * You will find the individual sensor header files in the library [src/src](../src/src) sub-folder
* Edit [SFE_QUAD_Sensors.h](../src/SFE_QUAD_Sensors.h) :
  * Add a _**#define INCLUDE_SFE_QUAD_SENSOR_NewSensorName**_ for the new sensor - in case the user wants to select which sensors to include
  * Add a new entry for the sensor in _**enum SFEQUADSensorType**_ (3 lines for each new sensor)
  * Add a new entry for the sensor in _**SFE_QUAD_Sensor <b>*</b>sensorFactory**_ (4 lines for each new sensor)
* Edit [SFE_QUAD_Headers.h](../src/SFE_QUAD_Headers.h) :
  * Add a new entry for the sensor (3 lines for each new sensor)
* Edit [.github/workflows/compile-sketch.yml](../.github/workflows/compile-sketch.yml) :
  * Add a _**- name: Update NewSensorName**_ entry for the new sensor
  * This automates copying the Arduino Library files into the [src/src](../src/src) sub-folder
  * The latest versions of the library .h. and .cpp will be copied and added to a GitHub pull request automatically

_That's all folks!_

A description of each step is included below:

## SFE_QUAD_Sensor_NewSensorName.h

The **SFE_QUAD_Sensor_NewSensorName.h** file is the interface between the SparkFun Qwiic Universal Auto-Detect **SFE_QUAD_Sensor** class
and the underlying Arduino Library for that sensor.

One of the requirements for SparkFun Qwiic Universal Auto-Detect was to be able to use the existing Arduino Library for any sensor
_as-is_ and without modification. The header file provides a standard set of methods to allow the sensor's senses to be: configured; enabled; and read.

You can use the existing **SFE_QUAD_Sensor_SensorName.h** files as a template for the new sensor.

If you are adding support for a 'simple' sensor which has no settings and requires no configuration, then the [BME280 header file](../src/src/SFE_QUAD_Sensor_BME280.h)
is a good choice.

For a more complex sensor, the [VL53L1X header file](../src/src/SFE_QUAD_Sensor_VL53L1X.h) is a good starting point.

### Compiler Guard

The first two lines of the header file are a _compiler guard_. They prevent the file from being included more than once when the code compiles.

The name used in the `#ifndef` and `#define` must be globally unique.

Replace the (e.g.) **BME280** with the name of your new sensor.
If you are adding a new sensor called **FOO**, the first two lines become:

```c++
#ifndef SPARKFUN_QUAD_SENSOR_HEADER_FOO_H // <=== Update this with the new sensor type
#define SPARKFUN_QUAD_SENSOR_HEADER_FOO_H // <=== Update this with the new sensor type
```

### Header File Name

Now is a good time to save the header file using its new name. In this case the file will be saved as: **src/src/SFE_QUAD_Sensor_FOO.h**

### #include the Sensor Library Header file

Line 4 includes the header file of the Arduino Library for the sensor.

The Arduino Library files will be copied into a sub-folder called **src/src/FOO**.
You can, if you wish, create the **FOO** sub-folder and add the library **.h** and **.cpp** files manually.
This will allow the code to compile while you are testing it.
However, the [.github/workflows/compile-sketch.yml](./.github/workflows/compile-sketch.yml) file will do this for you automatically once you have edited it.
**compile-sketch.yml** also ensures that the copy of the Arduino Library stays up to date. Any changes to the main Arduino Library are automatically merged into this library
whenever changes are pushed.
See [below](#update-compile-sketch-yml) for details.

Change line 4 so it will include the library header file correctly.

```c++
#include "FOO/FOO.h" // <=== Update this with the new sensor library header file
```

!!! note:
  Always enclose the include file folder and name in double quotes. Do not use less-than and greater-than.
  This ensures that the copy of the library in the **src/src/FOO** sub-folder is included, not a copy pointed to by the Arduino IDE path.

### CLASSNAME and CLASSTITLE

**CLASSNAME** is the name of the sensor class as defined in its Arduino Library. Open the Arduino Library header file to find the name of the class. Copy and paste the name into the **#define CLASSNAME**.

**CLASSTITLE** is how the sensor class is referred to within Qwiic Universal Auto-Detect. Take the **CLASSNAME** and prefix it with **SFE_QUAD_Sensor_** to form the **CLASSTITLE**:

```c++
#define CLASSNAME FOO // <=== Update this with the new sensor type

#define CLASSTITLE SFE_QUAD_Sensor_FOO // <=== Update this with the new sensor type
```

### SENSE_COUNT

The **SENSE_COUNT** is the number of senses this sensor has. A sensor can have more than one sense. E.g. the BME280 has three: Pressure, Temperature and Humidity.
Update the **SENSE_COUNT** with the number of senses:

```c++
#define SENSE_COUNT 3 // <=== Update this with the number of things this sensor can sense
```

### SETTING_COUNT and CONFIGURATION_ITEM_COUNT

The **SETTING_COUNT** is the number of things which can be set on this sensor. _**Settings**_ are set one at a time via the built-in setting menu ```settingMenu```.

Sensors can also have one or more _**Configuration Items**_. These are _**Settings**_ which need to be stored in storage media (SD, EEPROM, LittleFS) so they can be read and applied easily.

_**Settings**_ are applied manually and individually via the **settingMenu**. _**Configuration Items**_  are applied all together with a single call of ```applySensorAndMenuConfiguration```.

_**Settings**_ are _usually_ also _**Configuration Items**_, but not always. The number of _**Settings**_ is usually equal to, or greater than, the number of _**Configuration Items**_, but not always.

A simple sensor, like the BME280, has zero settings and zero configuration items.

A more complex sensor will have one or more settings and configuration items.

Change the definitions to match the new sensor:

```c++
#define SETTING_COUNT 0 // <=== Update this with the number of things that can be set on this sensor

#define CONFIGURATION_ITEM_COUNT 0 // <=== Update this with the number of things that can be configured on this sensor
```

### Settings vs. Configuration Items

The VL53L1X has five settings:
* Distance Mode: Short
* Distance Mode: Long
* Intermeasurement Period
* Crosstalk
* Offset

But it only has four configuration items requiring storage:
* Distance Mode
* Intermeasurement Period
* Crosstalk
* Offset

We do it this way so that the user can change the distance mode with a single key press.

We could have used a single distance mode ```BOOL``` setting, representing Short vs. Long, but:
* The user would have had to select the distance mode setting
* Then enter a valid ```BOOL``` (0 or 1)
* The code in ```setSetting``` would have had to validate the choice before applying it

By using two ```NONE``` choices, we both make things easier for the user and simplify the code.

This is discussed again in [setSetting](#setsetting) below.

### SENSOR_I2C_ADDRESSES

Sensors usually only have one I<sup>2</sup>C address, but sometimes can have multiple addresses.
**SENSOR_I2C_ADDRESSES** is an array containing all the valid addresses for this sensor.
```detectSensors``` will check each address consecutively when detecting which senors are attached.

Update **SENSOR_I2C_ADDRESSES** with the valid addresses for the new sensor:

```c++
#define SENSOR_I2C_ADDRESSES const uint8_t sensorI2cAddresses[] = {0x76, 0x77} // <=== Update this with the I2C addresses for this sensor
```

### detectSensor

At the simplest level, we can detect if a sensor is attached by checking if its I<sup>2</sup>C address is acknowledged.

A simple I<sup>2</sup>C _port scanner_ will scan through all valid addresses checking for an acknowledgement:

```c++
for (int i = 1; i < 127; i++)
{
  port.beginTransmission(i);
  if (port.endTransmission() == 0)
  {
    Serial.print("Something detected at address 0x");
    Serial.println(i, HEX);
  }
}
```

However:
* Getting an acknowledgement does not tell us what type of sensor was detected at that address, just that _something_ was detected
* The ```beginTransmission``` + ```endTransmission``` test can cause some sensors to produce errors

A better way is to use the sensor's ```begin``` method. However, the ```begin``` can sometimes take a long time to complete if no sensor is connected.

For the BME280, we use the ```beginTransmission``` + ```endTransmission``` test as it gives a fast indication of whether a device is connected (```beginI2C``` is slow if nothing is connected).
Followed by its ```beginI2C``` method for full confidence that we are detecting a BME280.

The BME280 has two valid I2C addresses, so we need to tell the code which address to use with the ```device->setI2CAddress(sensorAddress);```.

```c++
  // Detect the sensor. ===> Adapt this to match the sensor type <===
  bool detectSensor(uint8_t sensorAddress, TwoWire &port)
  {
    port.beginTransmission(sensorAddress); // Scan the sensor address first. beginI2C takes a long time if no device is connected
    if (port.endTransmission() == 0)
    {
      CLASSNAME *device = (CLASSNAME *)_classPtr;
      device->setI2CAddress(sensorAddress);
      return (device->beginI2C(port));
    }
    else
      return (false);
  }
```

The VL53L1X's ```detectSensor``` is slightly simpler, but again we use the ```beginTransmission``` + ```endTransmission``` test for speed:

```c++
  // Detect the sensor. ===> Adapt this to match the sensor type <===
  bool detectSensor(uint8_t sensorAddress, TwoWire &port)
  {
    port.beginTransmission(sensorAddress); // Scan the sensor address first
    if (port.endTransmission() == 0)
    {
      CLASSNAME *device = (CLASSNAME *)_classPtr;
      return (device->begin() == 0);
    }
    else
      return (false);
  }
```

Adapt the template code to match the new sensor.

### beginSensor

Sensors almost always require their ```begin``` method to be called before communication can take place. Again, you should adapt the template code to match the new sensor.
The VL53L1X's ```begin``` method returns **0** when the device is begun successfully, so its ```beginSensor``` is:

```c++
  // Begin the sensor. ===> Adapt this to match the sensor type <===
  bool beginSensor(uint8_t sensorAddress, TwoWire &port)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    return (device->begin() == 0);
  }
```

The BME280 returns ```true``` when successful, so its ```beginSensor``` is:

```c++
  // Begin the sensor. ===> Adapt this to match the sensor type <===
  bool beginSensor(uint8_t sensorAddress, TwoWire &port)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    device->setI2CAddress(sensorAddress);
    return (device->beginI2C(port));
  }
```

The sensor has already been detected, so you do not need to 'scan' the address again, but you do need to call ```begin``` again here.
The ```begin``` in ```detectSensor``` is 'lost' as the sensor has not been added to the linked-list of sensors at that point.

### initializeSensor

Each sensor is initialized - if required - when ```mySensors.initializeSensors();``` is called.

Some sensors - like the BME280 - don't require initialization. So its ```initializeSensor``` is essentially 'empty'. Unless a custom initializer
has been defined (see Example4), it simply returns ```true```. If a custom initializer has been defined, then that is called before returning ```true```.

```c++
  // Initialize the sensor. ===> Adapt this to match the sensor type <===
  bool initializeSensor(uint8_t sensorAddress, TwoWire &port)
  {
    if (_customInitializePtr == NULL) // Has a custom initialize function been defined?
    {
      return (true);
    }
    else
    {
      _customInitializePtr(sensorAddress, port, _classPtr); // Call the custom initialize function
      return (true);
    }
  }
```

The VL53L1X does require initialization. As a minimum, we need to set the distance mode and instruct it to ```startRanging```:

```c++
  // Initialize the sensor. ===> Adapt this to match the sensor type <===
  bool initializeSensor(uint8_t sensorAddress, TwoWire &port)
  {
    if (_customInitializePtr == NULL) // Has a custom initialize function been defined?
    {
      CLASSNAME *device = (CLASSNAME *)_classPtr;
      if (_shortDistanceMode)
        device->setDistanceModeShort();
      else
        device->setDistanceModeLong();
      device->startRanging();
      return (true);
    }
    else
    {
      _customInitializePtr(sensorAddress, port, _classPtr); // Call the custom initialize function
      return (true);
    }
  }
```

```_shortDistanceMode``` is an extra bool member variable we've added to the sensor class. It is initialized to ```true``` when the sensor object is created and added to the linked list:

```c++
  bool _shortDistanceMode;

  CLASSTITLE(TwoWire &port)
  {
    _sensorAddress = 0;
    _muxAddress = 0;
    _muxPort = 0;
    _classPtr = new CLASSNAME(port);
    _next = NULL;
    _logSense = new bool[SENSE_COUNT + 1];
    for (size_t i = 0; i <= SENSE_COUNT; i++)
      _logSense[i] = true;
    _customInitializePtr = NULL;
    _shortDistanceMode = true;
  }
```

```_shortDistanceMode```  records or indicates whether the sensor is in short or long distance mode.

Why did we do it this way? Couldn't we have used a ```BOOL``` configuration item for it instead? Yes, we could have done it that way, but, as we explained above:
* The user would have had to select the distance mode setting
* Then enter a valid ```BOOL``` (0 or 1)
* The code in ```setSetting``` would have had to validate the choice before applying it

By using two ```NONE``` choices, and storing the choice in ```_shortDistanceMode```, we both make things easier for the user and simplify the code.

### getSenseName

```getSenseName``` returns the name of each sense as ```const char *``` as it will appear in ```loggingMenu```.

The number of ```case``` statements must match **SENSE_COUNT**. Adapt the template code
to match the number of senses for the new sensor; add or remove ```case``` statements as necessary.

```c++
  // Return the name of the name of the specified sense. ===> Adapt this to match the sensor type <===
  const char *getSenseName(uint8_t sense)
  {
    switch (sense)
    {
    case 0:
      return ("Pressure (Pa)");
      break;
    case 1:
      return ("Temperature (C)");
      break;
    case 2:
      return ("Humidity (%)");
      break;
    default:
      return (NULL);
      break;
    }
    return (NULL);
  }
```

### getSenseReading

```getSenseReading``` is the method which calls the appropriate 'read' method for the selected ```sense```.
It is called by ```getSensorReadings```.

```getSenseReading``` calls the Arduino Library method to read that sense and converts the reading into text format:

```c++
  // Return the specified sense reading as text. ===> Adapt this to match the sensor type <===
  bool getSenseReading(uint8_t sense, char *reading)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (sense)
    {
    case 0:
      _sprintf._dtostrf((double)device->readFloatPressure(), reading); // Get the pressure
      return (true);
      break;
    case 1:
      _sprintf._dtostrf((double)device->readTempC(), reading); // Get the temperature
      return (true);
      break;
    case 2:
      _sprintf._dtostrf((double)device->readFloatHumidity(), reading); // Get the humidity
      return (true);
      break;
    default:
      return (false);
      break;
    }
    return (false);
  }
```

Again, the number of ```case``` statements must match **SENSE_COUNT**. And, of course, the order of the ```case``` statements must
be the same as ```getSenseName```.

Looking closely at the code for the **Pressure** (sense 0):

```_sprintf._dtostrf((double)device->readFloatPressure(), reading);```

The code is:
* Calling the Arduino Library ```readFloatPressure()``` method, using the ```_classPtr```
* The result is being cast to double
* ```_sprintf._dtostrf``` is a helper function from the ```SFE_QUAD_Sensors_sprintf``` class which converts the double to text
  * ```sprintf``` is not supported correctly on all platforms (Artemis / Apollo3 especially) so we added the helper method to the sensor class to ensure doubles are always converted to text correctly
* The text is copied into the char array ```reading```

If you like **exponent-format**, there is an additional helper function named ```_sprintf._etoa``` which will convert a double to exponent-format text.

```getSensorReadings``` pieces the text readings together in CSV format and retruns them in ```readings```.

If the sense methods return an integer (instead of float or double), then ```getSensorReadings``` does use ```sprintf``` to print the reading as text:

```c++
  // Return the specified sense reading as text. ===> Adapt this to match the sensor type <===
  bool getSenseReading(uint8_t sense, char *reading)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (sense)
    {
    case 0:
      sprintf(reading, "%d", device->getDistance());
      return (true);
      break;
    case 1:
      sprintf(reading, "%d", device->getRangeStatus());
      return (true);
      break;
    case 2:
      sprintf(reading, "%d", device->getSignalRate());
      return (true);
      break;
    default:
      return (false);
      break;
    }
    return (false);
  }
```

### getSettingName

Simple sensors, like the BME280, have no settings or configuration items. ```getSettingName``` simply returns ```NULL```.

```c++
  // Return the name of the name of the specified setting. ===> Adapt this to match the sensor type <===
  const char *getSettingName(uint8_t setting)
  {
    switch (setting)
    {
    default:
      return (NULL);
      break;
    }
    return (NULL);
  }
```

For the VL53L1X, ```getSettingName``` returns the name of each setting as it will appear in ```settingMenu```:

```c++
  // Return the name of the name of the specified setting. ===> Adapt this to match the sensor type <===
  const char *getSettingName(uint8_t setting)
  {
    switch (setting)
    {
    case 0:
      return ("Distance Mode: Short");
      break;
    case 1:
      return ("Distance Mode: Long");
      break;
    case 2:
      return ("Intermeasurement Period");
      break;
    case 3:
      return ("Crosstalk");
      break;
    case 4:
      return ("Offset");
      break;
    default:
      return (NULL);
      break;
    }
    return (NULL);
  }
```

The number of ```case``` statements must match **SETTING_COUNT**.

### getSettingType

```getSettingType``` returns the ```SFE_QUAD_Sensor_Setting_Type_e``` data type for the setting.

If the sensor has no settings (**SETTING_COUNT** is zero), then ```getSettingType``` simply returns false:

```c++
  // Return the type of the specified setting. ===> Adapt this to match the sensor type <===
  bool getSettingType(uint8_t setting, SFE_QUAD_Sensor_Setting_Type_e *type)
  {
    switch (setting)
    {
    default:
      return (false);
      break;
    }
    return (true);
  }
```

But if there are settings, it returns the data type which matches the type required by the Arduino Library setting method:

```c++
  // Return the type of the specified setting. ===> Adapt this to match the sensor type <===
  bool getSettingType(uint8_t setting, SFE_QUAD_Sensor_Setting_Type_e *type)
  {
    switch (setting)
    {
    case 0:
    case 1:
      *type = SFE_QUAD_SETTING_TYPE_NONE;
      break;
    case 2:
    case 3:
    case 4:
      *type = SFE_QUAD_SETTING_TYPE_UINT16_T;
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }
```

The ```SFE_QUAD_Sensor_Setting_Type_e``` setting types are:

```c++
    SFE_QUAD_SETTING_TYPE_NONE
    SFE_QUAD_SETTING_TYPE_BOOL
    SFE_QUAD_SETTING_TYPE_FLOAT
    SFE_QUAD_SETTING_TYPE_DOUBLE
    SFE_QUAD_SETTING_TYPE_INT
    SFE_QUAD_SETTING_TYPE_UINT8_T
    SFE_QUAD_SETTING_TYPE_UINT16_T
    SFE_QUAD_SETTING_TYPE_UINT32_T
```

All except ```NONE``` are self explanatory. If the Arduino Library setting method requires an ```int``` then set ```*type``` to ```SFE_QUAD_SETTING_TYPE_INT```. Etc..

If the setting type is anything other than ```NONE```, the ```settingMenu``` will call ```getSettingValueDouble```
and cast the result to the appropriate type before calling ```setSetting```.

The ```NONE``` type has no value, it simply causes the menu to do something when that menu option is selected.

Looking at the code above, the ```NONE``` type is used for settings 0 and 1: "Distance Mode: Short" and "Distance Mode: Long".
The matching code in ```setSetting``` then does something without needing a value from ```getSettingValueDouble```.

For the other three cases, a ```uint16_t``` will be passed to ```setSetting``` since that is what the Arduino Library methods require.

### setSetting

If the sensor has no settings (**SETTING_COUNT** is zero), then ```setSetting``` simply returns false:

```c++
  // Set the specified setting. ===> Adapt this to match the sensor type <===
  bool setSetting(uint8_t setting, SFE_QUAD_Sensor_Every_Type_t *value)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (setting)
    {
    default:
      return (false);
      break;
    }
    return (true);
  }
```

But for sensors like the VL53L1X, we do of course want ```setSettings``` to do something. Let's break it down into case 0-1 and 2-4:

```c++
 // Set the specified setting. ===> Adapt this to match the sensor type <===
  bool setSetting(uint8_t setting, SFE_QUAD_Sensor_Every_Type_t *value)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (setting)
    {
    case 0:
      device->stopRanging();
      _shortDistanceMode = true;
      device->setDistanceModeShort();
      device->startRanging();
      break;
    case 1:
      device->stopRanging();
      _shortDistanceMode = false;
      device->setDistanceModeLong();
      if (device->getIntermeasurementPeriod() < 140)
        device->setIntermeasurementPeriod(140);
      device->startRanging();
      break;
```

For the two ```NONE``` types, settings 0 and 1: "Distance Mode: Short" and "Distance Mode: Long" the code in the case statement changes the sensor's distance mode accordingly.

For case 0 ("Distance Mode: Short"), ```setSetting```:
* Stops the sensor with its ```stopRanging()``` method
* Sets the member variable ```_shortDistanceMode``` to ```true``` so we have a record of the mode
* Sets the distance mode to short with ```setDistanceModeShort()```
* (Re)starts the sensor with ```startRanging()```

The code for case 1 ("Distance Mode: Long") is similar, except:
* ```_shortDistanceMode``` is set to ```false```
* For the long distance mode, the sensor's measurement period cannot be shorter than 140. The measurement period is increased if necessary

For settings cases 2-4, the ```UINT16_T``` setting value is checked to make sure it is within the correct limits and is then passed to the Arduino Library set method:

```c++
   case 2:
      device->stopRanging();
      if (value->UINT16_T < 20)
        value->UINT16_T = 20;
      if (!_shortDistanceMode)
        if (value->UINT16_T < 140)
          value->UINT16_T = 140;
      if (value->UINT16_T > 1000)
        value->UINT16_T = 1000;
      device->setIntermeasurementPeriod(value->UINT16_T);
      device->startRanging();
      break;
    case 3:
      device->stopRanging();
      if (value->UINT16_T > 4000)
        value->UINT16_T = 4000;
      device->setXTalk(value->UINT16_T);
      device->startRanging();
      break;
    case 4:
      device->stopRanging();
      if (value->UINT16_T > 4000)
        value->UINT16_T = 4000;
      device->setOffset(value->UINT16_T);
      device->startRanging();
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }
```

### getConfigurationItemName

The **Configuration Item** methods are almost identical to the **Settings** methods. Remember that Configuration Items are simply Settings
which can be written to and read from storage.

For the VL53L1X, "Distance Mode: Short" and "Distance Mode: Long" are combined into a ```BOOL``` for storage. But, other than that,
the Configuration Items match the Settings.

```getConfigurationItemName``` returns a pointer to the name of each configuration item.

The number of ```case``` statements must match **CONFIGURATION_ITEM_COUNT**.

Some important points:
* Configuration Item names must not contain spaces
  * Use underscores where necessary
* The names should be unique
* Keep the names short but meaningful
  * Use abbreviations where possible
  * These names occupy storage media space which - for EEPROM - can be limited
* Never use commas in the names
  * The configurations are stored in CSV format

```c++
  // Return the name of the configuration item
  // Use underscores, not spaces
  const char *getConfigurationItemName(uint8_t configItem)
  {
    switch (configItem)
    {
    case 0:
      return ("Dist_Mode");
      break;
    case 1:
      return ("IM_Period");
      break;
    case 2:
      return ("Xtalk");
      break;
    case 3:
      return ("Offset");
      break;
    default:
      return (NULL);
      break;
    }
    return (NULL);
  }
```

### getConfigurationItemType

```getConfigurationItemType``` is very similar to ```getSettingType```.

For the VL53L1X, the only difference is that the two distance mode ```NONE``` types have been integrated into a single ```BOOL```

```c++
  // Return the type of the specified configuration item
  bool getConfigurationItemType(uint8_t configItem, SFE_QUAD_Sensor_Setting_Type_e *type)
  {
    switch (configItem)
    {
    case 0:
      *type = SFE_QUAD_SETTING_TYPE_BOOL;
      break;
    case 1:
    case 2:
    case 3:
      *type = SFE_QUAD_SETTING_TYPE_UINT16_T;
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }
```

### getConfigurationItem

```getConfigurationItem``` calls the Arduino Library's ```get``` function for that configuration item. The value is returned in the
appropriate field of the ```SFE_QUAD_Sensor_Every_Type_t```.

For the VL53L1X, the three ```uint16_t``` configuration items are returned in ```value->UINT16_T```.

The distance mode (config item 0) is simply read from the ```_shortDistanceMode``` member variable. We _could_ have made use of the Library's
```getDistanceMode``` method and converted the return value (1 or 2) to ```bool```. Doing it this way avoids an unnecessary I2C bus transaction.

```c++
  // Get (read) the sensor configuration item
  bool getConfigurationItem(uint8_t configItem, SFE_QUAD_Sensor_Every_Type_t *value)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (configItem)
    {
    case 0:
      value->BOOL = _shortDistanceMode;
      break;
    case 1:
      value->UINT16_T = device->getIntermeasurementPeriod();
      break;
    case 2:
      value->UINT16_T = device->getXTalk();
      break;
    case 3:
      value->UINT16_T = device->getOffset();
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }
```

### setConfigurationItem

```setConfigurationItem``` is very similar to ```setSetting```.

For the VL53L1X's distance mode, we use the value read from storage to update the ```_shortDistanceMode``` member variable.

```c++
  // Set (write) the sensor configuration item
  bool setConfigurationItem(uint8_t configItem, SFE_QUAD_Sensor_Every_Type_t *value)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (configItem)
    {
    case 0:
      _shortDistanceMode = value->BOOL;
      device->stopRanging();
      if (_shortDistanceMode)
        device->setDistanceModeShort();
      else
        device->setDistanceModeLong();
      device->startRanging();
      break;
    case 1:
      device->stopRanging();
      device->setIntermeasurementPeriod(value->UINT16_T);
      device->startRanging();
      break;
    case 2:
      device->stopRanging();
      device->setXTalk(value->UINT16_T);
      device->startRanging();
      break;
    case 3:
      device->stopRanging();
      device->setOffset(value->UINT16_T);
      device->startRanging();
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }
```

## SFE_QUAD_Sensors.h

When adding a new sensor, [SFE_QUAD_Sensors.h](../src/SFE_QUAD_Sensors.h) needs to be modified in three places:

### INCLUDE_SFE_QUAD_SENSOR_NewSensorName

Add a ```#define INCLUDE_SFE_QUAD_SENSOR_NewSensorName``` for the new sensor. This allows the user to select which sensors to include in the code build.
By default, all sensors are included. Only including selected sensors speeds up the compilation time and reduces the amount of program memory used.

However, **SFE_QUAD_Sensors.h** will be overwritten each time the library is updated. We have not (yet) been able to find a way of defining which sensors to include
in the main **.ino** file. If you can think of a way of doing this - which works on all platforms - then please send us a Pull Request!

For our fictitious **FOO** sensor, we would insert the #define in alphabetical order after **CCS811**:

```c++
// To select which sensors to include:
//   comment #define INCLUDE_SFE_QUAD_SENSOR_ALL
//   uncomment one or more #define INCLUDE_SFE_QUAD_SENSOR_

//#define INCLUDE_SFE_QUAD_SENSOR_ADS122C04 // Include individual sensors
//#define INCLUDE_SFE_QUAD_SENSOR_AHT20
//#define INCLUDE_SFE_QUAD_SENSOR_BME280
//#define INCLUDE_SFE_QUAD_SENSOR_CCS811_5A
//#define INCLUDE_SFE_QUAD_SENSOR_CCS811_5B
//#define INCLUDE_SFE_QUAD_SENSOR_FOO
```

The alphabetical ordering is not important, it just makes the list quicker to read.

### SFEQUADSensorType

Scrolling ~halfway down **SFE_QUAD_Sensors.h**, you will find ```enum SFEQUADSensorType``` near the start of the ```class SFE_QUAD_Sensors```.

We need to add three lines for the new sensor. The ordering here is important as the ```enum SFEQUADSensorType``` is stored with each **Configuration Item**.
The new sensor _must_ be added to the end of the ```enum``` just before ```SFE_QUAD_Sensor_Number_Of_Sensors```. Inserting it anywhere else will prevent existing saved configurations being used with the
updated library.

```c++
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_VL53L1X)
    Sensor_VL53L1X,
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_FOO)
    Sensor_FOO,
#endif
    SFE_QUAD_Sensor_Number_Of_Sensors // Must be last. <=== Add new sensors _above this line_ to preserve the existing enum values
  };
```

However, if you look closely at ```enum SFEQUADSensorType```, you will see that the **MS8607** appears before the **MS5637**.

```c++
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_MS8607) // MS8607 must be before MS5637 (otherwise MS8607 will appear as a MS5637)
    Sensor_MS8607,
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_MS5637)
    Sensor_MS5637,
#endif
```

This is because the MS8607 is essentially a MS5637 with an additional built-in humidity sensor (with its own I<sup>2</sup>C address). The MS8607 must be detected before the MS5637 otherwise it will appear as a MS5637 _and_ a MS8607. ```detectSensors``` contains some additional code to prevent the re-detection of an MS8607 as a MS5637.

There may be similar cases where it is necessary to detect sensors in a particular order and for the new sensor to be inserted part-way through ```enum SFEQUADSensorType```.
If that happens, and you are sending us a Pull Request, please make this clear in the notes.
We may still be able to merge your Pull Request, but we will need to make everyone aware that the new version is not backward-compatible with saved configurations from previous versions.

### sensorFactory

The final change to **SFE_QUAD_Sensors.h** is to add the new sensor to the ```sensorFactory```. This is the method which returns a new object of the requested sensor class.

The order here is not important. Insert the new sensor alphabetically (unless there is a good reason not to):

```c++
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_CCS811_5B)
    if (type == Sensor_CCS811_5B)
      return new SFE_QUAD_Sensor_CCS811_5B;
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_FOO)
    if (type == Sensor_FOO)
      return new SFE_QUAD_Sensor_FOO;
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_LPS25HB)
    if (type == Sensor_LPS25HB)
      return new SFE_QUAD_Sensor_LPS25HB;
#endif
```

## SFE_QUAD_Headers.h

When adding a new sensor, [SFE_QUAD_Headers.h](../src/SFE_QUAD_Headers.h) needs to be modified to include the header file new sensor.

The order here is not important. Insert the new sensor alphabetically (unless there is a good reason not to):

```c++
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_CCS811_5B)
#include "src/SFE_QUAD_Sensor_CCS811_5B.h"
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_FOO)
#include "src/SFE_QUAD_Sensor_FOO.h"
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_LPS25HB)
#include "src/SFE_QUAD_Sensor_LPS25HB.h"
#endif
```

## .github/workflows/compile-sketch.yml

The final change is to update [.github/workflows/compile-sketch.yml](../.github/workflows/compile-sketch.yml) to include the new sensor's Arduino Library.

Any changes to the sensor's Arduino Library are automatically merged into the copy in this library. That way, this library stays up to date with any and all changes to the
individual Arduino Libraries.

The entry for our fictitious **FOO** sensor would be something like:

```c++
      - name: Update FOO
        run: |
          cd ./src/src/
          mkdir -p FOO
          cd FOO
          curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_FOO_Arduino_Library/main/src/SparkFun_FOO_Arduino_Library.h
          curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_FOO_Arduino_Library/main/src/SparkFun_FOO_Arduino_Library.cpp
```

You need to include the full **raw.githubusercontent.com** address for the library files:
* Navigate to the Arduino Library on GitHub
* Navigate to the ```src``` sub-folder
* Open the ```.h``` file
* Click the **RAW** button to view the file's raw content
* Copy and paste the address from your browser into **compile-sketch.yml**
* Repeat for the ```.cpp``` file

If the Arduino Library contains more than the standard ```.h``` and ```.cpp``` files, include those too. E.g. looking at the SGP40:

```c++
      - name: Update SGP40
        run: |
          cd ./src/src/
          mkdir -p SGP40
          cd SGP40
          curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_SGP40_Arduino_Library/main/src/SparkFun_SGP40_Arduino_Library.h
          curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_SGP40_Arduino_Library/main/src/SparkFun_SGP40_Arduino_Library.cpp
          curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_SGP40_Arduino_Library/main/src/sensirion_arch_config.h
          curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_SGP40_Arduino_Library/main/src/sensirion_voc_algorithm.h
          curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_SGP40_Arduino_Library/main/src/sensirion_voc_algorithm.cpp
```

Finally, you need to check how the ```.cpp``` file includes its ```.h``` file. In Arduino examples, you will often see files included like this:

```c++
#include <FOO.h>
```

The less-than and greater-than tell (usually) the Arduino IDE compiler to search its **PATH** for ```FOO.h```. ```FOO.h``` will normally be
in a ```\library``` sub-folder.

For this library, we want to ensure the copy of the Arduino Library in the ```src\src``` sub-folder is used, not the copy from the IDE **PATH**.

Look inside the ```.cpp``` file. If you see:

```c++
#include <FOO.h>
```

the you need to include one extra line in **compile-sketch.yml** so that the less-than and greater-than are changed automatically to double-quotes:

```c++
      - name: Update FOO
        run: |
          cd ./src/src/
          mkdir -p FOO
          cd FOO
          curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_FOO_Arduino_Library/main/src/SparkFun_FOO_Arduino_Library.h
          curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_FOO_Arduino_Library/main/src/SparkFun_FOO_Arduino_Library.cpp
          # SparkFun_FOO_Arduino_Library.cpp uses #include <SparkFun_FOO_Arduino_Library.h>. We need to replace the < and > with double quotes
          sed -i 's/<SparkFun_FOO_Arduino_Library.h>/'\"'SparkFun_FOO_Arduino_Library.h'\"'/g' SparkFun_FOO_Arduino_Library.cpp
```


