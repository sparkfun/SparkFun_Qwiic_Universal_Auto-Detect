# Example 2 - ESP32 LittleFS

Another simple example to show the basic setup and use of SparkFun Qwiic Universal Auto-Detect when using LittleFS to store the sensor configuration.
This example was written for the [*SparkFun ESP32 Thing Plus C (SPX-18018)*](https://www.sparkfun.com/products/18018) but can be adapted for many platforms.

**Key Features**

* Declaring a SFE_QUAD_Sensors object
* Detecting which sensors are connected
* Initializing the sensors
* Setting up a menu
* Configuring the sensor settings through the menu system
* Writing the settings to LittleFS
* Reading the settings back from LittleFS and applying them

This example is almost identical to [Example1](ex_01_Thing_Plus_C.md). Please see the Example1 documentation for a full walk-through of the code.

The only differences in this example are:

## Setup

After installing this library in your local Arduino environment, begin with a standard Arduino sketch and include the header files for LittleFS and this library:

```C++
#include <LittleFS.h> // Include LittleFS.h to enable support for SFE_QUAD_Sensors__LittleFS. Do this before #include "SFE_QUAD_Sensors.h"

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect
```

The next step is to declare the object for the SparkFun Qwiic Universal Auto-Detect. Like most Arduino sketches, this is done at a global scope (after the include file declaration), not within the ```setup()``` or ```loop()``` functions. 

For this example, the **SFE_QUAD_Sensors__LittleFS** class is used. The sensor configuration will be stored in LittleFS.

```C++
SFE_QUAD_Sensors__LittleFS mySensors;
```

Other examples show how to use the other Sensors classes: EEPROM and SD.

## Initialization

Because we are using LittleFS to store the configuration, the call to ```beginStorage``` in ```setup()``` changes to:

```C++
  if (!mySensors.beginStorage("/Config.csv") // Begin the SD storage: "/config.csv" is the file which holds the configuration
  {
    Serial.println(F("beginStorage failed! You will not be able to write or read the sensor configuration..."));
  }
```
