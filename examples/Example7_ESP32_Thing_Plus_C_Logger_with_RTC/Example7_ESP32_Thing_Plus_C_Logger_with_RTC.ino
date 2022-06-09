/*
  SFE QUAD Logger - ESP32 Thing Plus C
  By: Paul Clark
  SparkFun Electronics
  Date: June 2022
  
  This code is a proof-of-concept demonstration of a scalable I2C sensing and logging device,
  based initially on the SparkFun Thing Plus C - ESP32 WROOM (SPX-18018):
  https://www.sparkfun.com/products/18018
  
  If you are using the SparkFun Thing Plus C, please make sure you have the latest Espressif Systems esp32 board package installed.
  Click here to get the board package:  http://boardsmanager/All#esp32
  Select the "ESP32 Dev Module" as the board.

  The sensor data, sensor configuration and logger configuration are written to microSD using SdFat
                                  
  License: MIT
  Please see LICENSE.md for more details
  
*/


#define serialQUAD Serial // Use Serial for the Logger and Qwiic Universal Auto-Detect menus

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Use SdFat for sensor data storage - and to hold the menu configuration

#include <SdFat.h> // Include SdFat.h to enable support for SFE_QUAD_Sensors__SdFat. Do this before #include "SFE_QUAD_Sensors.h"

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect

SFE_QUAD_Sensors__SdFat mySensors;

const char loggerConfigurationFileName[] = "Logger_Config.csv"; //The combined logger configuration will be stored in this file

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Thing Plus C hardware specifics

const int sd_cs = 5; //Define the microSD chip select pin (e.g. pin 5 on the Thing Plus C)

int qwiicPower = 0; //Thing Plus C digital pin 0 is connected to the v-reg that controls the Qwiic power. It is also connected to the BOOT button.

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // The Thing Plus C STAT LED is connected to digital pin 13
#endif

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// The sensor data log file

// Define the SdFat log file type - use the same type as the Qwiic Universal Auto-Detect library
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

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// ESP32 WiFi and RTC

