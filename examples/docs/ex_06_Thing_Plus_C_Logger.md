# Example 6 - ESP32 Thing Plus C Logger
# Logging sensor data to microSD card

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

This example is very similar to [Example1](ex_01_Thing_Plus_C.md). Please see the Example1 documentation for a full walk-through of the code.

Let's walk through the key differences:

## Setup

For this example, we need to: include the header files for SdFat and this library; and create a **SFE_QUAD_Sensors__SdFat** object.

```C++
#include <SdFat.h> // Include SdFat.h to enable support for SFE_QUAD_Sensors__SdFat. Do this before #include "SFE_QUAD_Sensors.h"

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect

SFE_QUAD_Sensors__SdFat mySensors;
```

We are using Bill Greiman's SdFat library as it: is very fast; and supports FAT16, FAT32 and exFAT. You can install SdFat through the Arduino IDE Library Manager: search for **SdFat exFAT**.

## The Menu

One of the key differences in this example is that it uses the built-in menu to control the logging rate. You will notice that the menu definition contains extra lines:

```C++
  mySensors.theMenu.addMenuItem("Logging interval (ms)", SFE_QUAD_MENU_VARIABLE_TYPE_ULONG);  
  SFE_QUAD_Menu_Every_Type_t defaultValue;
  defaultValue.ULONG = 1000;
  mySensors.theMenu.setMenuItemVariable("Logging interval (ms)", &defaultValue); // Set the default logging interval - this will be updated by readConfig
  defaultValue.ULONG = 100;
  mySensors.theMenu.setMenuItemVariableMin("Logging interval (ms)", &defaultValue); // Set the minimum logging interval - this will be updated by readConfig
  defaultValue.ULONG = 3600000;
  mySensors.theMenu.setMenuItemVariableMax("Logging interval (ms)", &defaultValue); // Set the maximum logging interval - this will be updated by readConfig
```

As before, most of the menu items are **SFE_QUAD_MENU_VARIABLE_TYPE_NONE** (for the lines of plain text) or **SFE_QUAD_MENU_VARIABLE_TYPE_CODE** (a function which is called by the menu).
In this example we have added an **SFE_QUAD_MENU_VARIABLE_TYPE_ULONG** item and have given it the name ```Logging interval (ms)```.

All Arduino platforms provide a millisecond counter called ```millis()```. It returns a count of milliseconds as an **unsigned long** (usually 32-bit) number.
We use that to control the logging rate by comparing it to the ```Logging interval (ms)```.

This line creates the logging interval menu item:
* ```mySensors.theMenu.addMenuItem("Logging interval (ms)", SFE_QUAD_MENU_VARIABLE_TYPE_ULONG);```

These three lines give ```Logging interval (ms)``` a default / initial value of 1000 (milliseconds):
* ```SFE_QUAD_Menu_Every_Type_t defaultValue;```
* ```defaultValue.ULONG = 1000;```
* ```mySensors.theMenu.setMenuItemVariable("Logging interval (ms)", &defaultValue);```

The first of those lines creates a variable called ```defaultValue``` of type ```SFE_QUAD_Menu_Every_Type_t```.
```SFE_QUAD_Menu_Every_Type_t``` is a struct containing, as the name suggests, one of every type. Here is its definition in the library:

```C++
// Struct to hold "every type"
typedef struct
{
  void (*CODE)();
  char *TEXT;
  bool BOOL;
  float FLOAT;
  double DOUBLE;
  int INT;
  uint8_t UINT8_T;
  uint16_t UINT16_T;
  uint32_t UINT32_T;
  unsigned long ULONG;
  long LONG;
} SFE_QUAD_Menu_Every_Type_t;
```

The second line of code loads the ```ULONG``` (unsigned long) with 1000. The third copies ```defaultValue``` into the linked-list menu entry for ```Logging interval (ms)```.

This provides the default or initial value for ```Logging interval (ms)```, but the code will automatically update it with the value stored on microSD card when we call ```readConfig();```

