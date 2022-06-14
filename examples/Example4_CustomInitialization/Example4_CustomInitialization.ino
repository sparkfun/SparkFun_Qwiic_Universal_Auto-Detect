/*
  SFE QUAD Custom Initialization
  By: Paul Clark
  SparkFun Electronics
  Date: June 2022
  
  This code is a proof-of-concept demonstration of a scalable I2C sensing and logging device,
  based initially on the SparkFun Thing Plus C - ESP32 WROOM (SPX-18018):
  https://www.sparkfun.com/products/18018
  
  This example demonstrates how to define a custom initialization function for a sensor - to avoid
  any changes you make to the library itself being overwritten when the library is updated.
                                  
  If you are using the SparkFun Thing Plus C, please make sure you have the latest Espressif Systems esp32 board package installed.
  Click here to get the board package:  http://boardsmanager/All#esp32
  Select the "ESP32 Dev Module" as the board.
                                  
  License: MIT
  Please see LICENSE.md for more details
  
*/

#include <SD.h> // Include SD.h to enable support for SFE_QUAD_Sensors__SD. Do this before #include "SFE_QUAD_Sensors.h"

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect

// ----------

// Define a custom initialization function for (e.g.) the u-blox GNSS

//         _____  You can use any name you like for the initializer. Use the same name when you call setCustomInitialize
//        /                                    _____  This _must_ be uint8_t sensorAddress, TwoWire &port, void *_classPtr
//        |                                   /
//        |                                   |
//        |              |--------------------^------------------------------|
void myCustomInitializer(uint8_t sensorAddress, TwoWire &port, void *_classPtr)
{
  SFE_UBLOX_GNSS *device = (SFE_UBLOX_GNSS *)_classPtr; // Use _classPtr to create a pointer to the sensor class
  
  device->setI2COutput(COM_TYPE_UBX); // Set the I2C port to output UBX only (turn off NMEA noise)
  device->saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save (only) the communications port settings to flash and BBR
  device->setAutoPVT(true); // Enable PVT at the navigation rate
  device->setMeasurementRate(250); // Set navigation rate to 4Hz
}

// ----------

SFE_QUAD_Sensors__SD mySensors;

const char configurationFileName[] = "/Config.csv"; //The sensor configuration will be stored in this file

//#define sd_cs SS // microSD chip select - this should work on most boards
//#define sd_cs CS // Some boards use CS instead
const int sd_cs = 5; //Uncomment this line to define a specific pin for the chip select (e.g. pin 5 on the Thing Plus C)


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

  // Set up a custom initializer for a u-blox GNSS module: with I2C address 0x42, on the main branch (Mux address 0, Mux port 0)
  // Do this after .beginSensors but before .initializeSensors
  mySensors.setCustomInitialize(&myCustomInitializer, "SFE_UBLOX_GNSS", 0x42, 0, 0);

  // Alternatively, we can use the same custom initializer for all instances of the sensor
  // Again, do this after .beginSensors but before .initializeSensors
  mySensors.setCustomInitialize(&myCustomInitializer, "SFE_UBLOX_GNSS");

  mySensors.initializeSensors(); // Initialize all the sensors

  if (!mySensors.beginStorage(sd_cs, (const char *)configurationFileName))
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
  mySensors.theMenu.addMenuItem("Write the sensor configuration to SD", writeConfig);
  mySensors.theMenu.addMenuItem("Read the sensor configuration from SD", readConfig);
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
