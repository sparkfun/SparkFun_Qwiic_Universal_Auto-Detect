#ifndef SPARKFUN_QUAD_SENSOR_HEADER_UBLOX_GNSS_H // <=== Update this with the new sensor type
#define SPARKFUN_QUAD_SENSOR_HEADER_UBLOX_GNSS_H // <=== Update this with the new sensor type

#include "UBLOX_GNSS/SparkFun_u-blox_GNSS_Arduino_Library.h" // <=== Update this with the new sensor library header file

#ifdef CLASSNAME
#undef CLASSNAME
#endif
#ifdef CLASSTITLE
#undef CLASSTITLE
#endif
#ifdef SENSE_COUNT
#undef SENSE_COUNT
#endif
#ifdef SETTING_COUNT
#undef SETTING_COUNT
#endif
#ifdef CONFIGURATION_ITEM_COUNT
#undef CONFIGURATION_ITEM_COUNT
#endif
#ifdef SENSOR_I2C_ADDRESSES
#undef SENSOR_I2C_ADDRESSES
#endif

#define CLASSNAME SFE_UBLOX_GNSS // <=== Update this with the new sensor type

#define CLASSTITLE SFE_QUAD_Sensor_UBLOX_GNSS // <=== Update this with the new sensor type

#define SENSE_COUNT 17 // <=== Update this with the number of things this sensor can sense

#define SETTING_COUNT 2 // <=== Update this with the number of things that can be set on this sensor

#define CONFIGURATION_ITEM_COUNT 1 // <=== Update this with the number of things that can be configured on this sensor

#define SENSOR_I2C_ADDRESSES const uint8_t sensorI2cAddresses[] = {0x42} // <=== Update this with the I2C addresses for this sensor

class CLASSTITLE : public SFE_QUAD_Sensor
{
public:
  CLASSTITLE(void)
  {
    _sensorAddress = 0;
    _muxAddress = 0;
    _muxPort = 0;
    _classPtr = new CLASSNAME;
    _next = NULL;
    _logSense = new bool[SENSE_COUNT + 1];
    for (size_t i = 0; i <= SENSE_COUNT; i++)
      _logSense[i] = true;
    _customInitializePtr = NULL;
  }

  void deleteSensorStorage(void)
  {
    delete (CLASSNAME *)_classPtr;
    delete[] _logSense;
  }

  // Return the sensor name as char
  const char *getSensorName(void)
  {
    return (STR(CLASSNAME));
  }

  // Return how many I2C addresses this sensor supports
  uint8_t getNumI2cAddresses(void)
  {
    SENSOR_I2C_ADDRESSES;
    return (sizeof(sensorI2cAddresses) / sizeof(sensorI2cAddresses[0]));
  }

  // Return the selected I2C address
  uint8_t getI2cAddress(uint8_t address)
  {
    SENSOR_I2C_ADDRESSES;
    return (sensorI2cAddresses[address]);
  }

