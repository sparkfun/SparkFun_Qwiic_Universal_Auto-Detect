/*
  SFE QUAD ESP32 Thing Plus C
  By: Paul Clark
  SparkFun Electronics
  Date: May 2022
  
  This code is a proof-of-concept demonstration of a scalable I2C sensing and logging device,
  based initially on the SparkFun Thing Plus C - ESP32 WROOM (SPX-18018):
  https://www.sparkfun.com/products/18018
  
  This example uses the SparkFun Thing Plus C microSD to save the sensor configuration.
  writeConfigurationToStorage and readConfigurationFromStorage write to and read from a file called "OLconfig.csv" on microSD.
  It can be adapted for any platform which supports the SD library.
  Later examples show how to use ESP32 LittleFS, ESP32 EEPROM and SAMD21 EEPROM to do the same thing.

  If you are using the SparkFun Thing Plus C, please make sure you have the latest Espressif Systems esp32 board package installed.
  Click here to get the board package:  http://boardsmanager/All#esp32
  Select the "ESP32 Dev Module" as the board.
                                  
  License: MIT
  Please see LICENSE.md for more details
  
*/

#include <SD.h> // Include SD.h to enable support for SFE_QUAD_Sensors__SD. Do this before #include "SFE_QUAD_Sensors.h"

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect

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
