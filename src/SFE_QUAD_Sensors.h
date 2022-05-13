#ifndef SPARKFUN_QUAD_SENSORS_H
#define SPARKFUN_QUAD_SENSORS_H

// SparkFun OpenLog Sensors

// To include all sensors, uncomment #define INCLUDE_SFE_QUAD_SENSOR_ALL

#define INCLUDE_SFE_QUAD_SENSOR_ALL // Include all sensors

// To select which sensors to include:
//   comment #define INCLUDE_SFE_QUAD_SENSOR_ALL
//   uncomment one or more #define INCLUDE_SFE_QUAD_SENSOR_

//#define INCLUDE_SFE_QUAD_SENSOR_ADS122C04 // Include individual sensors
//#define INCLUDE_SFE_QUAD_SENSOR_AHT20
//#define INCLUDE_SFE_QUAD_SENSOR_BME280
//#define INCLUDE_SFE_QUAD_SENSOR_CCS811_5A
//#define INCLUDE_SFE_QUAD_SENSOR_CCS811_5B
//#define INCLUDE_SFE_QUAD_SENSOR_LPS25HB
//#define INCLUDE_SFE_QUAD_SENSOR_MCP9600
//#define INCLUDE_SFE_QUAD_SENSOR_MICROPRESSURE
//#define INCLUDE_SFE_QUAD_SENSOR_MS5637
//#define INCLUDE_SFE_QUAD_SENSOR_MS8607
//#define INCLUDE_SFE_QUAD_SENSOR_NAU7802
//#define INCLUDE_SFE_QUAD_SENSOR_QWIICBUTTON
//#define INCLUDE_SFE_QUAD_SENSOR_SCD30
//#define INCLUDE_SFE_QUAD_SENSOR_SDP3X
//#define INCLUDE_SFE_QUAD_SENSOR_SGP30
//#define INCLUDE_SFE_QUAD_SENSOR_SGP40
//#define INCLUDE_SFE_QUAD_SENSOR_SHTC3
//#define INCLUDE_SFE_QUAD_SENSOR_TMP117
//#define INCLUDE_SFE_QUAD_SENSOR_UBLOX_GNSS
//#define INCLUDE_SFE_QUAD_SENSOR_VCNL4040
//#define INCLUDE_SFE_QUAD_SENSOR_VEML6075
//#define INCLUDE_SFE_QUAD_SENSOR_VEML7700
//#define INCLUDE_SFE_QUAD_SENSOR_VL53L1X

#include "Arduino.h"
#include <Wire.h>

#include "src/I2C_MUX/SparkFun_I2C_Mux_Arduino_Library.h"

// Stringify helpers
#define STR(x) STRSTR(x)
#define STRSTR(x) #x

class SFE_QUAD_Sensors_sprintf
{
public:
  char *OLS_dtostrf(double value, char *buffer);
  char *OLS_etoa(double value, char *buffer);
  unsigned char _prec = 7;                                // precision
  void setPrecision(unsigned char prec) { _prec = prec; } // Call setPrecision to change the number of decimal places for the readings
  unsigned char getPrecision(void) { return (_prec); }
};

class SFE_QUAD_Sensor
{
public:
  // The I2C address being used by this individual sensor. Zero if not yet detected
  uint8_t _sensorAddress;

  // The address of the mux this sensor is connected to. Zero if not connected through a mux
  uint8_t _muxAddress;

  // The mux port this sensor is connected to. Zero if not connected through a mux
  uint8_t _muxPort;

  // Pointer to the Arduino Library class instance
  void *_classPtr;

  // Pointer to the next sensor in the linked list
  SFE_QUAD_Sensor *_next;

  // Pointer to an array of bool
  // The 0th element in the array defines if any sense readings from this sensor should be logged
  // The remaining elements define if individual sense readings should be logged
  bool *_logSense;

  // Pointer to the custom initializeSensor (if any)
  void (*_customInitializePtr)(uint8_t sensorAddress, TwoWire &port, void *_classPtr);

  SFE_QUAD_Sensor(void)
  {
    _sensorAddress = 0;
    _muxAddress = 0;
    _muxPort = 0;
    _classPtr = NULL;
    _next = NULL;
    _logSense = NULL;
    _customInitializePtr = NULL;
  }

  // Enum for the different settings types
  typedef enum
  {
    SFE_QUAD_SETTING_TYPE_NONE = 0,
    SFE_QUAD_SETTING_TYPE_BOOL,
    SFE_QUAD_SETTING_TYPE_FLOAT,
    SFE_QUAD_SETTING_TYPE_DOUBLE,
    SFE_QUAD_SETTING_TYPE_INT,
    SFE_QUAD_SETTING_TYPE_UINT8_T,
    SFE_QUAD_SETTING_TYPE_UINT16_T,
    SFE_QUAD_SETTING_TYPE_UINT32_T,
    SFE_QUAD_SETTING_TYPE_UNKNOWN // Must be last
  } SFE_QUAD_Sensor_Setting_Type_e;

