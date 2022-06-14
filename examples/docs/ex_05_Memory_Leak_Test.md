# Example 5 - Memory Leak Test

In most Arduino code, it is usual to declare an object at a global scope (after the include file declaration), not within the ```setup()``` or ```loop()``` functions.
Because the object is global, both ```setup()``` and ```loop()``` can access it.

But you do not have to do it this way. You can, if you want to, declare the SparkFun Qwiic Universal Auto-Detect object ```mySensors``` _inside_ the ```loop()```:

```C++
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
```

The ```mySensors``` object is then _constructed_ (_instantiated_) at the start of the loop, and is _destructed_ (_deleted_) at the end of the loop.
A new, fresh instance of ```mySensors``` is used each time around the loop. This is a very good test for a memory leak!

Sometimes code may not release all of the memory it is using correctly when the destructor is called. Those small pieces of unreleased memory
add up, increasing each time around the loop, and eventually you run out of memory. This is called a memory leak. If you run your code on a processor
with very limited memory (like an ATmega328P, as used on our original RedBoards) you will run out of memory quickly.

We run Example5 on a RedBoard to make sure the library has no memory leaks. (Actually, we did find one - and we fixed it!)

If you want to try this yourself, you will need to edit **SFE_QUAD_Sensors.h** and only include one or two individual sensors to let the code compile on the ATmega328P.
It does not have anywhere near enough program memory to hold all of the sensor libraries included in this library! You will find SFE_QUAD_Sensors.h in the **src** folder.

```C++
// To include all sensors, uncomment #define INCLUDE_SFE_QUAD_SENSOR_ALL

//#define INCLUDE_SFE_QUAD_SENSOR_ALL // Include all sensors

// To select which sensors to include:
//   comment #define INCLUDE_SFE_QUAD_SENSOR_ALL
//   uncomment one or more #define INCLUDE_SFE_QUAD_SENSOR_

//#define INCLUDE_SFE_QUAD_SENSOR_ADS122C04 // Include individual sensors
//#define INCLUDE_SFE_QUAD_SENSOR_AHT20
#define INCLUDE_SFE_QUAD_SENSOR_BME280
//#define INCLUDE_SFE_QUAD_SENSOR_CCS811_5A
//#define INCLUDE_SFE_QUAD_SENSOR_CCS811_5B
//#define INCLUDE_SFE_QUAD_SENSOR_LPS25HB
//#define INCLUDE_SFE_QUAD_SENSOR_MAX17048
//#define INCLUDE_SFE_QUAD_SENSOR_MCP9600
//#define INCLUDE_SFE_QUAD_SENSOR_MICROPRESSURE
//#define INCLUDE_SFE_QUAD_SENSOR_MS5637
//#define INCLUDE_SFE_QUAD_SENSOR_MS8607
//#define INCLUDE_SFE_QUAD_SENSOR_NAU7802
//#define INCLUDE_SFE_QUAD_SENSOR_QWIICBUTTON
//#define INCLUDE_SFE_QUAD_SENSOR_SCD30
//#define INCLUDE_SFE_QUAD_SENSOR_SCD40
//#define INCLUDE_SFE_QUAD_SENSOR_SDP3X
//#define INCLUDE_SFE_QUAD_SENSOR_SGP30
//#define INCLUDE_SFE_QUAD_SENSOR_SGP40
//#define INCLUDE_SFE_QUAD_SENSOR_SHTC3
//#define INCLUDE_SFE_QUAD_SENSOR_STC31
//#define INCLUDE_SFE_QUAD_SENSOR_TMP117
//#define INCLUDE_SFE_QUAD_SENSOR_UBLOX_GNSS
//#define INCLUDE_SFE_QUAD_SENSOR_VCNL4040
//#define INCLUDE_SFE_QUAD_SENSOR_VEML6075
//#define INCLUDE_SFE_QUAD_SENSOR_VEML7700
//#define INCLUDE_SFE_QUAD_SENSOR_VL53L1X
```

