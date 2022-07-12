#ifndef SPARKFUN_QUAD_SENSOR_HEADER_VCNL4040_H // <=== Update this with the new sensor type
#define SPARKFUN_QUAD_SENSOR_HEADER_VCNL4040_H // <=== Update this with the new sensor type

#include "VCNL4040/SparkFun_VCNL4040_Arduino_Library.h" // <=== Update this with the new sensor library header file

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

#define CLASSNAME VCNL4040 // <=== Update this with the new sensor type

#define CLASSTITLE SFE_QUAD_Sensor_VCNL4040 // <=== Update this with the new sensor type

#define SENSE_COUNT 2 // <=== Update this with the number of things this sensor can sense

#define SETTING_COUNT 6 // <=== Update this with the number of things that can be set on this sensor

#define CONFIGURATION_ITEM_COUNT 5 // <=== Update this with the number of things that can be configured on this sensor

#define SENSOR_I2C_ADDRESSES const uint8_t sensorI2cAddresses[] = {0x60} // <=== Update this with the I2C addresses for this sensor

class CLASSTITLE : public SFE_QUAD_Sensor
{
public:
  uint8_t _ledCurrent;
  uint16_t _irDutyCycle;
  uint8_t _proxIntTime;
  uint8_t _proxRes;
  uint16_t _ambIntTime;

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
    _ledCurrent = 200;
    _irDutyCycle = 40;
    _proxIntTime = 8;
    _proxRes = 16;
    _ambIntTime = 80;
  }

  void deleteSensorStorage(void)
  {
    if (_classPtr != NULL)
    {
      delete (CLASSNAME *)_classPtr;
      _classPtr = NULL;
    }
    if (_logSense != NULL)
    {
      delete[] _logSense;
      _logSense = NULL;
    }
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
      return (device->begin(port));
    }
    else
      return (false);
  }

  // Begin the sensor. ===> Adapt this to match the sensor type <===
  bool beginSensor(uint8_t sensorAddress, TwoWire &port)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    return (device->begin(port));
  }

  // Initialize the sensor. ===> Adapt this to match the sensor type <===
  bool initializeSensor(uint8_t sensorAddress, TwoWire &port)
  {
    if (_customInitializePtr == NULL) // Has a custom initialize function been defined?
    {
      CLASSNAME *device = (CLASSNAME *)_classPtr;
      device->powerOnAmbient(); //Turn on ambient sensing
      device->setLEDCurrent(_ledCurrent);
      device->setIRDutyCycle(_irDutyCycle);
      device->setProxIntegrationTime(_proxIntTime);
      device->setProxResolution(_proxRes);
      device->setAmbientIntegrationTime(_ambIntTime);
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
      return ("Proximity");
      break;
    case 1:
      return ("Ambient Light (Lux)");
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
      sprintf(reading, "%d", device->getProximity());
      return (true);
      break;
    case 1:
      sprintf(reading, "%d", device->getAmbient());
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
      return ("LED Current (50 to 200mA)");
      break;
    case 1:
      return ("IR Duty Cycle (40 to 320)");
      break;
    case 2:
      return ("Proximity Integration Time (1 to 8)");
      break;
    case 3:
      return ("Ambient Light Integration Time (80 to 640ms)");
      break;
    case 4:
      return ("Proximity Resolution (12 bit)");
      break;
    case 5:
      return ("Proximity Resolution (16 bit)");
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
      *type = SFE_QUAD_SETTING_TYPE_UINT8_T;
      break;
    case 1:
      *type = SFE_QUAD_SETTING_TYPE_UINT16_T;
      break;
    case 2:
      *type = SFE_QUAD_SETTING_TYPE_UINT8_T;
      break;
    case 3:
      *type = SFE_QUAD_SETTING_TYPE_UINT16_T;
      break;
    case 4:
      *type = SFE_QUAD_SETTING_TYPE_NONE;
      break;
    case 5:
      *type = SFE_QUAD_SETTING_TYPE_NONE;
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
      if ((value->UINT8_T >= 50) && (value->UINT8_T <= 200))
      {
        _ledCurrent = value->UINT8_T;
        device->setLEDCurrent(value->UINT8_T);
      }
      break;
    case 1:
      if ((value->UINT16_T >= 40) && (value->UINT16_T <= 320))
      {
        _irDutyCycle = value->UINT16_T;
        device->setIRDutyCycle(value->UINT16_T);
      }
      break;
    case 2:
      if ((value->UINT8_T >= 1) && (value->UINT8_T <= 8))
      {
        _proxIntTime = value->UINT8_T;
        device->setProxIntegrationTime(value->UINT8_T);
      }
      break;
    case 3:
      if ((value->UINT16_T >= 80) && (value->UINT16_T <= 640))
      {
        _ambIntTime = value->UINT16_T;
        device->setAmbientIntegrationTime(value->UINT16_T);
      }
      break;
    case 4:
      _proxRes = 12;
      device->setProxResolution(12);
      break;
    case 5:
      _proxRes = 16;
      device->setProxResolution(16);
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
      return ("LED_I");
      break;
    case 1:
      return ("IR_Duty");
      break;
    case 2:
      return ("Prox_Int");
      break;
    case 3:
      return ("Amb_Int");
      break;
    case 4:
      return ("Prox_Res");
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
      *type = SFE_QUAD_SETTING_TYPE_UINT8_T;
      break;
    case 1:
      *type = SFE_QUAD_SETTING_TYPE_UINT16_T;
      break;
    case 2:
      *type = SFE_QUAD_SETTING_TYPE_UINT8_T;
      break;
    case 3:
      *type = SFE_QUAD_SETTING_TYPE_UINT16_T;
      break;
    case 4:
      *type = SFE_QUAD_SETTING_TYPE_UINT8_T;
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
    //CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (configItem)
    {
    case 0:
      value->UINT8_T = _ledCurrent;
      break;
    case 1:
      value->UINT16_T = _irDutyCycle;
      break;
    case 2:
      value->UINT8_T = _proxIntTime;
      break;
    case 3:
      value->UINT16_T = _ambIntTime;
      break;
    case 4:
      value->UINT8_T = _proxRes;
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
      _ledCurrent = value->UINT8_T;
      device->setLEDCurrent(value->UINT8_T);
      break;
    case 1:
      _irDutyCycle = value->UINT16_T;
      device->setIRDutyCycle(value->UINT16_T);
      break;
    case 2:
      _proxIntTime = value->UINT8_T;
      device->setProxIntegrationTime(value->UINT8_T);
      break;
    case 3:
      _ambIntTime = value->UINT16_T;
      device->setAmbientIntegrationTime(value->UINT16_T);
      break;
    case 4:
      _proxRes = value->UINT8_T;
      device->setProxResolution(value->UINT8_T);
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }
};

#endif
