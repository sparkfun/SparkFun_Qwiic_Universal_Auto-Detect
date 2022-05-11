# SparkFun Qwiic Universal Auto-Detect

[![SparkFun ESP32 Thing Plus C](https://cdn.sparkfun.com//assets/parts/1/7/2/3/9/18018-Thing_Plus_C_-_ESP32_WROOM-01.jpg)](https://www.sparkfun.com/products/18018)

[*SparkFun ESP32 Thing Plus C (SPX-18018)*](https://www.sparkfun.com/products/18018)

This library makes it possible to create a scalable Qwiic (I<sup>2</sup>C) sensing and logging device on a wide range of platforms.

We are very proud of [OpenLog Artemis](https://www.sparkfun.com/products/16832). The aim of OLA is simple: provide a system which
can automatically detect a wide range of Qwiic sensors and log data from them, all without writing a single line of code. It has been _very_ successful.
However, adding new sensors to OLA is quite complicated. There are [six files to change](https://github.com/sparkfun/OpenLog_Artemis/blob/main/ADDING_SENSORS.md)
and new code needs to be added in multiple places. Also, OLA is, by its nature, tied to the Artemis (Apollo3) platform.

With this library we wanted to make a fresh start. The intention is to provide a universal library which can automatically detect, configure and log data from an
even wider range of Qwiic sensors and to let it run on multiple platforms. Want to make a logging system which runs on your favourite hardware platform?
With this library you can absolutely do that!

If your platform has (e.g.) WiFi or BLE connectivity, this library makes it easy to access the sensor data over those connections. However, you won't find
any WiFi or BLE code in this library. Those are layers you can add yourself depending on which hardware platform you are using. As time goes on, we will add
_examples_ showing how you can (e.g.) access the sensor data via an ESP32 WiFi web page. But we won't ever build that functionality into the library itself.
It then becomes hardware-dependent and that goes against what this library sets out to achieve: hardware-independence!

This library is stand-alone. It contains a built-in copy of the Arduino Library for each sensor. You do not need to worry about installing those libraries yourself.
Each time we update the library, we have a script which will automatically include the latest version of the Arduino Library for each sensor. And each time we add
a new sensor, the latest Arduino Library for that is included automatically too. All you need to do is keep this single library up to date, via the Arduino Library
Manager or by forking or cloning it on GitHub, and everything else happens automatically.

This library contains everything you need to log data from all supported sensors. You do not need a WiFi connection and it does not need to connect to an IO server
or dashboard before you can access your sensor data. No account required!

When it comes to storing the configuration settings for your sensors, we've made that as hardware-independent as possible too. You will find support and examples
showing how you can store the configuration on microSD card, LittleFS or in EEPROM memory.

The examples in this library were written for the [SparkFun Thing Plus C - ESP32 WROOM (SPX-18018)](https://www.sparkfun.com/products/18018)
but can be adapted for any platform.

### Adding A New Sensor

Adding a new sensor is easy! Here are the steps:

* Create an _**SFE_QUAD_Sensor_?????.h**_ file for it, using the existing files as a template
  * You will find the individual sensor header files in the library [src/src](./src/src) sub-directory
* Edit [SFE_QUAD_Sensors.h](./src/SFE_QUAD_Sensors.h) :
  * Add a _**#define INCLUDE_SFE_QUAD_SENSOR_?????**_ for the new sensor - in case the user wants to select which sensors to include
  * Add a new entry for the sensor in _**enum SFEQUADSensorType**_ (3 lines for each new sensor)
  * Add a new entry for the sensor in _**SFE_QUAD_Sensor <b>*</b>sensorFactory**_ (4 lines for each new sensor)
* Edit [SFE_QUAD_Headers.h](./src/SFE_QUAD_Headers.h) :
  * Add a new entry for the sensor (3 lines for each new sensor)
* Edit [.github/workflows/compile-sketch.yml](./.github/workflows/compile-sketch.yml) :
  * Add a _**- name: Update ?????**_ entry for the new sensor
  * This automates copying the Arduino Library files into the [src/src](./src/src) subdirectory
  * The latest versions of the library .h. and .cpp will be copied and added to a GitHub pull request automatically

That's all folks!
                                
### Repository Contents

* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE.
* **/src** - Source files for the library (.cpp, .h).
* **keywords.txt** - Keywords from this library that will be highlighted in the Arduino IDE.
* **library.properties** - General library properties for the Arduino package manager.

### Documentation

* **[Installing an Arduino Library Guide](https://learn.sparkfun.com/tutorials/installing-an-arduino-library)** - Basic information on how to install an Arduino library.

### License Information

This product is _**open source**_!

Please review the [LICENSE.md](./LICENSE.md) file for license information.

If you have any questions or concerns on licensing, please contact techsupport@sparkfun.com.

Please use, reuse, and modify these files as you see fit. Please maintain attribution to SparkFun Electronics and release any derivative under the same license.

Distributed as-is; no warranty is given.

- Your friends at SparkFun.
