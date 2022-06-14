# Example 6 - ESP32 Thing Plus C Logger

A more complex example to show how to log sensor data to microSD card using the SparkFun Qwiic Universal Auto-Detect and SdFat libraries.
This example was written for the [*SparkFun ESP32 Thing Plus C (SPX-18018)*](https://www.sparkfun.com/products/18018) but can be adapted for many platforms.

## Key Features

* Declaring a SFE_QUAD_Sensors object
* Detecting which sensors are connected
* Initializing the sensors
* Setting up a menu
* Configuring the sensor settings through the menu system
* Log data to microSD card using the SdFat library
  * The logging rate / interval is set by the menu
* Writing the settings to microSD card using SdFat
* Reading the settings back from microSD card and applying them

## Setup

For this example, we need to include the header files for SdFat and this library:

```C++
#include <SdFat.h> // Include SdFat.h to enable support for SFE_QUAD_Sensors__SdFat. Do this before #include "SFE_QUAD_Sensors.h"

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect

SFE_QUAD_Sensors__SdFat mySensors;
```

We are using Bill Greiman's SdFat library as it: is very fast; and supports FAT16, FAT32 and exFAT. You can install SdFat through the Arduino IDE Library Manager: search for **SdFat exFAT**.

