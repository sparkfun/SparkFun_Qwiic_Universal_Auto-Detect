# Example 1 - ESP32 Thing Plus C

A simple example to show the basic setup and use of SparkFun Qwiic Universal Auto-Detect.
This example was written for the [*SparkFun ESP32 Thing Plus C (SPX-18018)*](https://www.sparkfun.com/products/18018) but can be adapted for many platforms.

## Key Features

* Declaring a SFE_QUAD_Sensors object
* Detecting which sensors are connected
* Initializing the sensors
* Setting up a menu
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
| `SFE_QUAD_Sensors` | This is the core Sensors class - with no built-in support for writing and reading the sensor and menu configuration |
| `SFE_QUAD_Sensors__SD` | This class adds support for storing the sensor and menu configuration using the Arduino SD library |
| `SFE_QUAD_Sensors__SdFat` | This class adds support for storing the sensor and menu configuration using Bill Greiman's SdFat library - with support for FAT16, FAT32 and exFAT |
| `SFE_QUAD_Sensors__LittleFS` | This class adds support for storing the sensor and menu configuration in memory using LittleFS. ESP32 boards - and others - support LittleFS |
| `SFE_QUAD_Sensors__EEPROM` | This class adds support for storing the sensor and menu configuration in EEPROM. Artemis boards - and others - support EEPROM |
| `SFE_QUAD_Sensors__EEPROM (SAMD)` | This class adds support for storing the sensor and menu configuration in SAMD memory using FlashAsEEPROM_SAMD. You can use this on SAMD21 and SAMD51 boards |

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
* Set up the menu
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

  mySensors.setMenuPort(Serial); // Use Serial for the menus

  mySensors.detectSensors(); // Detect which sensors are connected

  mySensors.beginSensors(); // Begin all the sensors

  mySensors.initializeSensors(); // Initialize all the sensors with the default settings

  if (!mySensors.beginStorage(CS, "/Config.csv") // Begin the SD storage: CS is the chip select pin; "/config.csv" is the file which holds the configuration
  {
    Serial.println(F("beginStorage failed! You will not be able to write or read the sensor configuration..."));
  }
```

## Menu Set-Up

One of the fantastic things about SparkFun Qwiic Universal Auto-Detect is that it has built-in support for menus.
The logging and sensor settings menus are built in to the ```SFE_QUAD_Sensors``` object and can be opened with a single call.
But it gets even better!
```SFE_QUAD_Sensors``` contains a ```SFE_QUAD_Menu``` object called ```theMenu``` which allows you to create your own 
menu and sub-menus quickly and easily. Each menu item can be:

| Menu Item | E.g. | SFE_QUAD_Menu_Variable_Type_e |
| :--- | :--- | :--- |
| A simple line of text | The name of the menu | `SFE_QUAD_MENU_VARIABLE_TYPE_NONE` |
| A link to a sub-menu | | `SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START` |
| A link to a piece of code | Code which is called when you select that menu option | `SFE_QUAD_MENU_VARIABLE_TYPE_CODE` |
| A text variable | A WiFi SSID or password | `SFE_QUAD_MENU_VARIABLE_TYPE_TEXT` |
| bool data | A menu setting which can be toggled true/false | `SFE_QUAD_MENU_VARIABLE_TYPE_BOOL` |
| float or double data | A battery voltage threshold | E.g. `SFE_QUAD_MENU_VARIABLE_TYPE_FLOAT` |
| signed or unsigned integer data | A logging interval | E.g. `SFE_QUAD_MENU_VARIABLE_TYPE_ULONG` |

The menu object has built-in methods which will write and read the menu settings to and from EEPROM, LittleFS or SD.
Say you want to be able to set your WiFi SSID within the menu and have it saved in EEPROM?
SparkFun Qwiic Universal Auto-Detect will let you do that with two lines of code!

The later examples show how to create more complex menus. However, in this example we keep things simple. The menu here allows you to:
* Open the sensor logging menu
* Open the sensor settings menu
* Write the sensor configuration to SD
* Read the sensor configuration from SD

```C++
  // Create the menu
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("====", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Open the sensor logging menu", openLoggingMenu); // This is SFE_QUAD_MENU_VARIABLE_TYPE_CODE
  mySensors.theMenu.addMenuItem("Open the sensor settings menu", openSettingMenu); // This is SFE_QUAD_MENU_VARIABLE_TYPE_CODE
  mySensors.theMenu.addMenuItem("Write the sensor configuration to SD", writeConfig); // This is SFE_QUAD_MENU_VARIABLE_TYPE_CODE
  mySensors.theMenu.addMenuItem("Read the sensor configuration from SD", readConfig); // This is SFE_QUAD_MENU_VARIABLE_TYPE_CODE
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  
  while (mySensors.theMenu._menuPort->available()) // Clear the menu serial buffer
    mySensors.theMenu._menuPort->read();

  Serial.println(F("Press any key to open the menu"));
```

You can use the Arduino IDE Serial Monitor, or a dedicated terminal-emulator like Tera Term, to access the menus.
Open your COM port at 115200 baud and press a key to open the menu:

```
Menu
====

1	: Open the sensor logging menu         
2	: Open the sensor settings menu        
3	: Write the sensor configuration to SD 
4	: Read the sensor configuration from SD

Enter a number, or enter 0 to exit:
```

The code called by the menu is described [below](#the-menus).

## Sensor Names

The sensor names helper text is generated by calling ```getSensorNames();```.
The names of all of the attached sensors are returned in ```mySensors.readings```, strung together in CSV (Comma Separated Value) format.

```C++
  mySensors.getSensorNames(); // Print the sensor names helper text - show which sensors have been found
  Serial.println(mySensors.readings);
```

The sensor name is made up from the following (separated by underscores):
* The sensor type (e.g. MS8607)
* Its I<sup>2</sup>C address
* If the sensor is connected through a [Qwiic Mux](https://www.sparkfun.com/products/16784), the name also contains:
  * The Mux I<sup>2</sup>C address
  * The number of the Mux port which the sensor is attached to
* If a Mux is not being used, the Mux address and port are both shown as **0**

## Sense Names

A sensor can have one or more senses. E.g. a MS8607 sensor has three senses: Pressure, Temperature and Humidity.
The sense names helper text is generated by calling ```getSenseNames();```.
It contains the name of each _enabled_ sense in CSV format. If a sense has been disabled via the ```loggingMenu```, its name will be missing.

```C++
  mySensors.getSenseNames(); // Print the sense names helper text - show which senses are being read
  Serial.println(mySensors.readings);
}
```

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
* Using a mux with address 113 (which is 0x71 hexadecimal (unshifted))
* Using ports 0 and 1

## Sensor Readings

The ```loop()``` reads the sense values from the attached sensors and prints them to Serial. It also checks if the user has pressed a key - to open the menu:

```C++
void loop()
{
  mySensors.getSensorReadings(); // Read all enabled senses from all enabled sensors

  Serial.println(mySensors.readings);

  if (mySensors.theMenu._menuPort->available()) // Has the user pressed a key?
  {
    mySensors.theMenu.openMenu(); // If so, open the menu
  }

  delay(500);
}
```

The ```getSensorReadings();``` method reads the sensor sense values and copies them into ```readings``` which is a char array inside the ```mySensors``` object.

You can print the readings with ```Serial.println(mySensors.readings);```

The ```delay(500);``` sets the read rate: the sensors are read every 500ms (twice per second).

The later examples show how to set the read rate using the built-in configurable menus.

## The Menus

The code which is called by the menu is included at the end of the example. It is very simple too:

```C++
void openLoggingMenu(void)
{
  mySensors.loggingMenu();
  
  mySensors.getSensorNames(); // Print the sensor names helper - it may have changed
  Serial.println(mySensors.readings);
  
  mySensors.getSenseNames(); // Print the sense names helper - it may have changed
  Serial.println(mySensors.readings);  
}

void openSettingMenu(void)
{
  mySensors.settingMenu();
}

void writeConfig(void)
{
  mySensors.getSensorAndMenuConfiguration();
  mySensors.writeConfigurationToStorage(false); // Set append to false - overwrite the configuration  
}

void readConfig(void)
{
  mySensors.readConfigurationFromStorage();
  mySensors.applySensorAndMenuConfiguration();
}
```

## The Logging Menu

If you select **Open the sensor logging menu** (Option 1), the built-in ```loggingMenu()``` method is called and the logging menu is opened.
The logging menu allows the user to configure which of the connected sensors and senses are enabled (selected) for 'logging'.
(This example does not actually log any data, the sense readings are only printed to Serial. The later examples show how to actually log the data.)
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
1.0100100e+3,2.0540001e+1,4.6127838e+1,1.0096700e+3,2.0670000e+1,4.1237396e+1
1.0100000e+3,2.0559999e+1,4.6127838e+1,1.0097000e+3,2.0690001e+1,4.1237396e+1
1.0099900e+3,2.0559999e+1,4.6120209e+1,1.0096900e+3,2.0690001e+1,4.1245026e+1
1.0099800e+3,2.0559999e+1,4.6127838e+1,1.0096800e+3,2.0690001e+1,4.1237396e+1

Menu
====

1	: Open the sensor logging menu         
2	: Open the sensor settings menu        
3	: Write the sensor configuration to SD 
4	: Read the sensor configuration from SD

Enter a number, or enter 0 to exit:
1

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

## The Setting Menu

If you select **Open the sensor settings menu** (Option 2), the built-in ```settingMenu()``` method is called and the sensor settings menu is opened.
The settings menu allows the user to configure the sensor settings - if it has any. The settings can be stored in EEPROM, LittleFS or on SD card.

A good example of how to use the settings is with the [NAU7802](https://www.sparkfun.com/products/15242) load cell sensor.
To use the load cell sensor, we need to calibrate it by setting its zero offset and calibration factor.

If we connect a single NAU7802, the readings appear initially as:

```
SparkFun Qwiic Universal Auto-Detect Example
Press any key to open the menu
NAU7802_42_0_0
Weight
nan
nan
nan
```

**nan** means Not A Number. The NAU7802 is retuning invalid data as it has not been calibrated.

If we open the settings menu we see:

```
Menu
====

1	: Open the sensor logging menu         
2	: Open the sensor settings menu        
3	: Write the sensor configuration to SD 
4	: Read the sensor configuration from SD

Enter a number, or enter 0 to exit:
2

Sensor NAU7802 at address 0x2A
1	: Set Zero Offset
2	: Set Calibration Factor
Enter a number to change the setting, or enter 0 to exit:
```

With no weight on the load cell, selecting option 1 sets the zero offset.

Place (e.g.) a 100g weight on the load cell, select option 2 and enter the value 100.00 when instructed:

```
Menu
====

1	: Open the sensor logging menu         
2	: Open the sensor settings menu        
3	: Write the sensor configuration to SD 
4	: Read the sensor configuration from SD

Enter a number, or enter 0 to exit:
2

Sensor NAU7802 at address 0x2A
1	: Set Zero Offset
2	: Set Calibration Factor
Enter a number to change the setting, or enter 0 to exit:
Set Zero Offset was successful

Sensor NAU7802 at address 0x2A
1	: Set Zero Offset
2	: Set Calibration Factor
Enter a number to change the setting, or enter 0 to exit:
Enter a floating point value (or wait 10 seconds to abort): 
Set Calibration Factor was successful

Sensor NAU7802 at address 0x2A
1	: Set Zero Offset
2	: Set Calibration Factor
Enter a number to change the setting, or enter 0 to exit:
settingMenu: exiting...

Menu
====

1	: Open the sensor logging menu         
2	: Open the sensor settings menu        
3	: Write the sensor configuration to SD 
4	: Read the sensor configuration from SD

Enter a number, or enter 0 to exit:

1.0056678e+2
```

The load cell has been successfully calibrated. The first reading is:
* 1.0056678e+2 = 100.56678g

Another of the fantastic things about SparkFun Qwiic Universal Auto-Detect is that we can now store that calibration to SD card by selecting
**Write the sensor configuration to SD** (Option 3) from the main menu.

Next time we power up the system, we can restore the calibration by selecting 
**Read the sensor configuration from SD** (Option 4):

```
1.5076448e+5
1.5081995e+5
1.5078534e+5
1.5074564e+5

Menu
====

1	: Open the sensor logging menu         
2	: Open the sensor settings menu        
3	: Write the sensor configuration to SD 
4	: Read the sensor configuration from SD

Enter a number, or enter 0 to exit:
4

Menu
====

1	: Open the sensor logging menu         
2	: Open the sensor settings menu        
3	: Write the sensor configuration to SD 
4	: Read the sensor configuration from SD

Enter a number, or enter 0 to exit:

1.1137814e+2
1.0925388e+2
1.1440324e+2
```

Storing the configuration was not possible with [OpenLog Artemis](https://www.sparkfun.com/products/19426). We are very pleased that
SparkFun Qwiic Universal Auto-Detect makes it so easy!

You can load the settings by default by calling ```readConfig();``` in your ```setup()```.

The stored configuration includes which mux port the sensor is connected to. So, you can:
* Connect multiple NAU7802s via a [Qwiic Mux](https://www.sparkfun.com/products/16784)
* Calibrate all of the sensors using the menus
* Store the configuration for all the sensors
* Restore the configuration for all the sensors

So long as you do not change which mux port each sensor is connected to, the correct configuration will be restored each time!

## Menu Timeout

If the 10 second menu timeout is too short for you, you can change it to (e.g.) 20 seconds by calling:
* ```mySensors.theMenu.setMenuTimeout(20000);```

The menu timeout is set in milliseconds.

