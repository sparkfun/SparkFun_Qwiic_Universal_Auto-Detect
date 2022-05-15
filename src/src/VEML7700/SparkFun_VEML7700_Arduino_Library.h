/*!
 * @file SparkFun_VEML7700_Arduino_Library.h
 *
 * SparkFun VEML7700 Ambient Light Sensor Arduino Library
 *
 * This library facilitates communication with the VEML7700 over I<sup>2</sup>C.
 *
 * Want to support open source hardware? Buy a board from SparkFun!
 * <br>SparkX smôl Environmental Peripheral Board (SPX-18976): https://www.sparkfun.com/products/18976
 *
 * This library was written by:
 * Paul Clark
 * SparkFun Electronics
 * November 4th 2021
 *
 * Please see LICENSE.md for the license information
 *
 */

#ifndef __SFE_SMOL_POWER_BOARD__
#define __SFE_SMOL_POWER_BOARD__

#include <Arduino.h>
#include <Wire.h>

typedef uint16_t VEML7700_t;

/**  VEML7700 I2C address */
#define VEML7700_I2C_ADDRESS 0x10

/** VEML7700 error code returns */
typedef enum
{
  VEML7700_ERROR_READ = -4,
  VEML7700_ERROR_WRITE = -3,
  VEML7700_ERROR_INVALID_ADDRESS = -2,
  VEML7700_ERROR_UNDEFINED = -1,
  VEML7700_ERROR_SUCCESS = 1
} VEML7700_error_t;
const VEML7700_error_t VEML7700_SUCCESS = VEML7700_ERROR_SUCCESS;

/** Sensitivity mode selection */
typedef enum
{
  VEML7700_SENSITIVITY_x1,
  VEML7700_SENSITIVITY_x2,
  VEML7700_SENSITIVITY_x1_8,
  VEML7700_SENSITIVITY_x1_4,
  VEML7700_SENSITIVITY_INVALID
} VEML7700_sensitivity_mode_t;

/** ALS integration time setting
    Note: these are defined here in simple sequential order.
          The actual register settings are defined in VEML7700_config_integration_time_t */
typedef enum
{
  VEML7700_INTEGRATION_25ms,
  VEML7700_INTEGRATION_50ms,
  VEML7700_INTEGRATION_100ms,
  VEML7700_INTEGRATION_200ms,
  VEML7700_INTEGRATION_400ms,
  VEML7700_INTEGRATION_800ms,
  VEML7700_INTEGRATION_INVALID
} VEML7700_integration_time_t;

/** ALS persistence protect number setting */
typedef enum
{
  VEML7700_PERSISTENCE_1,
  VEML7700_PERSISTENCE_2,
  VEML7700_PERSISTENCE_4,
  VEML7700_PERSISTENCE_8,
  VEML7700_PERSISTENCE_INVALID
} VEML7700_persistence_protect_t;

/** ALS interrupt enable setting */
typedef enum
{
  VEML7700_INT_DISABLE,
  VEML7700_INT_ENABLE,
  VEML7700_INT_INVALID
} VEML7700_interrupt_enable_t;

/** ALS interrupt status, logical OR of the crossing low and high thrteshold INT triggers */
typedef enum
{
  VEML7700_INT_STATUS_NONE,
  VEML7700_INT_STATUS_HIGH,
  VEML7700_INT_STATUS_LOW,
  VEML7700_INT_STATUS_BOTH,
  VEML7700_INT_STATUS_INVALID
} VEML7700_interrupt_status_t;

/** ALS shut down setting */
typedef enum
{
  VEML7700_POWER_ON,
  VEML7700_SHUT_DOWN,
  VEML7700_SHUTDOWN_INVALID
} VEML7700_shutdown_t;

/** Communication interface for the VEML7700 */
class VEML7700
{
public:
  /** @brief Class to communicate with the VEML7700 */
  VEML7700();

  /** Begin the VEML7700. Default to Wire */
  bool begin(TwoWire &wirePort = Wire);

  /** Enable debug messages. Default to Serial */
  void enableDebugging(Stream &debugPort = Serial);
  void disableDebugging();

  bool isConnected();

  /** Configuration controls */

  VEML7700_error_t setShutdown(VEML7700_shutdown_t);
  VEML7700_error_t powerOn() { return setShutdown(VEML7700_POWER_ON); };
  VEML7700_error_t shutdown() { return setShutdown(VEML7700_SHUT_DOWN); };
  VEML7700_shutdown_t getShutdown();

  VEML7700_error_t setInterruptEnable(VEML7700_interrupt_enable_t ie);
  VEML7700_error_t getInterruptEnable(VEML7700_interrupt_enable_t *ie);
  VEML7700_interrupt_enable_t getInterruptEnable();

  VEML7700_error_t setPersistenceProtect(VEML7700_persistence_protect_t pp);
  VEML7700_error_t getPersistenceProtect(VEML7700_persistence_protect_t *pp);
  VEML7700_persistence_protect_t getPersistenceProtect();
  const char * getPersistenceProtectStr();