The next four lines re-use ```defaultValue``` to set minimum and maximum values / limits for ```Logging interval (ms)```.
The minimum and maximum values are optional, but, if you do not include them, the code will accept any value from zero to the maximum an unsigned long can hold.
The code sets sensible limits of 100ms and 3600000ms (one hour):
* ```defaultValue.ULONG = 100;```
* ```mySensors.theMenu.setMenuItemVariableMin("Logging interval (ms)", &defaultValue);```
* ```defaultValue.ULONG = 3600000;```
* ```mySensors.theMenu.setMenuItemVariableMax("Logging interval (ms)", &defaultValue);```

In the main ```loop()```, we can read whatever value ```Logging interval (ms)``` has been set to, and use it to control when the sensors are read by comparing it to ```millis()```:

```C++
  // Logging interval - read the sensors every loggingInterval milliseconds
  static unsigned long lastRead = 0;
  SFE_QUAD_Menu_Every_Type_t loggingInterval;
  mySensors.theMenu.getMenuItemVariable("Logging interval (ms)", &loggingInterval); // Get the logging interval from theMenu
  if (millis() > (lastRead + loggingInterval.ULONG)) // Is it time to read the sensors?
  {
    lastRead = millis(); // Update the time of the last read
```

```lastRead``` is a **static** variable which holds a copy of ```millis()```. Because it is **static**, it retains its value each time the code goes around the ```loop()```.
(It is only set to zero the first time around the loop.) (We could have declared it as a global variable, before ```setup()```, instead.)

```loggingInterval``` is another ```SFE_QUAD_Menu_Every_Type_t```. We read the value of ```Logging interval (ms)``` from the menu linked-list and copy it into ```loggingInterval``` with this line of code:
* ```mySensors.theMenu.getMenuItemVariable("Logging interval (ms)", &loggingInterval);```

We then compare ```loggingInterval``` to the copy of ```millis()```, held in ```lastRead```, to decide when to take the next reading.

If we change ```loggingInterval``` by opening the menu and selecting option 1, the code will automatically use the new interval from then on!

Here is the value being changed from the default 1000ms to 500ms. Note the minimum and maximum values we set earlier. The menu will reject any values outside of those.

```
Menu
====

1	: Logging interval (ms)                 : 1000

2	: Stop logging                         

3	: Open the sensor logging menu         
4	: Open the sensor settings menu        
5	: Write the logger configuration to SD 
6	: Read the logger configuration from SD

Enter a number, or enter 0 to exit:
1
Enter the value (unsigned long) (Min: 100) (Max: 3600000) : 
500

Menu
====

1	: Logging interval (ms)                 : 500

2	: Stop logging                         

3	: Open the sensor logging menu         
4	: Open the sensor settings menu        
5	: Write the logger configuration to SD 
6	: Read the logger configuration from SD

Enter a number, or enter 0 to exit:
```

We can save the new value to SD card by selecting option 5.

## Logging data to SD card

The previous examples have not actually logged (saved or recorded) any sensor readings. They have only printed them to Serial.
But we do of course want to _save_ (write) that data to SD card so we can study it later. Here is the extra code which lets us do that.

### Creating a File object

SdFat allows us to use several different file types. SparkFun Qwiic Universal Auto-Detect defaults to using an ```FsFile``` object as it supports FAT16, FAT32 and exFAT (for very large files).

```SFE_QUAD_SD_FAT_TYPE``` is set to **3** inside ```SFE_QUAD_Sensors.h```. The example contains the following code just in case ```SFE_QUAD_SD_FAT_TYPE``` has been changed:

```C++
// Define the log file type - use the same type as the Qwiic Universal Auto-Detect library
#if SFE_QUAD_SD_FAT_TYPE == 1
  File32 sensorDataFile; //File that all sensor data is written to
#elif SFE_QUAD_SD_FAT_TYPE == 2
  ExFile sensorDataFile; //File that all sensor data is written to
#elif SFE_QUAD_SD_FAT_TYPE == 3
  FsFile sensorDataFile; //File that all sensor data is written to
#else // SD_FAT_TYPE == 0
  File sensorDataFile; //File that all sensor data is written to
#endif  // SD_FAT_TYPE

char sensorDataFileName[30] = ""; // This will hold the name of the sensorDataFile
bool onlineDataLogging; //This flag indicates if we are logging data to sensorDataFile
```