  // Detect the sensor. ===> Adapt this to match the sensor type <===
  bool detectSensor(uint8_t sensorAddress, TwoWire &port)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    return (device->begin(port, sensorAddress));
  }

  // Begin the sensor. ===> Adapt this to match the sensor type <===
  bool beginSensor(uint8_t sensorAddress, TwoWire &port)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    return (device->begin(port, sensorAddress));
  }

  // Initialize the sensor. ===> Adapt this to match the sensor type <===
  bool initializeSensor(uint8_t sensorAddress, TwoWire &port)
  {
    if (_customInitializePtr == NULL) // Has a custom initialize function been defined?
    {
      CLASSNAME *device = (CLASSNAME *)_classPtr;
      device->setI2COutput(COM_TYPE_UBX);                 // Set the I2C port to output UBX only (turn off NMEA noise)
      device->saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save (only) the communications port settings to flash and BBR
      device->setAutoPVT(true);                           // Enable PVT at the navigation rate
      return (true);
    }
    else
    {
      _customInitializePtr(sensorAddress, port, _classPtr); // Call the custom initialize function
      return (true);
    }
  }

  // Return the count of the number of things which this sensor can sense
  bool getSenseCount(uint8_t *count)
  {
    *count = SENSE_COUNT;
    return (true);
  }

  // Return the maximum length of the names of the things which this sensor can sense
  uint8_t getSenseNameMaxLen()
  {
    size_t longest = 0;
    for (uint8_t x = 0; x < SENSE_COUNT; x++)
      if (strlen(getSenseName(x)) > longest)
        longest = strlen(getSenseName(x));
    return (longest);
  }

  // Return the name of the name of the specified sense. ===> Adapt this to match the sensor type <===
  const char *getSenseName(uint8_t sense)
  {
    switch (sense)
    {
    case 0:
      return ("Year");
      break;
    case 1:
      return ("Month");
      break;
    case 2:
      return ("Day");
      break;
    case 3:
      return ("Hour");
      break;
    case 4:
      return ("Min");
      break;
    case 5:
      return ("Sec");
      break;
    case 6:
      return ("Latitude");
      break;
    case 7:
      return ("Longitude");
      break;
    case 8:
      return ("Altitude");
      break;
    case 9:
      return ("Alt (MSL)");
      break;
    case 10:
      return ("SIV");
      break;
    case 11:
      return ("Fix");
      break;
    case 12:
      return ("Carr Soln");
      break;
    case 13:
      return ("Speed");
      break;
    case 14:
      return ("Heading");
      break;
    case 15:
      return ("PDOP");
      break;
    case 16:
      return ("TOW");
      break;
    default:
      return (NULL);
      break;
    }
    return (NULL);
  }

  // Return the specified sense reading as text. ===> Adapt this to match the sensor type <===
  bool getSenseReading(uint8_t sense, char *reading)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (sense)
    {
    case 0:
      sprintf(reading, "%d", device->getYear());
      return (true);
      break;
    case 1:
      sprintf(reading, "%d", device->getMonth());
      return (true);
      break;
    case 2:
      sprintf(reading, "%d", device->getDay());
      return (true);
      break;
    case 3:
      sprintf(reading, "%d", device->getHour());
      return (true);
      break;
    case 4:
      sprintf(reading, "%d", device->getMinute());
      return (true);
      break;
    case 5:
      sprintf(reading, "%d", device->getSecond());
      return (true);
      break;
    case 6:
      sprintf(reading, "%d", device->getLatitude());
      return (true);
      break;
    case 7:
      sprintf(reading, "%d", device->getLongitude());
      return (true);
      break;
    case 8:
      sprintf(reading, "%d", device->getAltitude());
      return (true);
      break;
    case 9:
      sprintf(reading, "%d", device->getAltitudeMSL());
      return (true);
      break;
    case 10:
      sprintf(reading, "%d", device->getSIV());
      return (true);
      break;
    case 11:
      sprintf(reading, "%d", device->getFixType());
      return (true);
      break;
    case 12:
      sprintf(reading, "%d", device->getCarrierSolutionType());
      return (true);
      break;
    case 13:
      sprintf(reading, "%d", device->getGroundSpeed());
      return (true);
      break;
    case 14:
      sprintf(reading, "%d", device->getHeading());
      return (true);
      break;
    case 15:
      sprintf(reading, "%d", device->getPDOP());
      return (true);
      break;
    case 16:
      sprintf(reading, "%d", device->getTimeOfWeek());
      return (true);
      break;
    default:
      return (false);
      break;
    }
    return (false);
  }

  // Return the count of the number of things that can be set on this sensor
  bool getSettingCount(uint8_t *count)
  {
    *count = SETTING_COUNT;
    return (true);
  }

  // Return the maximum length of the names of the things that can be set on this sensor
  uint8_t getSettingNameMaxLen()
  {
    size_t longest = 0;
    for (uint8_t x = 0; x < SETTING_COUNT; x++)
      if (strlen(getSettingName(x)) > longest)
        longest = strlen(getSettingName(x));
    return (longest);
  }

  // Return the name of the name of the specified setting. ===> Adapt this to match the sensor type <===
  const char *getSettingName(uint8_t setting)
  {
    switch (setting)
    {
    case 0:
      return ("Factory Default (takes 5 seconds to complete)");
      break;
    case 1:
      return ("Set Measurement Rate/Interval (ms)");
      break;
    default:
      return (NULL);
      break;
    }
    return (NULL);
  }

  // Return the type of the specified setting. ===> Adapt this to match the sensor type <===
  bool getSettingType(uint8_t setting, SFE_QUAD_Sensor_Setting_Type_e *type)
  {
    switch (setting)
    {
    case 0:
      *type = SFE_QUAD_SETTING_TYPE_NONE;
      break;
    case 1:
      *type = SFE_QUAD_SETTING_TYPE_UINT16_T;
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }

  // Set the specified setting. ===> Adapt this to match the sensor type <===
  bool setSetting(uint8_t setting, SFE_QUAD_Sensor_Every_Type_t *value)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (setting)
    {
    case 0:
      device->factoryDefault();
      delay(5000);
      device->setI2COutput(COM_TYPE_UBX);                 // Set the I2C port to output UBX only (turn off NMEA noise)
      device->saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save (only) the communications port settings to flash and BBR
      device->setAutoPVT(true);                           // Enable PVT at the navigation rate
      break;
    case 1:
      device->setMeasurementRate(value->UINT16_T);
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }

  // Return the count of the number of things which can be configured on this sensor
  bool getConfigurationItemCount(uint8_t *count)
  {
    *count = CONFIGURATION_ITEM_COUNT;
    return (true);
  }

  // Return the largest number of characters in the array of configuration item names
  uint8_t getConfigurationItemNameMaxLen()
  {
    size_t longest = 0;
    for (uint8_t x = 0; x < CONFIGURATION_ITEM_COUNT; x++)
      if (strlen(getConfigurationItemName(x)) > longest)
        longest = strlen(getConfigurationItemName(x));
    return (longest);
  }

  // Return the name of the configuration item
  // Use underscores, not spaces
  const char *getConfigurationItemName(uint8_t configItem)
  {
    switch (configItem)
    {
    case 0:
      return ("Measurement_Rate");
      break;
    default:
      return (NULL);
      break;
    }
    return (NULL);
  }

  // Return the type of the specified configuration item
  bool getConfigurationItemType(uint8_t configItem, SFE_QUAD_Sensor_Setting_Type_e *type)
  {
    switch (configItem)
    {
    case 0:
      *type = SFE_QUAD_SETTING_TYPE_UINT16_T;
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }

  // Get (read) the sensor configuration item
  bool getConfigurationItem(uint8_t configItem, SFE_QUAD_Sensor_Every_Type_t *value)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (configItem)
    {
    case 0:
      value->UINT16_T = device->getMeasurementRate();
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }

  // Set (write) the sensor configuration item
  bool setConfigurationItem(uint8_t configItem, SFE_QUAD_Sensor_Every_Type_t *value)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (configItem)
    {
    case 0:
      device->setMeasurementRate(value->UINT16_T);
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }
};

#endif
