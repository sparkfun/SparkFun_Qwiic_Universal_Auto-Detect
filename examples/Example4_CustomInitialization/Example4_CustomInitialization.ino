/*
  SFE QUAD Custom Initialization
  By: Paul Clark
  SparkFun Electronics
  Date: May 2022
  
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

const char configurationFileName[] = "/OLconfig.csv"; //The sensor configuration will be stored in this file

//#define sd_cs SS // microSD chip select - this should work on most boards
//#define sd_cs CS // Some boards use CS instead
const int sd_cs = 5; //Uncomment this line to define a specific pin for the chip select (e.g. pin 5 on the Thing Plus C)


int qwiicPower = 0; //Thing Plus C digital pin 0 is connected to the v-reg that controls the Qwiic power


void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("SparkFun Qwiic Universal Auto-Detect Example"));

  pinMode(qwiicPower, OUTPUT); // Enable power for the Qwiic bus
  digitalWrite(qwiicPower, HIGH);

  Wire.begin();

  mySensors.setWirePort(Wire); // Tell the class which Wire port to use

  mySensors.enableDebugging(Serial); // Uncomment this line to enable debug messages on Serial

  mySensors.setMenuPort(Serial); // Use Serial for the logging menu

  while (mySensors._menuPort->available()) // Clear the menu serial buffer
    mySensors._menuPort->read();

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
    Serial.println(F("beginStorage failed! You will not be able to write or read the sensor configuration..."));

  Serial.println(F("Press L to open the logging menu"));
  Serial.println(F("Press S to open the setting menu"));
  Serial.println(F("Press W to write the sensor configuration to file"));
  Serial.println(F("Press R to read the configuration file and configure the sensors"));

  mySensors.getSensorNames(); // Print the sensor names helper
  Serial.println(mySensors.readings);
  mySensors.getSenseNames(); // Print the sense names helper
  Serial.println(mySensors.readings);
}

void loop()
{
  mySensors.getSensorReadings(); // Read everything from all sensors

  Serial.println(mySensors.readings);

  if (mySensors._menuPort->available())
  {
    char choice = mySensors._menuPort->read();
    
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
      mySensors.getSensorConfiguration();
      mySensors.writeConfigurationToStorage(false); // Set append to false - overwrite the configuration
    }

    else if ((choice == 'r') || (choice == 'R'))
    {
      mySensors.readConfigurationFromStorage();
      mySensors.applySensorConfiguration();
    }
  }

  delay(500);
}
