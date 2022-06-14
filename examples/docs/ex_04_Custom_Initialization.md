# Example 4 - Custom Initialization

Another of the fantastic things about SparkFun Qwiic Universal Auto-Detect is that it will automatically initialize the sensors for you.
Based on our experience with [OpenLog Artemis](https://www.sparkfun.com/products/19426), we have included initialization code for the sensors
that require it. The code is applied when you call ```initializeSensors();``` in ```setup()```.

But what if you want to initialize your sensor(s) differently? What do you do then? Do you need to edit the initialization code in this library - 
only to have your work overwritten when the library is updated? No! We have included a **Custom Initialization** feature to let you let you change
the initialization safely and easily!

In this example, we focus on the u-blox GNSS module.

The initialization code _inside this library_ looks like this:

```C++
  device->setI2COutput(COM_TYPE_UBX);                 // Set the I2C port to output UBX only (turn off NMEA noise)
  device->saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save (only) the communications port settings to flash and BBR
  device->setAutoPVT(true);                           // Enable PVT at the navigation rate
```

The code is doing three things which we (nearly) always need to do when communicating with a u-blox GNSS:
* We disable the NMEA protocol on the I2C port, leaving only UBX enabled
* We save the port configuration to battery-backed memory
* We tell the u-blox module to output its position, velocity and time (PVT) message at the standard 1Hz navigation rate

The third thing prevents the u-blox GNSS library from stalling (blocking) while it waits for the next PVT message. ```setAutoPVT(true);``` causes
the PVT messages to be generated periodically, and tells the library to return immediately using the data from the _most recent_ PVT message
instead of waiting for 'fresh' data to arrive. That way, you get your position immediately (even if it is _slightly_ out of date).

But what if you are always going to run the GNSS at 4Hz instead of the standard 1Hz? How do you tell the library to use your own
custom initialization code? This is how:

## Custom Initializer

At the start of the example, you will see the code for the custom initializer:

```C++
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
```

You will see that the first three lines of the custom initializer are the same as the initializer code built in to the library.
It is the 4th line ```setMeasurementRate(250);``` which sets the GNSS module to output PVT at 4Hz (intervals of 250ms) instead of 1Hz.

## setCustomInitialize

Now that we have declared our custom initializer, we need to tell the library to use it. In the ```setup()``` code you will see:

```C++
  // Set up a custom initializer for a u-blox GNSS module: with I2C address 0x42, on the main branch (Mux address 0, Mux port 0)
  // Do this after .beginSensors but before .initializeSensors
  mySensors.setCustomInitialize(&myCustomInitializer, "SFE_UBLOX_GNSS", 0x42, 0, 0);

  // Alternatively, we can use the same custom initializer for all instances of the sensor
  // Again, do this after .beginSensors but before .initializeSensors
  mySensors.setCustomInitialize(&myCustomInitializer, "SFE_UBLOX_GNSS");

  mySensors.initializeSensors(); // Initialize all the sensors
```

If you have multiple sensors of the same type connected, you can apply the same custom initializer to all of them. This line of code
causes the library to apply ```myCustomInitializer``` to all connected ***SFE_UBLOX_GNSS** sensors:
* ```setCustomInitialize(&myCustomInitializer, "SFE_UBLOX_GNSS");```

But, as the code suggests, you could also create separate custom initializers and apply them to the sensors individually based on their
I2C address, Mux address and Mux Port (if present):
* ```setCustomInitialize(&myCustomInitializer1, "SFE_UBLOX_GNSS", 0x42, 0x70, 0); // Apply myCustomInitializer1 to the GNSS on Mux 0x70 Port 0```
* ```setCustomInitialize(&myCustomInitializer2, "SFE_UBLOX_GNSS", 0x42, 0x70, 1); // Apply myCustomInitializer2 to the GNSS on Mux 0x70 Port 1```
* ```setCustomInitialize(&myCustomInitializer3, "SFE_UBLOX_GNSS", 0x42, 0x70, 2); // Apply myCustomInitializer3 to the GNSS on Mux 0x70 Port 2```

Because the custom initializer code is stored in your .ino code, it will not be overwritten when the library is updated!
