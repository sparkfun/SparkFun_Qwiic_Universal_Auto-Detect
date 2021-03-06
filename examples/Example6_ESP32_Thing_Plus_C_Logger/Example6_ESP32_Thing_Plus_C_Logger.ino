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

  The sensor data and configuration are written to microSD using SdFat
                                  
  License: MIT
  Please see LICENSE.md for more details
  
*/


#include <SdFat.h> // Include SdFat.h to enable support for SFE_QUAD_Sensors__SdFat. Do this before #include "SFE_QUAD_Sensors.h"

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect

SFE_QUAD_Sensors__SdFat mySensors;


const char configurationFileName[] = "Config.csv"; //The sensor configuration will be stored in this file

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
  delay(1000); //Give the sensors time to power on

  Serial.begin(115200);
  Serial.println(F("SparkFun Qwiic Universal Auto-Detect - ESP32 Thing Plus C Logger"));

  // Enable power for the Qwiic bus
  // The input pull-up is enough to enable the regulator
  // We can then also use the BOOT button as a "stop logging" button
  pinMode(qwiicPower, INPUT_PULLUP);

  // Flash LED_BUILTIN each time we write to microSD
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Wire.begin(); // Begin the Wire port at the default speed (usually 100kHz)

  mySensors.setWirePort(Wire); // Tell the class which Wire port to use

  //mySensors.enableDebugging(Serial); // Uncomment this line to enable debug messages on Serial

  mySensors.setMenuPort(Serial); // Use Serial for the logging menu

  mySensors.detectSensors(); // Detect which sensors are connected

  mySensors.beginSensors(); // Begin all the sensors

  mySensors.initializeSensors(); // Initialize all the sensors

  if (!mySensors.beginStorage(sd_cs, (const char *)configurationFileName))
  {
    Serial.println(F("beginStorage failed! You will not be able to read or write the sensor configuration..."));
  }

  // Read the configuration file and configure the sensors
  readConfig();

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
  
  if (onlineDataLogging)
  {
    Serial.print(F("Logging sensor data to: "));
    Serial.println(sensorDataFileName);
  }
  else
  {
    Serial.println(F("Failed to open sensor data log file"));
  }

  // Create the menu
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("====", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Logging interval (ms)", SFE_QUAD_MENU_VARIABLE_TYPE_ULONG);  
  SFE_QUAD_Menu_Every_Type_t defaultValue;
  defaultValue.ULONG = 1000;
  mySensors.theMenu.setMenuItemVariable("Logging interval (ms)", &defaultValue); // Set the default logging interval - this will be updated by readConfig
  defaultValue.ULONG = 100;
  mySensors.theMenu.setMenuItemVariableMin("Logging interval (ms)", &defaultValue); // Set the minimum logging interval - this will be updated by readConfig
  defaultValue.ULONG = 3600000;
  mySensors.theMenu.setMenuItemVariableMax("Logging interval (ms)", &defaultValue); // Set the maximum logging interval - this will be updated by readConfig
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Stop logging", stopLogging);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Open the sensor logging menu", openLoggingMenu);
  mySensors.theMenu.addMenuItem("Open the sensor settings menu", openSettingMenu);
  mySensors.theMenu.addMenuItem("Write the logger configuration to SD", writeConfig);
  mySensors.theMenu.addMenuItem("Read the logger configuration from SD", readConfig);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  
  while (mySensors.theMenu._menuPort->available()) // Clear the menu serial buffer
    mySensors.theMenu._menuPort->read();

  Serial.println(F("Press any key to open the menu"));

  mySensors.getSensorNames(); // Print the sensor names helper
  Serial.println(mySensors.readings);
  if (onlineDataLogging)
    sensorDataFile.println(mySensors.readings);

  mySensors.getSenseNames(); // Print the sense names helper
  Serial.println(mySensors.readings);
  if (onlineDataLogging)
    sensorDataFile.println(mySensors.readings);
}

void loop()
{
  // Logging interval - read the sensors every loggingInterval milliseconds
  static unsigned long lastRead = 0;
  SFE_QUAD_Menu_Every_Type_t loggingInterval;
  mySensors.theMenu.getMenuItemVariable("Logging interval (ms)", &loggingInterval); // Get the logging interval from theMenu
  if (millis() > (lastRead + loggingInterval.ULONG)) // Is it time to read the sensors?
  {
    lastRead = millis(); // Update the time of the last read
    
    mySensors.getSensorReadings(); // Read everything from all sensors
  
    Serial.println(mySensors.readings);
    
    if (onlineDataLogging)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      sensorDataFile.println(mySensors.readings); // Write the data to file
      sensorDataFile.sync(); // This will help prevent data loss if the power is removed during logging
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  if (mySensors.theMenu._menuPort->available()) // Has the user pressed a key?
  {
    mySensors.theMenu.openMenu(); // If so, open the menu
  }

  if (digitalRead(qwiicPower) == LOW) // Check if the user has pressed the stop logging button
  {
    stopLogging();
  }
}

void openLoggingMenu(void)
{
  mySensors.loggingMenu();
  
  if (onlineDataLogging) // Open a new log file - the sense names may have changed
  {
    sensorDataFile.close();
    
    Serial.println(F("Finding the next available log file..."));
    
    if (findNextAvailableLog(sensorDataFileName, "dataLog", false)) // Do not reuse empty files - to save time
    {
      onlineDataLogging = sensorDataFile.open(sensorDataFileName, O_CREAT | O_APPEND | O_WRITE) == true;
    }
    
    if (onlineDataLogging)
    {
      Serial.print(F("Logging sensor data to: "));
      Serial.println(sensorDataFileName);
    }
    else
    {
      Serial.println(F("Failed to open new sensor data log file"));
    }
  }

  mySensors.getSensorNames(); // Print the sensor names helper - it may have changed
  Serial.println(mySensors.readings);
  if (onlineDataLogging)
    sensorDataFile.println(mySensors.readings);

  mySensors.getSenseNames(); // Print the sense names helper - it may have changed
  Serial.println(mySensors.readings);
  if (onlineDataLogging)
    sensorDataFile.println(mySensors.readings);
}

void openSettingMenu(void)
{
  mySensors.settingMenu();
}

void writeConfig(void)
{
  mySensors.getSensorAndMenuConfiguration();
  if (mySensors.writeConfigurationToStorage(false)) // Set append to false - overwrite the configuration
    Serial.println(F("Logger configuration written to file"));
}

void readConfig(void)
{
  mySensors.readConfigurationFromStorage();
  mySensors.applySensorAndMenuConfiguration();
}

void stopLogging(void)
{
  if (onlineDataLogging)
  {
    sensorDataFile.close();
    onlineDataLogging = false;
    Serial.println(F("Log file closed"));
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
