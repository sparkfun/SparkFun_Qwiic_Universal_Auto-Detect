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

The **SFE_QUAD_Sensor_NewSensorName.h** file is the interface between the SparkFun Qwiic Universal Auto-Detect **SFE_QUAD_Sensors** class
and the underlying Arduino Library for that sensor.

One of the requirements for SparkFun Qwiic Universal Auto-Detect was to be able to use the existing Arduino Library for any sensor
_as-is_ and without modification. It provides a standard set of methods to allow the sensor's senses to be: configured; enabled; read; and logged.

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
  Always enclose the include file name in double quotes. Do not use less-than and greater-than.
  This ensures that the copy of the library in the **src/src/FOO** sub-folder is included, not a copy pointed to by the Arduino IDE path.

                            