#include <WiFi.h>
#include "time.h"
#include "sntp.h"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void setup()
{
  serialQUAD.begin(115200);
  delay(1000);
  serialQUAD.println(F("SparkFun Qwiic Universal Auto-Detect - ESP32 Thing Plus C Logger - with menus and RTC support"));
  serialQUAD.println();
  serialQUAD.println(F("A list of rules for your time zone can be obtained from:"));
  serialQUAD.println(F("https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h"));
  serialQUAD.println();

  // Enable power for the Qwiic bus
  // The input pull-up is enough to enable the regulator
  // We can then also use the BOOT button as a "stop logging" button
  pinMode(qwiicPower, INPUT_PULLUP);

  // Flash LED_BUILTIN each time we write to microSD
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Wire.begin();

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Detect, initialize and configure the sensors

  mySensors.setWirePort(Wire); // Tell the sensors instance which Wire port to use

  mySensors.enableDebugging(serialQUAD); // Uncomment this line to enable debug messages on serialQUAD

  mySensors.setMenuPort(serialQUAD); // Use serialQUAD for the logging menu
  
  mySensors.detectSensors(); // Detect which sensors are connected

  mySensors.beginSensors(); // Begin all the sensors

  mySensors.initializeSensors(); // Initialize all the sensors

  if (!mySensors.beginStorage(sd_cs, (const char *)loggerConfigurationFileName)) // Begin SdFat storage
    serialQUAD.println(F("beginStorage failed! You will not be able to read or write the sensor configuration..."));

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Create the menu

  // The Arduino IDE Serial Monitor does not support backspace so - by default - we need to clear text values when editing them.
  // The user enters the entire text value each time.
  // If the user is using a terminal emulator which supports backspace (e.g. Tera Term) then
  // setting supportsBackspace to true will allow the existing text to be edited.
  //mySensors.theMenu.setMenuPort(serialQUAD, true); // Uncomment this line to use serialQUAD for the menu - on a terminal emulator which supports backspace

  while (mySensors.theMenu._menuPort->available()) // Clear the menu serial buffer
    mySensors.theMenu._menuPort->read();

  // Create the menu - using unique menu item names.
  // You can duplicate NONE or SUB_MENU_START items (e.g. "Wifi Menu") but all other items must be unique

  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("====", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Open the sensor logging menu", openSensorLoggingMenu);
  mySensors.theMenu.addMenuItem("Open the sensor settings menu", openSensorSettingMenu);
  mySensors.theMenu.addMenuItem("Write the logger configuration to file", writeLoggerConfig);
  mySensors.theMenu.addMenuItem("Read the logger configuration from file", readLoggerConfig);
  mySensors.theMenu.addMenuItem("Stop logging", stopLogging);
  mySensors.theMenu.addMenuItem("Open new log file", newLogFile);
  mySensors.theMenu.addMenuItem("Set RTC using NTP over WiFi", setRTC);

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
  // A list of rules for your zone can be obtained from: https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
  //mySensors.theMenu.setMenuItemVariable("TimeZone Rule", "GMT0BST,M3.5.0/1,M10.5.0"); // TimeZone rule for Europe/London
  //mySensors.theMenu.setMenuItemVariable("TimeZone Rule", "CET-1CEST,M3.5.0,M10.5.0/3"); // TimeZone rule for Europe/Rome including daylight adjustment rules
  mySensors.theMenu.setMenuItemVariable("TimeZone Rule", "MST7MDT,M3.2.0,M11.1.0"); // TimeZone rule for America/Denver
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END); // End of the WiFi sub-menu
  
  // Logging sub-menu: set the logging interval etc.
  mySensors.theMenu.addMenuItem("Logging Menu", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START); // Start of the logging sub-menu
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Logging Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("============", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Logging interval (ms)", SFE_QUAD_MENU_VARIABLE_TYPE_ULONG);
  SFE_QUAD_Menu_Every_Type_t defaultValue;
  defaultValue.ULONG = 1000;
  mySensors.theMenu.setMenuItemVariable("Logging interval (ms)", &defaultValue); // Set the default logging interval - this will be updated by readLoggerConfig
  defaultValue.ULONG = 10;
  mySensors.theMenu.setMenuItemVariableMin("Logging interval (ms)", &defaultValue); // Set the minimum logging interval - this will be updated by readLoggerConfig
  defaultValue.ULONG = 3600000;
  mySensors.theMenu.setMenuItemVariableMax("Logging interval (ms)", &defaultValue); // Set the maximum logging interval - this will be updated by readLoggerConfig
  mySensors.theMenu.addMenuItem("Log Local Time", SFE_QUAD_MENU_VARIABLE_TYPE_BOOL);
  defaultValue.BOOL = 1;
  mySensors.theMenu.setMenuItemVariable("Log Local Time", &defaultValue);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END); // End of the logging sub-menu
  
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  // End of the menu

  serialQUAD.println(F("Press any key to open the menu"));
  serialQUAD.println();

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Read the configuration file, configure the sensors and update the menu variables. Do this _after_ the menu has been created.

  readLoggerConfig();

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Open the log file

  serialQUAD.println(F("Finding the next available log file..."));
  
  // mySensors.beginStorage has done the sd.begin for us
  // Open the next available log file
  onlineDataLogging = false;
  if (findNextAvailableLog(sensorDataFileName, "dataLog", false)) // Do not reuse empty files - to save time
  {
    // O_CREAT - create the file if it does not exist
    // O_APPEND - seek to the end of the file prior to each write
    // O_WRITE - open for write
    onlineDataLogging = sensorDataFile.open(sensorDataFileName, O_CREAT | O_APPEND | O_WRITE) == true;
  }
  
  if (onlineDataLogging)
  {
    serialQUAD.print(F("Logging sensor data to: "));
    serialQUAD.println(sensorDataFileName);
  }
  else
  {
    serialQUAD.println(F("Failed to open sensor data log file"));
  }

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Print and write the sensor helper text

  SFE_QUAD_Menu_Every_Type_t logLocalTime;
  mySensors.theMenu.getMenuItemVariable("Log Local Time", &logLocalTime);    
  
  mySensors.getSensorNames(); // Print the sensor names helper
  if (logLocalTime.BOOL)
    serialQUAD.print(F("Local_Time,"));
  serialQUAD.println(mySensors.readings);
  if (onlineDataLogging)
  {
    if (logLocalTime.BOOL)
      sensorDataFile.print(F("Local_Time,"));
    sensorDataFile.println(mySensors.readings);
  }

  mySensors.getSenseNames(); // Print the sense names helper
  if (logLocalTime.BOOL)
    serialQUAD.print(F("YYYY/MM/DD HH:MM:SS,"));
  serialQUAD.println(mySensors.readings);
  if (onlineDataLogging)
  {
    if (logLocalTime.BOOL)
      sensorDataFile.print(F("YYYY/MM/DD HH:MM:SS,"));
    sensorDataFile.println(mySensors.readings);
  }
    
} // /setup()

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void loop()
{

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Logging interval - read the sensors every loggingInterval milliseconds
  
  static unsigned long lastRead;

  SFE_QUAD_Menu_Every_Type_t loggingInterval;
  mySensors.theMenu.getMenuItemVariable("Logging interval (ms)", &loggingInterval); // Get the logging interval from theMenu
  if (millis() > (lastRead + loggingInterval.ULONG)) // Is it time to read the sensors?
  {
    lastRead = millis(); // Update the time of the last read
    
    mySensors.getSensorReadings(); // Read everything from all sensors
  
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
      sensorDataFile.sync(); // This will help prevent data loss if the power is removed during logging
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Menu
  
  if (mySensors.theMenu._menuPort->available()) // Has the user pressed a key?
  {
    mySensors.theMenu.openMenu(); // If so, open the menu
  }

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Stop logging button
  
  if (digitalRead(qwiicPower) == LOW) // Check if the user has pressed the stop logging button
  {
    if (onlineDataLogging)
    {
      sensorDataFile.close();
      onlineDataLogging = false;
      serialQUAD.println(F("Log file closed"));
    }
  }
  
} // /loop()

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Returns next available log file name
bool findNextAvailableLog(char *newFileName, const char *fileLeader, bool reuseEmpty)
{
// Define the SdFat log file type - use the same type as the Qwiic Universal Auto-Detect library
#if SFE_QUAD_SD_FAT_TYPE == 1
  File32 newFile;
#elif SFE_QUAD_SD_FAT_TYPE == 2
  ExFile newFile;
#elif SFE_QUAD_SD_FAT_TYPE == 3
  FsFile newFile;
#else // SD_FAT_TYPE == 0
  File newFile;
#endif  // SD_FAT_TYPE

  //Search for next available log file
  int newFileNumber = 0;
  bool keepGoing = true;
  while (keepGoing)
  {
    char newFileNumberStr[6]; // Five digits plus null
    if (newFileNumber < 10)
      sprintf(newFileNumberStr, "0000%d", newFileNumber);
    else if (newFileNumber < 100)
      sprintf(newFileNumberStr, "000%d", newFileNumber);
    else if (newFileNumber < 1000)
      sprintf(newFileNumberStr, "00%d", newFileNumber);
    else if (newFileNumber < 10000)
      sprintf(newFileNumberStr, "0%d", newFileNumber);
    else
      sprintf(newFileNumberStr, "%d", newFileNumber);
    sprintf(newFileName, "%s%s.csv", fileLeader, newFileNumberStr); //Splice the new file number into this file name. Max no. is 99999.

    // Use the same instance of sd as the Qwiic Universal Auto-Detect library
    if (mySensors.sd.exists(newFileName) == false)
    {
      keepGoing = false; //File name not found so we will use it.
    }
    else
    {
      if (reuseEmpty) // If resuseEmpty is true, open the file and see if it is empty. If it is, use it. Slow but efficient...
      {
        newFile.open(newFileName, O_READ); // Open the existing file
        size_t fileSize = newFile.fileSize(); // Get its size
        newFile.close(); // Close the file
        if (fileSize == 0)
          keepGoing = false; // File is empty so we will use it
      }
    }

    newFileNumber++; //Get ready to try the next number
    
    if (newFileNumber >= 100000)
      keepGoing = false; // Have we hit the maximum number of files?
  }

  if (newFileNumber >= 100000) // Have we hit the maximum number of files?
  {
    return (false);
  }

  return (true);
} // /findNextAvailableLog

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Open the sensor logging menu. Afterwards, open a new log file and write the helper text to it - it may have changed
void openSensorLoggingMenu(void)
{
  mySensors.loggingMenu(); // Open the _sensors_ logging menu

  newLogFile(); // Open a new log file - the sense names may have changed
  
  mySensors.getSensorNames(); // Print the sensor names helper - it may have changed
  serialQUAD.println(mySensors.readings);
  if (onlineDataLogging)
    sensorDataFile.println(mySensors.readings);

  mySensors.getSenseNames(); // Print the sense names helper - it may have changed
  serialQUAD.println(mySensors.readings);
  if (onlineDataLogging)
    sensorDataFile.println(mySensors.readings);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Open the sensor setting menu
void openSensorSettingMenu(void)
{
  mySensors.settingMenu(); // Open the _sensors_ setting menu
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Read the configuration file, configure the sensors and update the menu variables
void readLoggerConfig(void)
{
  bool success = mySensors.readConfigurationFromStorage();
  success &= mySensors.applySensorAndMenuConfiguration();
  if (success)
    serialQUAD.println(F("Logger configuration read from storage"));
  else
    serialQUAD.println(F("Unable to read logger configuration from storage"));
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Write the logger configuration to microSD
void writeLoggerConfig(void)
{
  bool success = mySensors.getSensorAndMenuConfiguration();
  success &= mySensors.writeConfigurationToStorage(false); // Set append to false - overwrite the configuration
  if (success)
    serialQUAD.println(F("Logger configuration written to file"));
  else
    serialQUAD.println(F("Unable to write logger configuration to file"));
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Stop logging - close the log file
void stopLogging(void)
{
  if (onlineDataLogging)
  {
    sensorDataFile.close();
    onlineDataLogging = false;
    serialQUAD.println(F("Log file closed"));
  }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Open a new log file
void newLogFile(void)
{
  if (onlineDataLogging)
  {
    sensorDataFile.close();

    serialQUAD.println(F("Finding the next available log file..."));
    
    if (findNextAvailableLog(sensorDataFileName, "dataLog", false)) // Do not reuse empty files - to save time
    {
      onlineDataLogging = sensorDataFile.open(sensorDataFileName, O_CREAT | O_APPEND | O_WRITE) == true;
    }
    
    if (onlineDataLogging)
    {
      serialQUAD.print(F("Logging sensor data to: "));
      serialQUAD.println(sensorDataFileName);
    }
    else
    {
      serialQUAD.println(F("Failed to open new sensor data log file"));
    }
  }
  else
  {
    serialQUAD.println(F("Data logging is not online"));
  }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Connect to WiFi and be ready to set RTC using NTP
// Code taken mostly from Espressif's SimpleTime example
void setRTC(void)
{
  // set notification call-back function
  sntp_set_time_sync_notification_cb( timeavailable );

  /**
   * NTP server address could be aquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 aquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE aquired NTP server address
   */
  sntp_servermode_dhcp(1);    // (optional)

  /**
   * A more convenient approach to handle TimeZones with daylightOffset 
   * would be to specify a environmnet variable with TimeZone definition including daylight adjustmnet rules.
   * A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
   */
  char time_zone[50];
  mySensors.theMenu.getMenuItemVariable("TimeZone Rule", time_zone, 50);

  char ntpServer1[20];
  mySensors.theMenu.getMenuItemVariable("NTP Server 1", ntpServer1, 20);
  char ntpServer2[20];
  mySensors.theMenu.getMenuItemVariable("NTP Server 2", ntpServer2, 20);

  configTzTime(time_zone, ntpServer1, ntpServer2);    

  //connect to WiFi

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

  if (WiFi.status() == WL_CONNECTED)
  {
    serialQUAD.println(" CONNECTED");
    struct tm timeinfo; // Doing a getLocalTime immediately after the WiFi connects seems critical to NTP being successful?!
    getLocalTime(&timeinfo);
  }
  else
  {
    serialQUAD.println(" Connection failed! Please try again...");
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback function (gets called when time is adjusted via NTP)
// (Does not get called if time is not adjusted...)
void timeavailable(struct timeval *t)
{
  serialQUAD.println("Got time adjustment from NTP");
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

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
