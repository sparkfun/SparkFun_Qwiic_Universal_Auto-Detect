# Example 7 - ESP32 Thing Plus C Logger RTC
# Logging sensor data with Real Time Clock support

A more complex example to show how to log sensor data to microSD card using SparkFun Qwiic Universal Auto-Detect. This example includes Real Time Clock (RTC) support.

This example was written for the [*SparkFun ESP32 Thing Plus C (SPX-18018)*](https://www.sparkfun.com/products/18018) but can be adapted for other ESP32 platforms.

## Key Features

* Declaring a SFE_QUAD_Sensors object
* Detecting which sensors are connected
* Initializing the sensors
* Setting up a menu
* Configuring the WiFi SSID and password through the menu system
* Setting a Time Zone Rule through the menu
* Connecting to WiFi and setting the ESP32 RTC using NTP
* Configuring the sensor settings through the menu
* Log data to microSD card using the SdFat library
    * The logging rate / interval is set by the menu
* Writing the settings to microSD card using SdFat
* Reading the settings back from microSD card and applying them

This example is very similar to [Example6](ex_06_Thing_Plus_C_Logger.md). Please see the Example1 and Example6 documentation for a full walk-through of the code.

Let's walk through the key differences:

## ESP32 WiFi and RTC

The code needs to include extra libraries to support ESP32 WiFi, RTC time and SNTP (Secure Network Time Protocol):

```C++
#include <WiFi.h>
#include "time.h"
#include "sntp.h"
```

## The Menu

The main difference in this example is that it uses the built-in menu to:
* Set the WiFi SSID and password
* Set the names of the NTP (Network Time Protocol) servers
* Set the RTC Time Zone Rule
* Set whether to log local time

You will notice that the menu definition contains a complete sub-menu for the WiFi and NTP settings (note the **SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START**):

```C++
  // WiFi sub-menu: set the WiFi SSID and password
  mySensors.theMenu.addMenuItem("WiFi Menu", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START); // Start of the WiFi sub-menu
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("WiFi Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("=========", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("WiFi SSID", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  mySensors.theMenu.setMenuItemVariable("WiFi SSID", "T-Rex"); // Set the default SSID - this will be updated by readLoggerConfig
  mySensors.theMenu.addMenuItem("WiFi password", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  mySensors.theMenu.setMenuItemVariable("WiFi password", "Has Big Teeth"); // Set the default password - this will be updated by readLoggerConfig
  mySensors.theMenu.addMenuItem("NTP Server 1", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  mySensors.theMenu.setMenuItemVariable("NTP Server 1", "pool.ntp.org");
  mySensors.theMenu.addMenuItem("NTP Server 2", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  mySensors.theMenu.setMenuItemVariable("NTP Server 2", "time.nist.gov");
  mySensors.theMenu.addMenuItem("TimeZone Rule", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  mySensors.theMenu.setMenuItemVariable("TimeZone Rule", "MST7MDT,M3.2.0,M11.1.0"); // TimeZone rule for America/Denver
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("A list of rules for your time zone can be obtained from:", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END); // End of the WiFi sub-menu
```

The sub-menu contains several **SFE_QUAD_MENU_VARIABLE_TYPE_TEXT** items to hold:
* The WiFi SSID (the name of the WiFi network you want to connect to)
* The WiFi password
* The names of the NTP servers
* The Time Zone Rule (which will automatically set the RTC to local time and adjust for daylight saving)

The ```setMenuItemVariable``` lines define the default / initial values for the text items. But, again, these can be updated via the menu or by
reading the configuration from SD card.

The WiFi sub-menu is opened by selecting option 8 from the main menu:

```
Menu
====

1	: Open the sensor logging menu                                     
2	: Open the sensor settings menu                                    
3	: Open new log file                                                
4	: Stop logging                                                     
5	: Set RTC using NTP over WiFi                                      

6	: Write the logger configuration to file                           
7	: Read the logger configuration from file                          

8	: WiFi Menu                                                        
9	: Logging Menu                                                     

Enter a number, or enter 0 to exit:
8

WiFi Menu
=========

1	: WiFi SSID                                                         : T-Rex
2	: WiFi password                                                     : Has Big Teeth
3	: NTP Server 1                                                      : pool.ntp.org
4	: NTP Server 2                                                      : time.nist.gov
5	: TimeZone Rule                                                     : MST7MDT,M3.2.0,M11.1.0

A list of rules for your time zone can be obtained from:
https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h

Enter a number, or enter 0 to exit:
```

To change (e.g.) the name of NTP Server 1, select option 3 and then enter the new name:

```
WiFi Menu
=========

1	: WiFi SSID                                                         : T-Rex
2	: WiFi password                                                     : Has Big Teeth
3	: NTP Server 1                                                      : pool.ntp.org
4	: NTP Server 2                                                      : time.nist.gov
5	: TimeZone Rule                                                     : MST7MDT,M3.2.0,M11.1.0

A list of rules for your time zone can be obtained from:
https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h

Enter a number, or enter 0 to exit:
3
Enter the value (text):
europe.pool.ntp.org
WiFi Menu
=========

1	: WiFi SSID                                                         : T-Rex
2	: WiFi password                                                     : Has Big Teeth
3	: NTP Server 1                                                      : europe.pool.ntp.org
4	: NTP Server 2                                                      : time.nist.gov
5	: TimeZone Rule                                                     : MST7MDT,M3.2.0,M11.1.0

A list of rules for your time zone can be obtained from:
https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h

Enter a number, or enter 0 to exit:
```

As before, the updated settings can be saved to SD card with main menu option 6.

If you are using a terminal emulator, like Tera Term, you may want to enable **backspace** support.
This allows you to edit existing TEXT values without having to re-enter the whole text each time.

```C++
  // The Arduino IDE Serial Monitor does not support backspace so - by default - we need to clear text values when editing them.
  // The user enters the entire text value each time.
  // If the user is using a terminal emulator which supports backspace (e.g. Tera Term) then
  // setting supportsBackspace to true will allow the existing text to be edited.
  mySensors.theMenu.setMenuPort(serialQUAD, true); // Uncomment this line to use serialQUAD for the menu - on a terminal emulator which supports backspace
```

## Setting the RTC

Setting the RTC is a two-step process:
* The ESP32 connects to WiFi
* The ESP32 requests network time from the NTP server(s) and sets the RTC accordingly (using the Time Zone Rule)

The code in ```setRTC()``` is taken mostly from Espressif's SimpleTime example.

The WiFi SSID and password are copied from the menu linked-list into char arrays ```ssid``` and ```password```. The ```WiFi.begin``` then uses the copies:

```C++
  char ssid[32];
  mySensors.theMenu.getMenuItemVariable("WiFi SSID", ssid, 32);
  char password[32];
  mySensors.theMenu.getMenuItemVariable("WiFi password", password, 32);

  serialQUAD.printf("Connecting to %s ", ssid);

  WiFi.begin(ssid, password);

  const unsigned long wifiConnectTimeout = 10000; // Allow 10s for WiFi connection
  unsigned long startTime = millis();
  
  while ((WiFi.status() != WL_CONNECTED) && (millis() < (startTime + wifiConnectTimeout)))
  {
    delay(500);
    serialQUAD.print(".");
  }
```

The names of the NTP servers and the Time Zone Rule are copied and passed into ```configTzTime```:

```C++
  char time_zone[50];
  mySensors.theMenu.getMenuItemVariable("TimeZone Rule", time_zone, 50);

  char ntpServer1[20];
  mySensors.theMenu.getMenuItemVariable("NTP Server 1", ntpServer1, 20);
  char ntpServer2[20];
  mySensors.theMenu.getMenuItemVariable("NTP Server 2", ntpServer2, 20);

  configTzTime(time_zone, ntpServer1, ntpServer2);    
```

There is no function call to specifically set the RTC, it is handled automatically when ```getLocalTime``` is requested:

```C++
  if (WiFi.status() == WL_CONNECTED)
  {
    serialQUAD.println(" CONNECTED");
    struct tm timeinfo; // Doing a getLocalTime immediately after the WiFi connects seems critical to NTP being successful?!
    getLocalTime(&timeinfo);
  }
```

In this example, the WiFi connection is only used to set the RTC, so the WiFi connection is closed once ```getLocalTime``` returns.

## Log Local Time

By default, the RTC local time will be included in the log file. But you can disable it if you want to.

The Logging Sub-Menu includes a bool (boolean) item called ```Log Local Time```:

```C++
  mySensors.theMenu.addMenuItem("Log Local Time", SFE_QUAD_MENU_VARIABLE_TYPE_BOOL);
  defaultValue.BOOL = 1;
  mySensors.theMenu.setMenuItemVariable("Log Local Time", &defaultValue);
```

In the ```loop()```, ```Log Local Time``` controls if the local time is printed and added to the log file (before ```mySensors.readings```):

```C++
    SFE_QUAD_Menu_Every_Type_t logLocalTime;
    mySensors.theMenu.getMenuItemVariable("Log Local Time", &logLocalTime);

    if (logLocalTime.BOOL)
      printLocalTime(serialQUAD); // Print local time if desired
    
    serialQUAD.println(mySensors.readings); // Print the sensor readings
    
    if (onlineDataLogging)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      if (logLocalTime.BOOL)
        printLocalTime(sensorDataFile); // Write local time to the file if desired
      sensorDataFile.println(mySensors.readings); // Write the data to file
```

The helper function ```printLocalTime``` prints the RTC time to Serial or the log file in **YYYY/MM/DD HH:MM:SS** format:

```C++
void printLocalTime(Print &pr)
{
  // getLocalTime stalls for several seconds if the RTC has not been set (using NTP)
  // Use gettimeofday instead (and manually convert to tm)
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  time_t t = (time_t)tv.tv_sec;
  struct tm *timeinfo;
  timeinfo = localtime(&t);
  pr.print(timeinfo, "%Y/%m/%d %H:%M:%S,");
}
```

## Log File Timestamps

This example includes two new support functions: ```updateDataFileCreate``` and ```updateDataFileAccess```.
These make use of SdFat's ```timestamp``` method to set the log file creation and write times to local (RTC) time.
When you look at the directory of the microSD card on your computer, you will notice that the file times and dates are correct.
