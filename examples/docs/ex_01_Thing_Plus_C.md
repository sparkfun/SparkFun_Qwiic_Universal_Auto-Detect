# Example 1 - ESP32 Thing Plus C

A simple example to show the basic setup and use of SparkFun Qwiic Universal Auto-Detect.
This example was written for the [*SparkFun ESP32 Thing Plus C (SPX-18018)*](https://www.sparkfun.com/products/18018) but can be adapted for many platforms.

**Key Features**

* Declaring a SFE_QUAD_Sensors object
* Detecting which sensors are connected
* Initializing the sensors
* Configuring the sensor settings through the menu system
* Writing the settings to microSD card using the standard Arduino SD library
* Reading the settings back from microSD card and applying them

Later examples show how to store the sensor settings in EEPROM and LittleFS.

## Setup

After installing this library in your local Arduino environment, begin with a standard Arduino sketch and include the header files for SD and this library:

```C++
#include <SD.h> // Include SD.h to enable support for SFE_QUAD_Sensors__SD. Do this before #include "SFE_QUAD_Sensors.h"

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect
```

The next step is to declare the object for the SparkFun Qwiic Universal Auto-Detect. Like most Arduino sketches, this is done at a global scope (after the include file declaration), not within the ```setup()``` or ```loop()``` functions. 

Select one of the following classes:

| Class | Description |
| :--- | :--- |
| `SFE_QUAD_Sensors` | This is the core Sensors class - with no built-in support for writing and reading the sensor configuration |
| `SFE_QUAD_Sensors__SD` | This class adds support for storing the sensor configuration using the Arduino SD library |
| `SFE_QUAD_Sensors__SdFat` | This class adds support for storing the sensor configuration using Bill Greiman's SdFat library - with support for FAT16, FAT32 and exFAT |
| `SFE_QUAD_Sensors__LittleFS` | This class adds support for storing the sensor configuration in memory using LittleFS. ESP32 boards - and others - support LittleFS |
| `SFE_QUAD_Sensors__EEPROM` | This class adds support for storing the sensor configuration in EEPROM. Artemis boards - and others - support EEPROM |
| `SFE_QUAD_Sensors__EEPROM (SAMD)` | This class adds support for storing the sensor configuration in SAMD memory using FlashAsEEPROM_SAMD. You can use this on SAMD21 and SAMD51 boards |

For this example, the **SFE_QUAD_Sensors__SD** class is used. The sensor configuration will be stored on SD card using the standard Arduino SD library.

```C++
SFE_QUAD_Sensors__SD mySensors;
```

The later examples show how to use the other Sensors classes. E.g. if you wanted to use the **SFE_QUAD_Sensors__EEPROM** class for **SAMD**, the code becomes:

```C++
#include <FlashAsEEPROM_SAMD.h> // Include FlashAsEEPROM_SAMD.h to enable support for SFE_QUAD_Sensors__EEPROM on SAMD platforms. Do this before #include "SFE_QUAD_Sensors__EEPROM.h"

#include "SFE_QUAD_Sensors__EEPROM.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect

SFE_QUAD_Sensors__EEPROM mySensors;
```

## Initialization

In the ```setup()``` function of this sketch, we need to do several things to:
* Start ```Wire``` (I<sup>2</sup>C) communication
* Tell the QUAD object which Wire port to use
* Tell the QUAD object which Serial port to use for the menus and to display the sensor readings
* Detect which sensors are attached
* Initialize the sensors
* Begin the microSD storage, so we can later write and read the settings to and from microSD card
* Display which sensors have been found and what the sense names are:
  * A sensor can have multiple senses. E.g. a MS8607 sensor has three senses: Pressure, Temperature and Humidity
  * You can tell the QUAD object which senses to enable through the menus. For the MS8607, the code can read: none, one, two or all three senses

```C++
void setup()
{
  delay(1000); //Give the sensors time to power on

  Serial.begin(115200);
  Serial.println(F("SparkFun Qwiic Universal Auto-Detect Example"));

  Wire.begin(); // Begin the Wire port at the default speed (usually 100kHz)

  mySensors.setWirePort(Wire); // Tell the class which Wire port to use

  //mySensors.enableDebugging(Serial); // Uncomment this line to enable debug messages on Serial

  mySensors.setMenuPort(Serial); // Use Serial for the logging menu

  mySensors.detectSensors(); // Detect which sensors are connected

  mySensors.beginSensors(); // Begin all the sensors

  mySensors.initializeSensors(); // Initialize all the sensors with the default settings

  if (!mySensors.beginStorage(CS, "/OLconfig.csv") // Begin the SD storage: CS is the chip select pin; "/OLconfig.csv" is the file which holds the configuration
  {
    Serial.println(F("beginStorage failed! You will not be able to write or read the sensor configuration..."));
  }

  mySensors.getSensorNames(); // Print the sensor names helper text - show which sensors have been found
  Serial.println(mySensors.readings);

  mySensors.getSenseNames(); // Print the sense names helper text - show which senses are being read
  Serial.println(mySensors.readings);
```

## Sensor Names

The sensor names helper text is generated by calling ```getSensorNames();```.
The names of all of the attached sensors are strung together in CSV (Comma Separated Value) format.

The sensor name is made up from the following (separated by underscores):
* The sensor type (e.g. MS8607)
* Its I<sup>2</sup>C address
* If the sensor is connected through a [Qwiic Mux](https://www.sparkfun.com/products/16784), the name also contains:
  * The Mux I<sup>2</sup>C address
  * The number of the Mux port which the sensor is attached to
* If a Mux is not being used, the Mux address and port are both shown as ```0```

## Sense Names

A sensor can have one or more senses. E.g. a MS8607 sensor has three senses: Pressure, Temperature and Humidity.
The sense names helper text is generated by calling ```getSenseNames();```.
It contains the name of each _enabled_ sense in CSV format. If a sense has been disabled via the ```loggingMenu```, its name will be missing.

E.g. if a single MS8607 is attached, not using a mux, and all three senses are enabled, the sensor and sense names will be shown as:

```
MS8607_64_0_0,MS8607_64_0_0,MS8607_64_0_0
Pressure (mbar),Temperature (C),Humidity (%)
```

The sensor name is the following (separated by underscores):
* MS8607
* The I<sup>2</sup>C address: 64 decimal (which is 0x40 hexadecimal (unshifted))
* No mux (address is shown as zero)
* No mux port (port number is shown as zero)

The sense names are self-explanatory. Again, they are Comma-Separated.

If two MS8607s are attached, using a [Qwiic Mux](https://www.sparkfun.com/products/16784), the sensor and sense helper text could be:

```
MS8607_64_113_0,MS8607_64_113_0,MS8607_64_113_0,MS8607_64_113_1,MS8607_64_113_1,MS8607_64_113_1
Pressure (mbar),Temperature (C),Humidity (%),Pressure (mbar),Temperature (C),Humidity (%)
```

The sensor names indicate that the two MS8607s are connected:
* Using a Mux with address 113 (which is 0x71 hexadecimal (unshifted))
* Using ports 0 and 1

## Sensor Readings

The ```loop()``` reads the sense values from the attached sensors and prints them to Serial. In its simplest form, the loop could be:

```C++
void loop()
{
  mySensors.getSensorReadings(); // Read all enabled senses from all enabled sensors

  Serial.println(mySensors.readings);

  delay(500);
}
```

The ```getSensorReadings();``` method reads the sensor sense values and copies them into ```readings``` which is a char array inside the ```mySensors``` object.

You can print the readings with ```Serial.println(mySensors.readings);```

The ```delay(500);``` sets the read rate: the sensors are read ever 500ms (twice per second).

The later examples show how to set the read rate using the built-in configurable menus.

## The Menus

```setup()``` also prints some helper text showing how to access the built-in menus via Serial.
You can use the Arduino IDE Serial Monitor, or a dedicated terminal-emulator like Tera Term, to access the menus.
Open your COM port at 115200 baud and press the appropriate key:

```C++
  Serial.println(F("Press L to open the logging menu"));
  Serial.println(F("Press S to open the setting menu"));
  Serial.println(F("Press W to write the sensor configuration to file"));
  Serial.println(F("Press R to read the configuration file and configure the sensors"));
}
```

The ```loop()``` contains code which checks if the user has pressed a key and performs the requested action:

```C++
  if (mySensors._menuPort->available()) // Has the user pressed a key?
  {
    char choice = mySensors._menuPort->read(); // Read which key was pressed
    
    if ((choice == 'l') || (choice == 'L'))
    {
      mySensors.loggingMenu();
      
      mySensors.getSensorNames(); // Print the sensor names helper
      Serial.println(mySensors.readings);
      mySensors.getSenseNames(); // Print the sense names helper
      Serial.println(mySensors.readings);
    }
      
    else if ((choice == 's') || (choice == 'S'))
      mySensors.settingMenu();

    else if ((choice == 'w') || (choice == 'W'))
    {
      mySensors.getSensorAndMenuConfiguration();
      mySensors.writeConfigurationToStorage(false); // Set append to false - overwrite the configuration
    }

    else if ((choice == 'r') || (choice == 'R'))
    {
      mySensors.readConfigurationFromStorage();
      mySensors.applySensorAndMenuConfiguration();
    }
  }
```

## The Logging Menu

If you press **l** or **L**, the built-in ```loggingMenu()``` method is called and the logging menu is opened.
The logging menu allows the user to configure which of the connected sensors and senses are enabled (selected) for 'logging'.
The menu automatically configures itself depending on which sensors are connected.
For each sensor, there is an option to disable all of its senses if required.
Extra lines in the menu allow individual senses to be enabled or disabled.
E.g. if only a single MS8607 is attached, the menu will appear as:

```
Sensor MS8607 at address 0x40
1	: Log this sensor     : Yes
2	: Log Pressure (mbar) : Yes
3	: Log Temperature (C) : Yes
4	: Log Humidity (%)    : Yes
Enter a number to toggle the setting, or enter 0 to exit:
```

Selecting option 1 will disable the sensor completely:

```
Sensor MS8607 at address 0x40
1	: Log this sensor     : No
Enter a number to toggle the setting, or enter 0 to exit:
```

Selecting option 1 again, followed by option 3, results in the Temperature sense being disabled:

```
Sensor MS8607 at address 0x40
1	: Log this sensor     : Yes
2	: Log Pressure (mbar) : Yes
3	: Log Temperature (C) : No
4	: Log Humidity (%)    : Yes
Enter a number to toggle the setting, or enter 0 to exit:
```

The helper text and readings become:

```
MS8607_64_0_0,MS8607_64_0_0
Pressure (mbar),Humidity (%)
1.0110200e+3,4.1237396e+1
1.0109800e+3,4.1237396e+1
1.0109800e+3,4.1222137e+1
1.0109800e+3,4.1206879e+1
1.0109500e+3,4.1199249e+1
```

We can see that only Pressure and Humidity are enabled.
The pressure and temperature are logged in _**exponent**_ format as it is compact yet provides good precision / resolution.
The pressure is 1.011e+3 mbar = 1011 mbar.
The humidity is 4.12e+1 % = 41.2 %.
The default precision (number of decimal places) is 7.
You can adjust the precision by calling the method ```_sprintf.setPrecision```.
E.g. to change the precision to 5 decimal places, call:
* ```mySensors._sprintf.setPrecision(5);```

There is a helper method ```_sprintf.expStrToDouble``` which you can use to convert exponent-format text into a double.

If two MS8607s are connected via a Mux, the helper text, readings and menus become:

```
MS8607_64_113_0,MS8607_64_113_0,MS8607_64_113_0,MS8607_64_113_1,MS8607_64_113_1,MS8607_64_113_1
Pressure (mbar),Temperature (C),Humidity (%),Pressure (mbar),Temperature (C),Humidity (%)
1.0111400e+3,2.2910000e+1,4.6791595e+1,1.0108100e+3,2.3150000e+1,4.1435760e+1
1.0111500e+3,2.2930000e+1,4.6799225e+1,1.0108300e+3,2.3160000e+1,4.1451019e+1
1.0111300e+3,2.2930000e+1,4.6806854e+1,1.0108400e+3,2.3160000e+1,4.1466278e+1
1.0111500e+3,2.2940001e+1,4.6806854e+1,1.0108000e+3,2.3160000e+1,4.1481537e+1

Sensor MS8607 at address 0x40, mux address 0x71 port 0
1	: Log this sensor     : Yes
2	: Log Pressure (mbar) : Yes
3	: Log Temperature (C) : Yes
4	: Log Humidity (%)    : Yes
Sensor MS8607 at address 0x40, mux address 0x71 port 1
5	: Log this sensor     : Yes
6	: Log Pressure (mbar) : Yes
7	: Log Temperature (C) : Yes
8	: Log Humidity (%)    : Yes
Enter a number to toggle the setting, or enter 0 to exit:
```
