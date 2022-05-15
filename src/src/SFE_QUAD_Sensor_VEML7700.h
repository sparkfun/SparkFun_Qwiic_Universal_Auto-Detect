#ifndef SPARKFUN_QUAD_SENSOR_HEADER_VEML7700_H // <=== Update this with the new sensor type
#define SPARKFUN_QUAD_SENSOR_HEADER_VEML7700_H // <=== Update this with the new sensor type

#include "VEML7700/SparkFun_VEML7700_Arduino_Library.h" // <=== Update this with the new sensor library header file

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

#define CLASSNAME VEML7700 // <=== Update this with the new sensor type

#define CLASSTITLE SFE_QUAD_Sensor_VEML7700 // <=== Update this with the new sensor type

#define SENSE_COUNT 3 // <=== Update this with the number of things this sensor can sense

#define SETTING_COUNT 14 // <=== Update this with the number of things that can be set on this sensor

#define CONFIGURATION_ITEM_COUNT 3 // <=== Update this with the number of things that can be configured on this sensor

#define SENSOR_I2C_ADDRESSES const uint8_t sensorI2cAddresses[] = {0x10} // <=== Update this with the I2C addresses for this sensor

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
      return ("Ambient Light");
      break;
    case 1:
      return ("White Level");
      break;
    case 2:
      return ("Lux");
      break;
    default:
      return (NULL);
      break;
    }
    return (NULL);
  }

  // Return the specified sense reading as exponential format text. ===> Adapt this to match the sensor type <===
  bool getSenseReading(uint8_t sense, char *reading)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (sense)
    {
    case 0:
      sprintf(reading, "%d", device->getAmbientLight());
      return (true);
      break;
    case 1:
      sprintf(reading, "%d", device->getWhiteLevel());
      return (true);
      break;
    case 2:
      OLS_sprintf.OLS_etoa((double)device->getLux(), reading); // Get the Lux
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
      return ("Integration Time: 25ms");
      break;
    case 1:
      return ("Integration Time: 50ms");
      break;
    case 2:
      return ("Integration Time: 100ms");
      break;
    case 3:
      return ("Integration Time: 200ms");
      break;
    case 4:
      return ("Integration Time: 400ms");
      break;
    case 5:
      return ("Integration Time: 800ms");
      break;
    case 6:
      return ("Sensitivity: x1");
      break;
    case 7:
      return ("Sensitivity: x2");
      break;
    case 8:
      return ("Sensitivity: x1_8");
      break;
    case 9:
      return ("Sensitivity: x1_4");
      break;
    case 10:
      return ("Persistence: x1");
      break;
    case 11:
      return ("Persistence: x2");
      break;
    case 12:
      return ("Persistence: x4");
      break;
    case 13:
      return ("Persistence: x8");
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
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
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
      device->setIntegrationTime(VEML7700_INTEGRATION_25ms);
      break;
    case 1:
      device->setIntegrationTime(VEML7700_INTEGRATION_50ms);
      break;
    case 2:
      device->setIntegrationTime(VEML7700_INTEGRATION_100ms);
      break;
    case 3:
      device->setIntegrationTime(VEML7700_INTEGRATION_200ms);
      break;
    case 4:
      device->setIntegrationTime(VEML7700_INTEGRATION_400ms);
      break;
    case 5:
      device->setIntegrationTime(VEML7700_INTEGRATION_800ms);
      break;
    case 6:
      device->setSensitivityMode(VEML7700_SENSITIVITY_x1);
      break;
    case 7:
      device->setSensitivityMode(VEML7700_SENSITIVITY_x2);
      break;
    case 8:
      device->setSensitivityMode(VEML7700_SENSITIVITY_x1_8);
      break;
    case 9:
      device->setSensitivityMode(VEML7700_SENSITIVITY_x1_4);
      break;
    case 10:
      device->setPersistenceProtect(VEML7700_PERSISTENCE_1);
      break;
    case 11:
      device->setPersistenceProtect(VEML7700_PERSISTENCE_2);
      break;
    case 12:
      device->setPersistenceProtect(VEML7700_PERSISTENCE_4);
      break;
    case 13:
      device->setPersistenceProtect(VEML7700_PERSISTENCE_8);
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
      return ("Int_Time");
      break;
    case 1:
      return ("Sens");
      break;
    case 2:
      return ("Pers");
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
    case 1:
    case 2:
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
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    switch (configItem)
    {
    case 0:
      value->UINT8_T = (uint8_t)device->getIntegrationTime(); // VEML7700_integration_time_t
      break;
    case 1:
      value->UINT8_T = (uint8_t)device->getSensitivityMode(); // VEML7700_sensitivity_mode_t
      break;
    case 2:
      value->UINT8_T = (uint8_t)device->getIntegrationTime(); // VEML7700_integration_time_t
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
      device->setIntegrationTime((VEML7700_integration_time_t)value->UINT8_T);
      break;
    case 1:
      device->setSensitivityMode((VEML7700_sensitivity_mode_t)value->UINT8_T);
      break;
    case 2:
      device->setIntegrationTime((VEML7700_integration_time_t)value->UINT8_T);
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }
};

#endif
