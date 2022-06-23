#ifndef SPARKFUN_QUAD_SENSOR_HEADER_VL53L1X_H // <=== Update this with the new sensor type
#define SPARKFUN_QUAD_SENSOR_HEADER_VL53L1X_H // <=== Update this with the new sensor type

#include "VL53L1X/SparkFun_VL53L1X.h" // <=== Update this with the new sensor library header file

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

#define CLASSNAME SFEVL53L1X // <=== Update this with the new sensor type

#define CLASSTITLE SFE_QUAD_Sensor_VL53L1X // <=== Update this with the new sensor type

#define SENSE_COUNT 3 // <=== Update this with the number of things this sensor can sense

#define SETTING_COUNT 5 // <=== Update this with the number of things that can be set on this sensor

#define CONFIGURATION_ITEM_COUNT 4 // <=== Update this with the number of things that can be configured on this sensor

#define SENSOR_I2C_ADDRESSES const uint8_t sensorI2cAddresses[] = {0x29} // <=== Update this with the I2C addresses for this sensor

class CLASSTITLE : public SFE_QUAD_Sensor
{
public:
  bool _shortDistanceMode;

  CLASSTITLE(TwoWire &port)
  {
    _sensorAddress = 0;
    _muxAddress = 0;
    _muxPort = 0;
    _classPtr = new CLASSNAME(port);
    _next = NULL;
    _logSense = new bool[SENSE_COUNT + 1];
    for (size_t i = 0; i <= SENSE_COUNT; i++)
      _logSense[i] = true;
    _customInitializePtr = NULL;
    _shortDistanceMode = true;
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
    port.beginTransmission(sensorAddress); // Scan the sensor address first
    if (port.endTransmission() == 0)
    {
      CLASSNAME *device = (CLASSNAME *)_classPtr;
      return (device->begin() == 0);
    }
    else
      return (false);
  }

  // Begin the sensor. ===> Adapt this to match the sensor type <===
  bool beginSensor(uint8_t sensorAddress, TwoWire &port)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    return (device->begin() == 0);
  }

  // Initialize the sensor. ===> Adapt this to match the sensor type <===
  bool initializeSensor(uint8_t sensorAddress, TwoWire &port)
  {
    if (_customInitializePtr == NULL) // Has a custom initialize function been defined?
    {
      CLASSNAME *device = (CLASSNAME *)_classPtr;
      if (_shortDistanceMode)
        device->setDistanceModeShort();
      else
        device->setDistanceModeLong();
      device->startRanging();
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
      return ("Distance (mm)");
      break;
    case 1:
      return ("Status (0 = Good)");
      break;
    case 2:
      return ("Signal Rate");
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
      sprintf(reading, "%d", device->getDistance());
      return (true);
      break;
    case 1:
      sprintf(reading, "%d", device->getRangeStatus());
      return (true);
      break;
    case 2:
      sprintf(reading, "%d", device->getSignalRate());
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
      return ("Distance Mode: Short");
      break;
    case 1:
      return ("Distance Mode: Long");
      break;
    case 2:
      return ("Intermeasurement Period");
      break;
    case 3:
      return ("Crosstalk");
      break;
    case 4:
      return ("Offset");
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
    case 1:
      *type = SFE_QUAD_SETTING_TYPE_NONE;
      break;
    case 2:
    case 3:
    case 4:
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
      device->stopRanging();
      _shortDistanceMode = true;
      device->setDistanceModeShort();
      device->startRanging();
      break;
    case 1:
      device->stopRanging();
      _shortDistanceMode = false;
      device->setDistanceModeLong();
      if (device->getIntermeasurementPeriod() < 140)
        device->setIntermeasurementPeriod(140);
      device->startRanging();
      break;
    case 2:
      device->stopRanging();
      if (value->UINT16_T < 20)
        value->UINT16_T = 20;
      if (!_shortDistanceMode)
        if (value->UINT16_T < 140)
          value->UINT16_T = 140;
      if (value->UINT16_T > 1000)
        value->UINT16_T = 1000;
      device->setIntermeasurementPeriod(value->UINT16_T);
      device->startRanging();
      break;
    case 3:
      device->stopRanging();
      if (value->UINT16_T > 4000)
        value->UINT16_T = 4000;
      device->setXTalk(value->UINT16_T);
      device->startRanging();
      break;
    case 4:
      device->stopRanging();
      if (value->UINT16_T > 4000)
        value->UINT16_T = 4000;
      device->setOffset(value->UINT16_T);
      device->startRanging();
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
      return ("Dist_Mode");
      break;
    case 1:
      return ("IM_Period");
      break;
    case 2:
      return ("Xtalk");
      break;
    case 3:
      return ("Offset");
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
      *type = SFE_QUAD_SETTING_TYPE_BOOL;
      break;
    case 1:
    case 2:
    case 3:
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
      value->BOOL = _shortDistanceMode;
      break;
    case 1:
      value->UINT16_T = device->getIntermeasurementPeriod();
      break;
    case 2:
      value->UINT16_T = device->getXTalk();
      break;
    case 3:
      value->UINT16_T = device->getOffset();
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
      _shortDistanceMode = value->BOOL;
      device->stopRanging();
      if (_shortDistanceMode)
        device->setDistanceModeShort();
      else
        device->setDistanceModeLong();
      device->startRanging();
      break;
    case 1:
      device->stopRanging();
      device->setIntermeasurementPeriod(value->UINT16_T);
      device->startRanging();
      break;
    case 2:
      device->stopRanging();
      device->setXTalk(value->UINT16_T);
      device->startRanging();
      break;
    case 3:
      device->stopRanging();
      device->setOffset(value->UINT16_T);
      device->startRanging();
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }
};

#endif
