# API Reference for the SFE_QUAD_Sensors class

Methods to setup the ```SFE_QUAD_Sensor``` object:
* Return a pointer to the specified sensor class so it can be added to the linked-list of sensors
* Define which Wire port will be used
* Define which Serial port will be used for the built-in menus
* Detect which sensors are attached
* Begin those sensors
* Initialize those sensors (if required)
* Get the sense readings from all enabled sensors
* Get the names of all enabled sensors
* Get the names of all enabled senses
* Define a custom initializer for a sensor (if required / desired)
* Determine if a sensor exists
* Open the logging menu
* Open the setting menu
* Get the sensor and menu configuration in text format
* Apply configuration settings to the sensors and menus

## Brief Overview

The ```SFE_QUAD_Sensors``` class allows a linked-list of individual ```SFE_QUAD_Sensor``` objects to be generated and maintained.

A ```SFE_QUAD_Sensors``` object contains a pointer to a ```SFE_QUAD_Sensor``` named ```_head```.
```_head``` the the head (start) of a linked-list of ```SFE_QUAD_Sensor``` objects.

```_head``` is ```NULL``` initially. After calling ```detectSensors```, it contains the address of the first ```SFE_QUAD_Sensor``` in the linked-list.

Each ```SFE_QUAD_Sensor``` contains a pointer named ```_next``` which points to the next ```SFE_QUAD_Sensor``` in the linked-list.

The ```_next``` of the final ```SFE_QUAD_Sensor``` in the list is ```NULL```.

```detectSensors``` discovers which individual sensors are attached on the selected Wire port. ```detectSensors``` has built-in Qwiic Mux support
and will discover all muxes, and all sensors connected to the ports on those muxes.

```beginSensors``` will call the ```.begin``` method of each attached sensor in turn.

```initialzeSensors``` will perform any additional initialization (if any) required by those sensors.

It is possible to override the initialization code for each sensor type, or individual sensor objects, by calling ```setCustomInitialize```.
(You must call ```setCustomInitialize``` _before_ ```initialzeSensors```.)

```loggingMenu``` and ```settingMenu``` are methods which open built-in menus to:
* Configure which senses on each sensor are enabled for logging
* Call any settings methods the sensors may have (if any)

The sense readings are stored in a dynamic char array named ```readings```. The enabled senses are read with ```getSensorReadings```.
Memory for ```readings``` is allocated dynamically (using ```new```). The memory is freed automatically by the class destructor.
The class has been checked for memory leaks - see Example5.

The names of all enabled sensors, together with their I2C and Mux addresses (if any), can be read with ```getSensorNames```. The names are returned in ```readings```.

The names of all enabled senses can be read with ```getSenseNames```. The names are returned in ```readings```.

The sensor and menu configuration can be stored temporarily in a dynamic char array named ```configuration```.
The text CSV ```configuration``` is written to storage and read from storage by the individual classes for EEPROM, LittleFS, SD and SdFat:
* ```SFE_QUAD_Sensors__EEPROM```
* ```SFE_QUAD_Sensors__LittleFS```
* ```SFE_QUAD_Sensors__SD```
* ```SFE_QUAD_Sensors__SdFat```

Those classes provide additional methods named ```writeConfigurationToStorage```, ```readConfigurationFromStorage``` etc. which write and read
```configuration``` to and from the appropriate storage medium.
