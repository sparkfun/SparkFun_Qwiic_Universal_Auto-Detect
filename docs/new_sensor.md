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

## The **SFE_QUAD_Sensor_NewSensorName.h** File

The **SFE_QUAD_Sensor_NewSensorName.h** file is the interface between the SparkFun Qwiic Universal Auto-Detect **SFE_QUAD_Sensor** class
and the underlying Arduino Library for that sensor.

One of the requirements for SparkFun Qwiic Universal Auto-Detect was to be able to use the existing Arduino Library for any sensor
_as-is_ and without modification. It provides a standard set of methods to allow the sensor's senses to be: configured; enabled; and read.

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

__**Settings**_ are _usually_ also _**Configuration Items**_, but not always. The number of _**Settings**_ is usually equal to, or greater than, the number of _**Configuration Items**_, but not always.

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

But it only has four cofiguration items requiring storage:
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

However:
* Getting an acknowledgement does not tell us what type of sensor was detected at that address, just that _something_ was detected
* The ```beginTransmission``` + ```endTransmission``` test can cause some sensors to produce errors

A better way is to use the sensor's ```begin``` method. However, the ```begin``` can sometimes take a long time to complete if no sensor is connected.

For the BME280, we use the ```beginTransmission``` + ```endTransmission``` test as it gives a fast indication of whether a device is connected (```beginI2C``` is slow if nothing is connected).
Followed by its ```begin``` method for full confidence that we are detecting a BME280.

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

Sensors almost always require their ```begin``` method to be called before communication can take place. Again, you should adapt the template code to match the new sensor:

```c++
  // Begin the sensor. ===> Adapt this to match the sensor type <===
  bool beginSensor(uint8_t sensorAddress, TwoWire &port)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    return (device->begin() == 0);
  }
```

The sensor has already been detected, so you do not need to 'scan' the address again, but you do need to call ```begin``` (again) here.
(The ```begin``` in ```detectSensor``` is 'lost' as the sensor has not been added to the linked-list of sensors at that point.)



Here is the code for the two distance mode _**Settings**_:

```c++
   case 0: // Distance Mode: Short
      device->stopRanging();
      _shortDistanceMode = true;
      device->setDistanceModeShort();
      device->startRanging();
      break;
    case 1: // Distance Mode: Long
      device->stopRanging();
      _shortDistanceMode = false;
      device->setDistanceModeLong();
      if (device->getIntermeasurementPeriod() < 140)
        device->setIntermeasurementPeriod(140);
      device->startRanging();
      break;
```

And here is the code for the setting (applying) the single distance mode _**Configuration Item**_:

```c++
    case 0:
      _shortDistanceMode = value->BOOL;
      device->stopRanging();
      if (_shortDistanceMode)
        device->setDistanceModeShort();
      else
        device->setDistanceModeLong();
      device->startRanging();
      break;
```


