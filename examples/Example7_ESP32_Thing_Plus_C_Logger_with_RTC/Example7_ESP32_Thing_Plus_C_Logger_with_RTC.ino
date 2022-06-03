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


#include <SdFat.h> // Include SdFat.h to enable support for SFE_QUAD_Sensors__SdFat. Do this before #include "SFE_QUAD_Sensors.h"

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect

SFE_QUAD_Sensors__SdFat mySensors;

#include "SFE_QUAD_Menus.h"

SFE_QUAD_Menu theMenu;

const char sensorConfigurationFileName[] = "Sensor_Config.csv"; //The sensor configuration will be stored in this file
const char loggerConfigurationFileName[] = "Logger_Config.csv"; //The logger configuration will be stored in this file

const int sd_cs = 5; //Define the microSD chip select pin (e.g. pin 5 on the Thing Plus C)


int qwiicPower = 0; //Thing Plus C digital pin 0 is connected to the v-reg that controls the Qwiic power. It is also connected to the BOOT button.

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // The Thing Plus C STAT LED is connected to digital pin 13
#endif


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


void setup()
{
  serialQUAD.begin(115200);
  delay(1000);
  serialQUAD.println(F("SparkFun Qwiic Universal Auto-Detect - ESP32 Thing Plus C Logger"));

  // Enable power for the Qwiic bus
  // The input pull-up is enough to enable the regulator
  // We can then also use the BOOT button as a "stop logging" button
  pinMode(qwiicPower, INPUT_PULLUP);

  // Flash LED_BUILTIN each time we write to microSD
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Wire.begin();

  mySensors.setWirePort(Wire); // Tell the class which Wire port to use

  //mySensors.enableDebugging(serialQUAD); // Uncomment this line to enable debug messages on serialQUAD

  mySensors.setMenuPort(serialQUAD); // Use serialQUAD for the logging menu
  theMenu.setMenuPort(serialQUAD); // Use serialQUAD for the menu

  theMenu.setDebugPort(serialQUAD); // Uncomment this line to enable menu debug messages on serialQUAD

  while (theMenu._menuPort->available()) // Clear the menu serial buffer
    theMenu._menuPort->read();

  mySensors.detectSensors(); // Detect which sensors are connected

  mySensors.beginSensors(); // Begin all the sensors

  mySensors.initializeSensors(); // Initialize all the sensors

  if (!mySensors.beginStorage(sd_cs, (const char *)sensorConfigurationFileName))
    serialQUAD.println(F("beginStorage failed! You will not be able to read or write the sensor configuration..."));

  // Read the configuration file and configure the sensors
  readConfigurationFromStorage();

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

  // Create the menu - using unique menu item names
  theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("====", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("Open the sensor logging menu", loggingMenu);
  theMenu.addMenuItem("Open the sensor settings menu", settingMenu);
  theMenu.addMenuItem("Write the sensor settings to file", writeConfigurationToStorage);
  theMenu.addMenuItem("Read the sensor settings from file", readConfigurationFromStorage);
  theMenu.addMenuItem("Stop logging", stopLogging);
  theMenu.addMenuItem("Open new log file", newLogFile);
  theMenu.addMenuItem("Set RTC using NTP over WiFi", setRTC);
  theMenu.addMenuItem("WiFi settings", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START);
  theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("WiFi Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("=========", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("WiFi SSID", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  theMenu.setMenuItemVariable("WiFi SSID", "T-Rex");
  theMenu.addMenuItem("WiFi password", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  theMenu.setMenuItemVariable("WiFi password", "Has Big Teeth");
  theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END);
  theMenu.addMenuItem("Logging settings", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START);
  theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("Logging Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("============", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  theMenu.addMenuItem("Logging interval (ms)", SFE_QUAD_MENU_VARIABLE_TYPE_ULONG);
  SFE_QUAD_Menu_Every_Type_t defaultValue;
  defaultValue.ULONG = 1000;
  theMenu.setMenuItemVariable("Logging interval (ms)", &defaultValue);
  theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END);
  theMenu.addMenuItem("Write the logger configuration to file", writeLoggerConfig);
  theMenu.addMenuItem("Read the logger configuration from file", readLoggerConfig);

  readLoggerConfig(); // Read any existing values from file
  
  serialQUAD.println(F("Press any key to open the menu"));

  mySensors.getSensorNames(); // Print the sensor names helper
  serialQUAD.println(mySensors.readings);
  if (onlineDataLogging)
    sensorDataFile.println(mySensors.readings);

  mySensors.getSenseNames(); // Print the sense names helper
  serialQUAD.println(mySensors.readings);
  if (onlineDataLogging)
    sensorDataFile.println(mySensors.readings);
}

void loop()
{
  static unsigned long lastRead;

  SFE_QUAD_Menu_Every_Type_t loggingInterval;
  theMenu.getMenuItemVariable("Logging interval (ms)", &loggingInterval);
  if (millis() > (lastRead + (unsigned long)loggingInterval.ULONG))
  {
    lastRead = millis(); // Update the time of the last read
    
    mySensors.getSensorReadings(); // Read everything from all sensors
  
    serialQUAD.println(mySensors.readings);
    
    if (onlineDataLogging)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      sensorDataFile.println(mySensors.readings); // Write the data to file
      sensorDataFile.sync(); // This will help prevent data loss if the power is removed during logging
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  if (theMenu._menuPort->available()) // Has the user pressed a key?
  {
    theMenu.openMenu();
  }

  if (digitalRead(qwiicPower) == LOW) // Check if the user has pressed the stop logging button
  {
    if (onlineDataLogging)
    {
      sensorDataFile.close();
      onlineDataLogging = false;
      serialQUAD.println(F("Log file closed"));
    }
  }
}

//Returns next available log file name
bool findNextAvailableLog(char *newFileName, const char *fileLeader, bool reuseEmpty)
{
// Define the log file type - use the same type as the Qwiic Universal Auto-Detect library
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
}

// Open the sensor logging menu. Afterwards, open a new log file and write the helper text to it - it may have changed
void loggingMenu(void)
{
  mySensors.loggingMenu();

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

// Open the sensor setting menu
void settingMenu(void)
{
  mySensors.settingMenu();
}

// Read the configuration file and configure the sensors
void readConfigurationFromStorage(void)
{
  mySensors.readConfigurationFromStorage();
  mySensors.applySensorConfiguration();
}

// Write the sensor configuration to microSD
void writeConfigurationToStorage(void)
{
  mySensors.getSensorConfiguration();
  if (mySensors.writeConfigurationToStorage(false)) // Set append to false - overwrite the configuration
    serialQUAD.println(F("Sensor configuration written to file"));
}

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

// Open a new log file
void newLogFile(void)
{
  if (onlineDataLogging)
  {
    sensorDataFile.close();
    
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

void setRTC(void)
{
  
}

void writeLoggerConfig(void)
{
// Define the log file type - use the same type as the Qwiic Universal Auto-Detect library
#if SFE_QUAD_SD_FAT_TYPE == 1
  File32 newFile;
#elif SFE_QUAD_SD_FAT_TYPE == 2
  ExFile newFile;
#elif SFE_QUAD_SD_FAT_TYPE == 3
  FsFile newFile;
#else // SD_FAT_TYPE == 0
  File newFile;
#endif  // SD_FAT_TYPE

  bool success = newFile.open(loggerConfigurationFileName, O_CREAT | O_WRONLY | O_TRUNC) == true;

  if (success)
  {
    success = theMenu.writeMenuVariables(&newFile);
    newFile.close();
  }

  if (!success)
    theMenu._menuPort->println(F("writeLoggerConfig failed!"));
}

void readLoggerConfig(void)
{
  
}