  typedef struct
  {
    bool BOOL;
    float FLOAT;
    double DOUBLE;
    int INT;
    uint8_t UINT8_T;
    uint16_t UINT16_T;
    uint32_t UINT32_T;
  } SFE_QUAD_Sensor_Every_Type_t;

  // Delete (deallocate) the _classPtr and _logSense
  virtual void deleteSensorStorage(void) { ; }

  // Return the sensor name as char
  virtual const char *getSensorName(void) { return (NULL); }

  // Return how many I2C addresses this sensor supports
  virtual uint8_t getNumI2cAddresses(void) { return (0); }

  // Return the selected I2C address
  virtual uint8_t getI2cAddress(uint8_t address) { return (0); }

  // Detect if the sensor is present
  virtual bool detectSensor(uint8_t sensorAddress, TwoWire &port) { return (false); }

  // Begin the sensor - usually by calling its .begin function
  virtual bool beginSensor(uint8_t sensorAddress, TwoWire &port) { return (false); }

  // Initialize the sensor - apply any additional settings required after begin. A custom initializer can be provided with setCustomInitialize
  virtual bool initializeSensor(uint8_t sensorAddress, TwoWire &port) { return (false); }

  // Set the custom initializer pointer for this sensor
  void setCustomInitializePtr(void (*pointer)(uint8_t sensorAddress, TwoWire &port, void *_classPtr)) { _customInitializePtr = pointer; }

  // A sensor can have several senses. E.g. the BME280 has 3 : pressure, temperature and humidity

  // Return the count of the number of things which this sensor can sense
  virtual bool getSenseCount(uint8_t *count) { return (false); }

  // Return the name of the specified sense
  virtual const char *getSenseName(uint8_t sense) { return (NULL); }

  // Return the largest number of characters in the array of sense names
  virtual uint8_t getSenseNameMaxLen() { return (0); }

  // Return the sense reading as char
  virtual bool getSenseReading(uint8_t sense, char *reading) { return (NULL); }

  // A sensor may have several things which can be set
  // E.g. the NAU7802 scale has 2 : zero offset (calculateZeroOffset) and calibration factor (calculateCalibrationFactor)
  // setSetting sets the specified setting
  // For the NAU7802:
  //   setSetting(zero offset) would be called via the settings menu when the scale has no weight on it
  //   setSetting(calibration factor, 100.0) would be called via the settings menu when the scale has 100g on it
  // Note: settings may be different to configuration items

  // Return the count of the number of things which can be set on this sensor
  virtual bool getSettingCount(uint8_t *count) { return (false); }

  // Return the name of the specified setting
  virtual const char *getSettingName(uint8_t sense) { return (NULL); }

  // Return the largest number of characters in the array of setting names
  virtual uint8_t getSettingNameMaxLen() { return (0); }

  // Return the type of the specified setting
  virtual bool getSettingType(uint8_t setting, SFE_QUAD_Sensor_Setting_Type_e *type) { return (false); }

  // Set the specified setting
  virtual bool setSetting(uint8_t setting, SFE_QUAD_Sensor_Every_Type_t *value) { return (false); }

  // Configuration items can be written to and read from file
  // They are used to record the sensor configuration and restore the configuration
  // Note: Configuration items may or may not be the same as the settings
  // ** Each configuration item must have equivalent get and set functions **
  // E.g. the NAU7802 has 2 : getZeroOffset / setZeroOffset and getCalibrationFactor / setCalibrationFactor

  // Return the count of the number of things which can be configured on this sensor
  virtual bool getConfigurationItemCount(uint8_t *count) { return (false); }

  // Return the name of the configuration item
  virtual const char *getConfigurationItemName(uint8_t configItem) { return (NULL); }

  // Return the largest number of characters in the array of configuration item names
  virtual uint8_t getConfigurationItemNameMaxLen() { return (0); }

  // Return the type of the specified configuration item
  virtual bool getConfigurationItemType(uint8_t configItem, SFE_QUAD_Sensor_Setting_Type_e *type) { return (false); }

  // Get (read) the sensor configuration item
  virtual bool getConfigurationItem(uint8_t configItem, SFE_QUAD_Sensor_Every_Type_t *value) { return (false); }

  // Set (write) the sensor configuration item
  virtual bool setConfigurationItem(uint8_t configItem, SFE_QUAD_Sensor_Every_Type_t *value) { return (false); }

  SFE_QUAD_Sensors_sprintf OLS_sprintf; // Provide access to the common sprintf(%f) and sprintf(%e) functions
};

#include "SFE_QUAD_Headers.h"

