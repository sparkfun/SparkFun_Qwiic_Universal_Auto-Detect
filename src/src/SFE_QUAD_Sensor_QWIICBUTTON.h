#ifndef SPARKFUN_QUAD_SENSOR_HEADER_QWIICBUTTON_H // <=== Update this with the new sensor type
#define SPARKFUN_QUAD_SENSOR_HEADER_QWIICBUTTON_H // <=== Update this with the new sensor type

#include "QWIICBUTTON/SparkFun_Qwiic_Button.h" // <=== Update this with the new sensor library header file

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

#define CLASSNAME QwiicButton // <=== Update this with the new sensor type

#define CLASSTITLE SFE_QUAD_Sensor_QWIICBUTTON // <=== Update this with the new sensor type

#define SENSE_COUNT 2 // <=== Update this with the number of things this sensor can sense

#define SETTING_COUNT 2 // <=== Update this with the number of things that can be set on this sensor

#define CONFIGURATION_ITEM_COUNT 2 // <=== Update this with the number of things that can be configured on this sensor

#define SENSOR_I2C_ADDRESSES const uint8_t sensorI2cAddresses[] = {0x6F, 0x6E, 0x6D, 0x6C, 0x6B, 0x6A, 0x69, 0x68} // <=== Update this with the I2C addresses for this sensor

class CLASSTITLE : public SFE_QUAD_Sensor
{
public:
  uint8_t _ledBrightness;
  bool _ledState;
  bool _toggleLEDOnClick;

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
    _ledBrightness = 128;
    _ledState = false;
    _toggleLEDOnClick = true;
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
    return (device->begin(sensorAddress, port));
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
      device->LEDoff();
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
      return ("Pressed");
      break;
    case 1:
      return ("Clicked");
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
    {
      long pressedPopped = 0;
      while (device->isPressedQueueEmpty() == false)
      {
        pressedPopped = device->popPressedQueue();
      }
      _sprintf._etoa(((double)pressedPopped) / 1000.0, reading); // Get the pressure
      return (true);
    }
      break;
    case 1:
    {
      long clickedPopped = 0;
      while (device->isClickedQueueEmpty() == false)
      {
        clickedPopped = device->popClickedQueue();
        _ledState ^= 1; // Toggle ledState
      }
      _sprintf._etoa(((double)clickedPopped) / 1000.0, reading); // Get the pressure
      if (_toggleLEDOnClick)
      {
        if (_ledState)
          device->LEDon(_ledBrightness);
        else
          device->LEDoff();
      }
      return (true);
    }
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
      return ("Toggle LED on Click");
      break;
    case 1:
      return ("LED Brightness");
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
      *type = SFE_QUAD_SETTING_TYPE_BOOL;
      break;
    case 1:
      *type = SFE_QUAD_SETTING_TYPE_UINT8_T;
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
      _toggleLEDOnClick = value->BOOL;
      break;
    case 1:
      _ledBrightness = value->UINT8_T;
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
      return ("Toggle_LED");
      break;
    case 1:
      return ("LED_Bright");
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
      value->BOOL = _toggleLEDOnClick;
      break;
    case 1:
      value->UINT8_T = _ledBrightness;
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
      _toggleLEDOnClick = value->BOOL;
      break;
    default:
      _ledBrightness = value->UINT8_T;
      break;
    }
    return (true);
  }
};

#endif
