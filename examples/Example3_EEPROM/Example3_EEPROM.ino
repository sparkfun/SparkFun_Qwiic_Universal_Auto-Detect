/*
  SFE QUAD EEPROM
  By: Paul Clark
  SparkFun Electronics
  Date: June 2022
  
  This code is a proof-of-concept demonstration of a scalable I2C sensing and logging device,
  based initially on the SparkFun Thing Plus C - ESP32 WROOM (SPX-18018):
  https://www.sparkfun.com/products/18018
  
  This example is written for the SparkFun Thing Plus C (ESP32) but will work on any board which supports EEPROM.
  writeConfigurationToStorage and readConfigurationFromStorage write to and read from EEPROM.
  Please see the previous examples if you want to use microSD or LittleFS to store the configuration.
                                  
  If you are using the SparkFun Thing Plus C, please make sure you have the latest Espressif Systems esp32 board package installed.
  Click here to get the board package:  http://boardsmanager/All#esp32
  Select the "ESP32 Dev Module" as the board.
                                  
  License: MIT
  Please see LICENSE.md for more details
  
*/

#include <EEPROM.h> // Include EEPROM.h to enable support for SFE_QUAD_Sensors__EEPROM. Do this before #include "SFE_QUAD_Sensors.h"

//#include <FlashAsEEPROM_SAMD.h> // Khoi Hoang's FlashStorage_SAMD works well too:  http://librarymanager/All#FlashStorage_SAMD

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect

SFE_QUAD_Sensors__EEPROM mySensors;


int qwiicPower = 0; //Thing Plus C digital pin 0 is connected to the v-reg that controls the Qwiic power


void setup()
{
  delay(1000); //Give the sensors time to power on

  Serial.begin(115200);
  Serial.println(F("SparkFun Qwiic Universal Auto-Detect Example"));

  pinMode(qwiicPower, OUTPUT); // Enable power for the Qwiic bus
  digitalWrite(qwiicPower, HIGH);

  Wire.begin(); // Begin the Wire port at the default speed (usually 100kHz)

  mySensors.setWirePort(Wire); // Tell the class which Wire port to use

  //mySensors.enableDebugging(Serial); // Uncomment this line to enable debug messages on Serial

  mySensors.setMenuPort(Serial); // Use Serial for the menus

  mySensors.detectSensors(); // Detect which sensors are connected

  mySensors.beginSensors(); // Begin all the sensors

  mySensors.initializeSensors(); // Initialize all the sensors

  if (!mySensors.beginStorage())
  {
    Serial.println(F("beginStorage failed! You will not be able to write or read the sensor configuration..."));
  }

  // Create the menu
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("====", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Open the sensor logging menu", openLoggingMenu);
  mySensors.theMenu.addMenuItem("Open the sensor settings menu", openSettingMenu);
  mySensors.theMenu.addMenuItem("Write the sensor configuration to EEPROM", writeConfig);
  mySensors.theMenu.addMenuItem("Read the sensor configuration from EEPROM", readConfig);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  
  while (mySensors.theMenu._menuPort->available()) // Clear the menu serial buffer
    mySensors.theMenu._menuPort->read();

  Serial.println(F("Press any key to open the menu"));

  mySensors.getSensorNames(); // Print the sensor names helper
  Serial.println(mySensors.readings);
  
  mySensors.getSenseNames(); // Print the sense names helper
  Serial.println(mySensors.readings);
}

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
