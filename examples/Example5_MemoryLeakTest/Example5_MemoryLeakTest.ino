/*
  SFE QUAD Memory Leak Test
  By: Paul Clark
  SparkFun Electronics
  Date: May 2022
  
  This example checks for a memory leak by instantiating mySensors inside the main loop.
  It should run forever, even on boards with limited RAM like the ATmega328P (Uno).
  You will need to edit SFE_QUAD_Sensors.h and only include one or two individual sensors to let the code compile on the ATmega328P.
                                  
  License: MIT
  Please see LICENSE.md for more details
  
*/

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect


int qwiicPower = 0; //Thing Plus C digital pin 0 is connected to the v-reg that controls the Qwiic power


void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("SparkFun Qwiic Universal Auto-Detect Example"));

  pinMode(qwiicPower, OUTPUT); // Enable power for the Qwiic bus
  digitalWrite(qwiicPower, HIGH);

  Wire.begin();

  while(Serial.available()) // Clear the serial buffer
    Serial.read();

  Serial.println(F("Press any key to begin..."));

  while(!Serial.available()) // Wait for the user to press a key
    ;
}

void loop()
{
  SFE_QUAD_Sensors mySensors;
  
  mySensors.setWirePort(Wire); // Tell the class which Wire port to use

  mySensors.enableDebugging(Serial); // Uncomment this line to enable debug messages on Serial

  mySensors.detectSensors(); // Detect which sensors are connected

  mySensors.beginSensors(); // Begin all the sensors

  mySensors.initializeSensors(); // Initialize all the sensors

  mySensors.getSensorNames(); // Print the sensor names helper
  Serial.println(mySensors.readings);
  mySensors.getSenseNames(); // Print the sense names helper
  Serial.println(mySensors.readings);

  mySensors.getSensorReadings(); // Read everything from all sensors
  Serial.println(mySensors.readings);

  delay(500);
}