This creates a ```FsFile``` object called ```sensorDataFile```. We have not yet opened the file, or given it a name. We do that within ```setup()```:

```C++
  // mySensors.beginStorage has done the sd.begin for us
  // Open the next available log file
  onlineDataLogging = false;
  Serial.println(F("Finding the next available log file..."));
  if (findNextAvailableLog(sensorDataFileName, "dataLog", false)) // Do not reuse empty files - to save time
  {
    // O_CREAT - create the file if it does not exist
    // O_APPEND - seek to the end of the file prior to each write
    // O_WRITE - open for write
    onlineDataLogging = sensorDataFile.open(sensorDataFileName, O_CREAT | O_APPEND | O_WRITE) == true;
  }
```

Normally we would need to call ```sd.begin``` but ```mySensors.beginStorage``` has done that for us. The example re-uses the ```SdFs sd``` object which is created in ```SFE_QUAD_Sensors.h```.

```onlineDataLogging``` is the global **bool** we created above. We use it as a flag to indicate that the log file is open and that data can be written to it.

```findNextAvailableLog``` is a helper function which finds the name of the next available file, by adding a five digit sequential number to "dataLog".
The data log files are called:
* dataLog00000.csv
* dataLog00001.csv
* dataLog00002.csv

etc.. You will find the code for ```findNextAvailableLog``` at the bottom of the example.

### Writing data to file

The sensor and sense helper text is written into the log file by these lines:

```C++
  mySensors.getSensorNames(); // Print the sensor names helper
  if (onlineDataLogging)
    sensorDataFile.println(mySensors.readings);
```

```C++
  mySensors.getSenseNames(); // Print the sense names helper
  if (onlineDataLogging)
    sensorDataFile.println(mySensors.readings);
```

A new log file is created each time you open the logging menu as the sensor and sense helper text may have changed.

The sense data is written to file by these lines. They also flash LED_BUILTIN to indicate that the write is taking place:

```C++
    if (onlineDataLogging)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      sensorDataFile.println(mySensors.readings); // Write the data to file
      sensorDataFile.sync(); // This will help prevent data loss if the power is removed during logging
      digitalWrite(LED_BUILTIN, LOW);
    }
```

### Closing the log file

The ```sensorDataFile.sync();``` should prevent data loss if you disconnect the power while the log file is still open.
But, for safety, you should always close the log file before removing the power. Call ```stopLogging()``` via the menu or by pressing the stop-logging button.

```stopLogging()``` closes the log file:

```C++
void stopLogging(void)
{
  if (onlineDataLogging)
  {
    sensorDataFile.close();
    onlineDataLogging = false;
    Serial.println(F("Log file closed"));
  }
}
```

### Stop-Logging Button

Most ESP32 boards have a **BOOT** button which can be used as a general-purpose input when code is running.
This example uses the **BOOT** button as a stop-logging button.

On the [Thing Plus C](https://www.sparkfun.com/products/18018), the BOOT pin (digital pin 0) is normally used as an output to enable the voltage regulator for the Qwiic bus:

```C++
int qwiicPower = 0; //Thing Plus C digital pin 0 is connected to the v-reg that controls the Qwiic power. It is also connected to the BOOT button.
```

However, we can 'cheat' and use it as an input if we set it to ```INPUT_PULLUP``` in ```setup()```. The pull-up resistor, when enabled, is enough to enable the Qwiic voltage regulator:

```C++
  // Enable power for the Qwiic bus
  // The input pull-up is enough to enable the regulator
  // We can then also use the BOOT button as a "stop logging" button
  pinMode(qwiicPower, INPUT_PULLUP);
```

The ```loop()``` checks if the button has been pressed and closes the log file if it has:

```C++
  if (digitalRead(qwiicPower) == LOW) // Check if the user has pressed the stop logging button
  {
    stopLogging();
  }
```

On the Thing Plus C, pressing the BOOT button will disable the power for the Qwiic bus and power-off all of the sensors.
When you release the button, the Qwiic power will be re-enabled but the sensors will need to be re-initialized before you can use them.
You will need to press the **RST** (RESET) button, or power the board off and on again, to re-start the code and re-initialize the sensors.
