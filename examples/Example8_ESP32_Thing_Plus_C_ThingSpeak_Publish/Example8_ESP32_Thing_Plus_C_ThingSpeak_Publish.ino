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

  This example shows how to publish your sensor data to ThingSpeak
                                  
  License: MIT
  Please see LICENSE.md for more details
  
*/


#define serialQUAD Serial // Use Serial for the Logger and Qwiic Universal Auto-Detect menus

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Use EEPROM for configuration storage

#include <EEPROM.h> // Include EEPROM.h to enable support for SFE_QUAD_Sensors__EEPROM. Do this before #include "SFE_QUAD_Sensors.h"

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect

SFE_QUAD_Sensors__EEPROM mySensors;

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Thing Plus C hardware specifics

int qwiicPower = 0; //Thing Plus C digital pin 0 is connected to the v-reg that controls the Qwiic power. It is also connected to the BOOT button.

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // The Thing Plus C STAT LED is connected to digital pin 13
#endif

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// ESP32 WiFi

#include <WiFi.h>

WiFiClient myClient;

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// ThingSpeak Arduino Library

#include <ThingSpeak.h> // Click here to get the library:  http://librarymanager/All#ThingSpeak

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void setup()
{
  serialQUAD.begin(115200);
  delay(1000);
  serialQUAD.println(F("SparkFun Qwiic Universal Auto-Detect - ESP32 Thing Plus C - Publish to ThingSpeak"));
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

  //mySensors.enableDebugging(serialQUAD); // Uncomment this line to enable debug messages on serialQUAD

  mySensors.setMenuPort(serialQUAD); // Use serialQUAD for the logging menu
  
  mySensors.detectSensors(); // Detect which sensors are connected

  mySensors.beginSensors(); // Begin all the sensors

  mySensors.initializeSensors(); // Initialize all the sensors

  if (!mySensors.beginStorage()) // Begin EEPROM storage
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
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Write the logger configuration to EEPROM", writeLoggerConfig);
  mySensors.theMenu.addMenuItem("Read the logger configuration from EEPROM", readLoggerConfig);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);

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
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Connect to WiFi", wifiConnect);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END); // End of the WiFi sub-menu
  
  // ThingSpeak sub-menu: set the publish interval etc.
  mySensors.theMenu.addMenuItem("ThingSpeak Menu", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("ThingSpeak Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("============", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Write API Key", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  mySensors.theMenu.setMenuItemVariable("Write API Key", "REPLACE_WITH_YOUR_KEY"); // Set the default API Key - this will be updated by readLoggerConfig
  mySensors.theMenu.addMenuItem("Channel ID", SFE_QUAD_MENU_VARIABLE_TYPE_ULONG);
  SFE_QUAD_Menu_Every_Type_t defaultValue;
  defaultValue.ULONG = 1;
  mySensors.theMenu.setMenuItemVariable("Channel ID", &defaultValue); // Set the default channel number
  mySensors.theMenu.addMenuItem("Publish interval (ms)", SFE_QUAD_MENU_VARIABLE_TYPE_ULONG);
  defaultValue.ULONG = 60000;
  mySensors.theMenu.setMenuItemVariable("Publish interval (ms)", &defaultValue); // Set the default publish interval
  defaultValue.ULONG = 15000;
  mySensors.theMenu.setMenuItemVariableMin("Publish interval (ms)", &defaultValue); // Minimum interval is 15 seconds
  defaultValue.ULONG = 3600000;
  mySensors.theMenu.setMenuItemVariableMax("Publish interval (ms)", &defaultValue);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END); // End of the sub-menu
  
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  // End of the menu

  serialQUAD.println(F("Press any key to open the menu"));
  serialQUAD.println();

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Read the configuration file, configure the sensors and update the menu variables. Do this _after_ the menu has been created.

  readLoggerConfig();

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Print the sensor helper text

  mySensors.getSensorNames(); // Print the sensor names helper
  serialQUAD.println(mySensors.readings);

  mySensors.getSenseNames(); // Print the sense names helper
  serialQUAD.println(mySensors.readings);
    
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Get ready to connect to WiFi

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(myClient);  // Initialize ThingSpeak

} // /setup()

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void loop()
{

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Publish interval - read the sensors every publishInterval milliseconds
  
  static unsigned long lastRead = 0;

  SFE_QUAD_Menu_Every_Type_t publishInterval;
  mySensors.theMenu.getMenuItemVariable("Publish interval (ms)", &publishInterval); // Get the publish interval from theMenu
  if (millis() > (lastRead + publishInterval.ULONG)) // Is it time to read the sensors?
  {
    lastRead = millis(); // Update the time of the last read
    
    mySensors.getSensorReadings(); // Read everything from all sensors
  
    serialQUAD.println(mySensors.readings); // Print the sensor readings

    // ThingSpeak channels can have up to 8 fields
    // Extract up to 8 Strings from mySensors.readings using strtok_r
    unsigned int fieldNum = 1;
    char *preserve;
    char *field = strtok_r(mySensors.readings, ",", &preserve);
    int result = TS_OK_SUCCESS;
    
    while ((field != NULL) && (fieldNum <= 8) && (result == TS_OK_SUCCESS))
    {
      //serialQUAD.println("Adding field " + String(field));
      result = ThingSpeak.setField(fieldNum, String(field));
      field = strtok_r(NULL, ",", &preserve);
      fieldNum++;
    }

    if (result == TS_OK_SUCCESS)
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        serialQUAD.println(F("WiFi is not connected. Attempting to reconnect..."));
        wifiConnect();
      }

      if (WiFi.status() == WL_CONNECTED)
      {
        serialQUAD.println(F("Publishing data to ThingSpeak"));

        SFE_QUAD_Menu_Every_Type_t channelNumber;
        mySensors.theMenu.getMenuItemVariable("Channel ID", &channelNumber); // Get the channel number

        char myWriteAPIKey[33];
        mySensors.theMenu.getMenuItemVariable("Write API Key", myWriteAPIKey, 33);
        
        result = ThingSpeak.writeFields(channelNumber.ULONG, myWriteAPIKey);

        if (result == TS_OK_SUCCESS)
          serialQUAD.println(F("Channel update was successful"));
        else
          serialQUAD.println("Channel update failed with error " + String(result));
      }
    }
    else
    {
      serialQUAD.println(F("ThingSpeak setField failed. Aborting..."));
    } 
  }

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Menu
  
  if (mySensors.theMenu._menuPort->available()) // Has the user pressed a key?
  {
    mySensors.theMenu.openMenu(); // If so, open the menu
  }

} // /loop()

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Open the sensor logging menu. Afterwards, print the helper text - it may have changed
void openSensorLoggingMenu(void)
{
  mySensors.loggingMenu(); // Open the _sensors_ logging menu

  mySensors.getSensorNames(); // Print the sensor names helper - it may have changed
  serialQUAD.println(mySensors.readings);

  mySensors.getSenseNames(); // Print the sense names helper - it may have changed
  serialQUAD.println(mySensors.readings);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Open the sensor setting menu
void openSensorSettingMenu(void)
{
  mySensors.settingMenu(); // Open the _sensors_ setting menu
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Read the configuration, configure the sensors and update the menu variables
void readLoggerConfig(void)
{
  bool success = mySensors.readConfigurationFromStorage();
  success &= mySensors.applySensorAndMenuConfiguration();
  if (success)
    serialQUAD.println(F("Configuration read from EEPROM"));
  else
    serialQUAD.println(F("Unable to read configuration from EEPROM"));
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Write the logger configuration to EEPROM
void writeLoggerConfig(void)
{
  bool success = mySensors.getSensorAndMenuConfiguration();
  success &= mySensors.writeConfigurationToStorage(false); // Set append to false - overwrite the configuration
  if (success)
    serialQUAD.println(F("Configuration written to EEPROM"));
  else
    serialQUAD.println(F("Unable to write configuration to EEPROM"));
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Connect to WiFi
void wifiConnect(void)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    serialQUAD.println("WiFi is already connected! Disconnecting...");
    
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(5000);    
  }

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
    serialQUAD.println(" CONNECTED");
  else
    serialQUAD.println(" Connection failed! Please try again...");
}