#if __has_include("SFE_QUAD_Headers.h") // Check if the compiler supports __has_include
#define OLS_COMPILER_HAS_INCLUDE
#else
#define OLS_COMPILER_NO_HAS_INCLUDE
#endif

class SFE_QUAD_Sensors
{
public:
  SFE_QUAD_Sensors(void);
  ~SFE_QUAD_Sensors(void);

  enum SFEQUADSensorType
  {
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_ADS122C04) // <=== Add more copies of these three lines when adding new sensors
    Sensor_ADS122C04,
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_AHT20)
    Sensor_AHT20,
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_BME280)
    Sensor_BME280,
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_CCS811_5A)
    Sensor_CCS811_5A,
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_CCS811_5B)
    Sensor_CCS811_5B,
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_LPS25HB)
    Sensor_LPS25HB,
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_MCP9600)
    Sensor_MCP9600,
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_MICROPRESSURE)
    Sensor_MICROPRESSURE,
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_MS5637)
    Sensor_MS5637,
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_NAU7802)
    Sensor_NAU7802,
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_UBLOX_GNSS)
    Sensor_UBLOX_GNSS,
#endif
    SFE_QUAD_Sensor_Number_Of_Sensors // Must be last. <=== Add new sensors _above this line_ to preserve the existing enum values
  };

  SFE_QUAD_Sensor *sensorFactory(SFEQUADSensorType type) // Return a pointer to the sensor class for the selected id
  {
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_ADS122C04) // <=== Add more copies of these four lines when adding new sensors
    if (type == Sensor_ADS122C04)
      return new SFE_QUAD_Sensor_ADS122C04;
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_AHT20)
    if (type == Sensor_AHT20)
      return new SFE_QUAD_Sensor_AHT20;
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_BME280)
    if (type == Sensor_BME280)
      return new SFE_QUAD_Sensor_BME280;
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_CCS811_5A)
    if (type == Sensor_CCS811_5A)
      return new SFE_QUAD_Sensor_CCS811_5A;
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_CCS811_5B)
    if (type == Sensor_CCS811_5B)
      return new SFE_QUAD_Sensor_CCS811_5B;
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_LPS25HB)
    if (type == Sensor_LPS25HB)
      return new SFE_QUAD_Sensor_LPS25HB;
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_MCP9600)
    if (type == Sensor_MCP9600)
      return new SFE_QUAD_Sensor_MCP9600;
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_MICROPRESSURE)
    if (type == Sensor_MICROPRESSURE)
      return new SFE_QUAD_Sensor_MICROPRESSURE;
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_MS5637)
    if (type == Sensor_MS5637)
      return new SFE_QUAD_Sensor_MS5637;
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_NAU7802)
    if (type == Sensor_NAU7802)
      return new SFE_QUAD_Sensor_NAU7802;
#endif
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_UBLOX_GNSS)
    if (type == Sensor_UBLOX_GNSS)
      return new SFE_QUAD_Sensor_UBLOX_GNSS;
#endif
    return NULL;
  }

  void setWirePort(TwoWire &port);    // Define which Wire (I2C) port will be used
  void enableDebugging(Stream &port); // Define which Serial port (Stream) will be used for debug messages
  void setMenuPort(Stream &port);     // Define which Serial port (Stream) will be used for the menus
  bool detectSensors(void);           // Detect which I2C sensors are attached
  bool beginSensors(void);            // Begin each sensor - usually by calling the sensor's .begin function
  bool initializeSensors(void);       // Apply any additional settings needed for initialization
  bool getSensorReadings(void);       // Get all enabled readings from all enabled sensors. Readings are stored in readings in CSV format
  bool getSensorNames(void);          // Get the names etc. all enabled sensors. Stored in readings in CSV format
  bool getSenseNames(void);           // Get the names all enabled sensor senses. Stored in readings in CSV format

  bool setCustomInitialize(void (*pointer)(uint8_t sensorAddress, TwoWire &port, void *_classPtr), const char *sensorName, uint8_t i2cAddress, uint8_t muxAddress = 0, uint8_t muxPort = 0); // Set a custom initialize function for this sensor
  bool setCustomInitialize(void (*pointer)(uint8_t sensorAddress, TwoWire &port, void *_classPtr), const char *sensorName);                                                                  // Set a custom initialize function for all instances of this sensor

  SFE_QUAD_Sensor *sensorExists(const char *sensorName, uint8_t i2cAddress, uint8_t muxAddress = 0, uint8_t muxPort = 0); // Check if a sensor exists. Returns NULL if the sensor does not exist

  bool loggingMenu(void);                        // The logging menu - select which sensors and which senses are enabled for logging
  uint32_t getMenuChoice(unsigned long timeout); // Helper function for loggingMenu - select a menu choice

  bool settingMenu(void);                                           // The setting menu - apply settings to individual sensors. Note: settings are different to configuration
  bool getSettingValueDouble(double *value, unsigned long timeout); // Helper function for settingMenu - allow the user to enter a double value via the menu port. Supports exponent format

  bool getSensorConfiguration(void);   // Read the sensor configuration from the sensors. Store it in configuration in text format
  bool applySensorConfiguration(void); // Apply the configuration to the sensors

  SFE_QUAD_Sensor *_head; // The head of the linked list of sensors
  char *readings;        // The sensor readings stored as text (CSV)
  char *configuration;   // The sensor configuration, read by getSensorConfiguration, stored as text
  bool _printDebug;      // A flag to show if debug messages are enabled. Set true by enableDebugging
  TwoWire *_i2cPort;     // The I2C (TwoWire) port which the sensors are connected to
  Stream *_menuPort;     // The Serial port (Stream) used for the menu
  Stream *_debugPort;    // The Serial port (Stream) used for debug messages. Call enableDebugging to set the port

  SFE_QUAD_Sensors_sprintf OLS_sprintf; // Provide access to the common sprintf(%f) and sprintf(%e) functions
};