  VEML7700_error_t setIntegrationTime(VEML7700_integration_time_t it);
  VEML7700_error_t getIntegrationTime(VEML7700_integration_time_t *it);
  VEML7700_integration_time_t getIntegrationTime();
  const char * getIntegrationTimeStr();

  VEML7700_error_t setSensitivityMode(VEML7700_sensitivity_mode_t sm);
  VEML7700_error_t getSensitivityMode(VEML7700_sensitivity_mode_t *sm);
  VEML7700_sensitivity_mode_t getSensitivityMode();
  const char * getSensitivityModeStr();

  VEML7700_error_t setHighThreshold(uint16_t threshold);
  VEML7700_error_t getHighThreshold(uint16_t *threshold);
  uint16_t getHighThreshold();

  VEML7700_error_t setLowThreshold(uint16_t threshold);
  VEML7700_error_t getLowThreshold(uint16_t *threshold);
  uint16_t getLowThreshold();

  /** Read the sensor data */

  VEML7700_error_t getAmbientLight(uint16_t *ambient);
  uint16_t getAmbientLight();

  VEML7700_error_t getWhiteLevel(uint16_t *whiteLevel);
  uint16_t getWhiteLevel();

  VEML7700_error_t getLux(float *lux);
  float getLux();

  /** Note: reading the interrupt status register clears the interrupts.
            So, we need to check both interrupt flags in a single read. */
  VEML7700_error_t getInterruptStatus(VEML7700_interrupt_status_t *status);
  VEML7700_interrupt_status_t getInterruptStatus();

private:

  /** Provide bit field access to the configuration register */
  typedef struct
  {
    union
    {
      VEML7700_t all;
      struct
      {
        VEML7700_t CONFIG_REG_SD : 1; // ALS shut down
        VEML7700_t CONFIG_REG_INT_EN : 1; // ALS interrupt enable
        VEML7700_t CONFIG_REG_RES1 : 2; // Reserved
        VEML7700_t CONFIG_REG_PERS : 2; // ALS persistence protect number
        VEML7700_t CONFIG_REG_IT : 4; // ALS integration time
        VEML7700_t CONFIG_REG_RES2 : 1; // Reserved
        VEML7700_t CONFIG_REG_SM : 2; // ALS sensitivity mode
        VEML7700_t CONFIG_REG_RES3 : 3; // Reserved
      };
    };
  } VEML7700_CONFIGURATION_REGISTER_t;
  VEML7700_CONFIGURATION_REGISTER_t _configurationRegister;

  /** Provide bit field access to the interrupt status register
      Note: reading the interrupt status register clears the interrupts.
            So, we need to check both interrupt flags in a single read. */
  typedef struct
  {
    union
    {
      VEML7700_t all;
      struct
      {
        VEML7700_t INT_STATUS_REG_RES : 14; // Reserved
        // Bit 14 indicates if the high threshold was exceeded
        // Bit 15 indicates if the low threshold was exceeded
        VEML7700_t INT_STATUS_REG_INT_FLAGS : 2;
      };
    };
  } VEML7700_INTERRUPT_STATUS_REGISTER_t;

  /** VEML7700 Registers */
  typedef enum
  {
    VEML7700_CONFIGURATION_REGISTER,
    VEML7700_HIGH_THRESHOLD,
    VEML7700_LOW_THRESHOLD,
    VEML7700_ALS_OUTPUT = 4,
    VEML7700_WHITE_OUTPUT,
    VEML7700_INTERRUPT_STATUS
  } VEML7700_registers_t;

  /** ALS integration time setting */
  typedef enum
  {
    VEML7700_CONFIG_INTEGRATION_25ms = 0b1100,
    VEML7700_CONFIG_INTEGRATION_50ms = 0b1000,
    VEML7700_CONFIG_INTEGRATION_100ms = 0b0000,
    VEML7700_CONFIG_INTEGRATION_200ms = 0b0001,
    VEML7700_CONFIG_INTEGRATION_400ms = 0b0010,
    VEML7700_CONFIG_INTEGRATION_800ms = 0b0011,
    VEML7700_CONFIG_INTEGRATION_INVALID
  } VEML7700_config_integration_time_t;

  TwoWire *_i2cPort;
  Stream *_debugPort;
  uint8_t _deviceAddress;
  bool _debugEnabled;

  VEML7700_error_t _connected(void);

  /** I2C Read/Write */
  VEML7700_error_t readI2CBuffer(uint8_t *dest, VEML7700_registers_t startRegister, uint16_t len);
  VEML7700_error_t writeI2CBuffer(uint8_t *src, VEML7700_registers_t startRegister, uint16_t len);
  VEML7700_error_t readI2CRegister(VEML7700_t *dest, VEML7700_registers_t registerAddress);
  VEML7700_error_t writeI2CRegister(VEML7700_t data, VEML7700_registers_t registerAddress);

  /** Convert the (sequential) integration time into the corresponding (non-sequential) configuration value */
  VEML7700_config_integration_time_t integrationTimeConfig(VEML7700_integration_time_t it);
  /** Convert the (non-sequential) integration time config into the corresponding (sequential) integration time */
  VEML7700_integration_time_t integrationTimeFromConfig(VEML7700_config_integration_time_t it);

};

#endif