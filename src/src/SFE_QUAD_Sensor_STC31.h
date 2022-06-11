#ifndef SPARKFUN_QUAD_SENSOR_HEADER_STC31_H // <=== Update this with the new sensor type
#define SPARKFUN_QUAD_SENSOR_HEADER_STC31_H // <=== Update this with the new sensor type

#include "STC31/SparkFun_STC3x_Arduino_Library.h" // <=== Update this with the new sensor library header file

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

#define CLASSNAME STC3x // <=== Update this with the new sensor type

#define CLASSTITLE SFE_QUAD_Sensor_STC31 // <=== Update this with the new sensor type

#define SENSE_COUNT 2 // <=== Update this with the number of things this sensor can sense

#define SETTING_COUNT 7 // <=== Update this with the number of things that can be set on this sensor

#define CONFIGURATION_ITEM_COUNT 4 // <=== Update this with the number of things that can be configured on this sensor

#define SENSOR_I2C_ADDRESSES const uint8_t sensorI2cAddresses[] = {0x29, 0x2A, 0x2B, 0x2C} // <=== Update this with the I2C addresses for this sensor

class CLASSTITLE : public SFE_QUAD_Sensor
{
public:
  float _rh;
  float _temp;
  uint16_t _press;
  STC3X_binary_gas_type_e _binaryGas;
  bool _co2;
  bool _validTemp;

  CLASSTITLE(void)
  {
    _sensorAddress = 0;
    _muxAddress = 0;
    _muxPort = 0;
    _classPtr = new CLASSNAME(STC3x_SENSOR_STC31);
    _next = NULL;
    _logSense = new bool[SENSE_COUNT + 1];
    for (size_t i = 0; i <= SENSE_COUNT; i++)
      _logSense[i] = true;
    _customInitializePtr = NULL;
    _rh = 50.0;
    _temp = 25.0;
    _press = 1000;
    _binaryGas = STC3X_BINARY_GAS_CO2_AIR_25;
    _co2 = false;
    _validTemp = false;
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
      return (device->begin(sensorAddress, port));
    }
    else
      return (false);
  }

  // Begin the sensor. ===> Adapt this to match the sensor type <===
  bool beginSensor(uint8_t sensorAddress, TwoWire &port)
  {
    CLASSNAME *device = (CLASSNAME *)_classPtr;
    return (device->begin(sensorAddress, port));
  }

  // Initialize the sensor. ===> Adapt this to match the sensor type <===
  bool initializeSensor(uint8_t sensorAddress, TwoWire &port)
  {
    if (_customInitializePtr == NULL) // Has a custom initialize function been defined?
    {
      CLASSNAME *device = (CLASSNAME *)_classPtr;
      device->setBinaryGas(_binaryGas);
      //device->setTemperature(_temp);
      device->setRelativeHumidity(_rh);
      device->setPressure(_press);
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
      return ("CO2 (%)");
      break;
    case 1:
      return ("Temperature (C)");
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
      if (_co2 == false)
      {
        device->measureGasConcentration();
        _validTemp = true;
      }
      _co2 = false;
      _sprintf._etoa((double)device->getCO2(), reading); // Get the CO2 concentration
      return (true);
      break;
    case 1:
      if (_validTemp == false)
      {
        device->measureGasConcentration();
        _co2 = true;
      }
      _validTemp = false;
      _sprintf._etoa((double)device->getTemperature(), reading); // Get the temperature
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
      return ("Set binary gas to CO2 in N2.  Range: 0 to 100 vol%");
      break;
    case 1:
      return ("Set binary gas to CO2 in Air. Range: 0 to 100 vol%");
      break;
    case 2:
      return ("Set binary gas to CO2 in N2.  Range: 0 to 25 vol%");
      break;
    case 3:
      return ("Set binary gas to CO2 in Air. Range: 0 to 25 vol%");
      break;
    case 4:
      return ("Ambient temperature (C)");
      break;
    case 5:
      return ("Relative humidity (%)");
      break;
    case 6:
      return ("Ambient pressure (mbar)");
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
      *type = SFE_QUAD_SETTING_TYPE_NONE;
      break;
    case 2:
      *type = SFE_QUAD_SETTING_TYPE_NONE;
      break;
    case 3:
      *type = SFE_QUAD_SETTING_TYPE_NONE;
      break;
    case 4:
      *type = SFE_QUAD_SETTING_TYPE_FLOAT;
      break;
    case 5:
      *type = SFE_QUAD_SETTING_TYPE_FLOAT;
      break;
    case 6:
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
      _binaryGas = STC3X_BINARY_GAS_CO2_N2_100;
      device->setBinaryGas(_binaryGas);
      break;
    case 1:
      _binaryGas = STC3X_BINARY_GAS_CO2_AIR_100;
      device->setBinaryGas(_binaryGas);
      break;
    case 2:
      _binaryGas = STC3X_BINARY_GAS_CO2_N2_25;
      device->setBinaryGas(_binaryGas);
      break;
    case 3:
      _binaryGas = STC3X_BINARY_GAS_CO2_AIR_25;
      device->setBinaryGas(_binaryGas);
      break;
    case 4:
      _temp = value->FLOAT;
      device->setTemperature(_temp);
      break;
    case 5:
      _rh = value->FLOAT;
      device->setRelativeHumidity(_rh);
      break;
    case 6:
      _press = value->UINT16_T;
      device->setPressure(_press);
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
      return ("Binary_gas");
      break;
    case 1:
      return ("Temp");
      break;
    case 2:
      return ("RH");
      break;
    case 3:
      return ("Press");
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
      *type = SFE_QUAD_SETTING_TYPE_FLOAT;
      break;
    case 2:
      *type = SFE_QUAD_SETTING_TYPE_FLOAT;
      break;
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
      value->UINT8_T = (uint8_t)_binaryGas;
      break;
    case 1:
      value->FLOAT = _temp;
      break;
    case 2:
      value->FLOAT = _rh;
      break;
    case 3:
      value->UINT16_T = _press;
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
      _binaryGas = (STC3X_binary_gas_type_e)value->UINT8_T;
      device->setBinaryGas(_binaryGas);
      break;
    case 1:
      _temp = value->FLOAT;
      //device->setTemperature(_temp);
      break;
    case 2:
      _rh = value->FLOAT;;
      device->setRelativeHumidity(_rh);
      break;
    case 3:
      _press = value->UINT16_T;
      device->setPressure(_press);
      break;
    default:
      return (false);
      break;
    }
    return (true);
  }
};

#endif