// For all platforms (that support SD.h)
#if __has_include(<SD.h>) || defined(OLS_COMPILER_NO_HAS_INCLUDE)

#define SFE_QUAD_SENSORS_SD

#include <SPI.h>
#include <SD.h>

class SFE_QUAD_Sensors__SD : public SFE_QUAD_Sensors
{
public:
  ~SFE_QUAD_Sensors__SD();

  bool beginStorage(int csPin, const char *theFileName); // Begin the SD card
  bool writeConfigurationToStorage(bool append = false); // Write configuration to theFileName
  bool readConfigurationFromStorage(void);               // Read theFileName, copy the contents into configuration
  bool endStorage(void);                                 // End the storage (if required)

private:
  File _theStorage;             // SD File
  char *_theStorageName = NULL; // The name of the settings file - set by beginStorage
  int _csPin = -1;              // The SPI Chip Select pin - set by beginStorage
};

#endif

// For all platforms that support LittleFS.h
#if __has_include(<LittleFS.h>)

#define SFE_QUAD_SENSORS_LITTLEFS

#include "FS.h"
#include <LittleFS.h>

#define FORMAT_LITTLEFS_IF_FAILED true

class SFE_QUAD_Sensors__LittleFS : public SFE_QUAD_Sensors
{
public:
  ~SFE_QUAD_Sensors__LittleFS();

  bool beginStorage(const char *theFileName);            // Begin LittleFS
  bool writeConfigurationToStorage(bool append = false); // Write configuration to theFileName
  bool readConfigurationFromStorage(void);               // Read theFileName, copy the contents into configuration
  bool endStorage(void);                                 // End the storage (if required)

private:
  File _theStorage;             // SD File
  char *_theStorageName = NULL; // The name of the settings file - set by beginStorage
};

#endif

// For all platforms that support EEPROM.h
#if __has_include(<EEPROM.h>)

#define SFE_QUAD_SENSORS_EEPROM

#include <EEPROM.h>

class SFE_QUAD_Sensors__EEPROM : public SFE_QUAD_Sensors
{
public:
  ~SFE_QUAD_Sensors__EEPROM();

  bool beginStorage(void);                               // Begin LittleFS
  bool writeConfigurationToStorage(bool append = false); // Write configuration to theFileName
  bool readConfigurationFromStorage(void);               // Read theFileName, copy the contents into configuration
  bool endStorage(void);                                 // End the storage (if required)

private:
  bool checkStorageCRC(int *crc1address = NULL, uint8_t *CRC1 = NULL, uint8_t *CRC2 = NULL);
  bool writeStorageCRC(void);
};

#endif

// For SAMD platforms that support FlashAsEEPROM.h
#if __has_include(<FlashAsEEPROM_SAMD.h>) || (defined(OLS_COMPILER_NO_HAS_INCLUDE) && defined(ARDUINO_ARCH_SAMD))

#define SFE_QUAD_SENSORS_EEPROM

#include <FlashAsEEPROM_SAMD.hpp> // See: https://github.com/khoih-prog/FlashStorage_SAMD#howto-fix-multiple-definitions-linker-error

class SFE_QUAD_Sensors__EEPROM : public SFE_QUAD_Sensors
{
public:
  ~SFE_QUAD_Sensors__EEPROM();

  bool beginStorage(void);                               // Begin LittleFS
  bool writeConfigurationToStorage(bool append = false); // Write configuration to theFileName
  bool readConfigurationFromStorage(void);               // Read theFileName, copy the contents into configuration
  bool endStorage(void);                                 // End the storage (if required)

private:
  bool checkStorageCRC(int *crc1address = NULL, uint8_t *CRC1 = NULL, uint8_t *CRC2 = NULL);
  bool writeStorageCRC(void);
};

#endif

#endif
