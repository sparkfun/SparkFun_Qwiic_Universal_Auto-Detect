/*!
 * @file SparkFun_VEML7700_Arduino_Library.h
 *
 * @mainpage SparkFun VEML7700 Ambient Light Sensor Arduino Library
 * 
 * @section intro_sec Introduction
 * 
 * This library facilitates communication with the VEML7700 over I<sup>2</sup>C.
 * 
 * Want to support open source hardware? Buy a board from SparkFun!
 * <br>SparkX smôl Environmental Peripheral Board (SPX-18976): https://www.sparkfun.com/products/18976
 * 
 * @section author Author
 * 
 * This library was written by:
 * Paul Clark
 * SparkFun Electronics
 * November 4th 2021
 * 
 * @section license License
 * 
 * MIT: please see LICENSE.md for the full license information
 * 
 */

#include "SparkFun_VEML7700_Arduino_Library.h"

#define VEML7700_REGISTER_LENGTH 2 // 2 bytes per register (16-bit)
#define VEML7700_NUM_INTEGRATION_TIMES 6 // Number of supported integration times
#define VEML7700_NUM_GAIN_SETTINGS 4 // Number of supported gain settings
#define VEML7700_NUM_PERSISTENCE_PROTECT 4 // Number of supported persistence protect settings

/** The sensor resolution vs. gain and integration time. Taken from the VEML7700 Application Note. */
const float VEML7700_LUX_RESOLUTION[VEML7700_NUM_GAIN_SETTINGS][VEML7700_NUM_INTEGRATION_TIMES] =
{
// 25ms    50ms    100ms   200ms   400ms   800ms
  {0.2304, 0.1152, 0.0576, 0.0288, 0.0144, 0.0072}, // Gain (sensitivity) 1
  {0.1152, 0.0576, 0.0288, 0.0144, 0.0072, 0.0036}, // Gain (sensitivity) 2
  {1.8432, 0.9216, 0.4608, 0.2304, 0.1152, 0.0576}, // Gain (sensitivity) 1/8
  {0.9216, 0.4608, 0.2304, 0.1152, 0.0576, 0.0288}  // Gain (sensitivity) 1/4
};

/** The VEML7700 gain (sensitivity) settings as text (string) */
const char *VEML7700_GAIN_SETTINGS[VEML7700_NUM_GAIN_SETTINGS + 1] =
{
  // Note: these are in the order defined by ALS_SM and VEML7700_sensitivity_mode_t
  "x1","x2","x1/8","x1/4","INVALID"
};

/** The VEML7700 integration time settings as text (strting) */
const char *VEML7700_INTEGRATION_TIMES[VEML7700_NUM_INTEGRATION_TIMES + 1] =
{
  // Note: these are in ascending (VEML7700_integration_time_t) order
  //       _not_ in ALS_IT (VEML7700_config_integration_time_t) order
  "25ms","50ms","100ms","200ms","400ms","800ms","INVALID"
};

/** The VEML7700 persistence protect settings as text (string) */
const char *VEML7700_PERSISTENCE_PROTECT_SETTINGS[VEML7700_NUM_PERSISTENCE_PROTECT + 1] =
{
  "1", "2", "4", "8", "INVALID"
};

VEML7700::VEML7700()
{
  _i2cPort = NULL;
  _debugPort = NULL;
  _deviceAddress = VEML7700_I2C_ADDRESS;
  _debugEnabled = false;
}

/**************************************************************************/
/*!
    @brief  Begin communication with the VEML7700
    @param  wirePort
            <br>The TwoWire (I2C) port used to communicate with the sensor.
            <br>Default is Wire.
    @return True if communication with the VEML7700 was successful, otherwise false.
*/
/**************************************************************************/
bool VEML7700::begin(TwoWire &wirePort)
{
  VEML7700_error_t err;

  _i2cPort = &wirePort;

  /** Write _configurationRegister into the VEML7700_CONFIGURATION_REGISTER.
      This will place the device into a known state, in case it was configured previously
      and remained powered on when the code was restarted. */

  _configurationRegister.all = 0x0000; // Clear the reserved bits
  _configurationRegister.CONFIG_REG_SD = VEML7700_POWER_ON;
  _configurationRegister.CONFIG_REG_INT_EN = VEML7700_INT_DISABLE;
  _configurationRegister.CONFIG_REG_PERS = VEML7700_PERSISTENCE_1;
  _configurationRegister.CONFIG_REG_IT = (VEML7700_t)integrationTimeConfig(VEML7700_INTEGRATION_100ms);
  _configurationRegister.CONFIG_REG_SM = VEML7700_SENSITIVITY_x1;

  err = writeI2CRegister(_configurationRegister.all, VEML7700_CONFIGURATION_REGISTER);

  if (_debugEnabled)
  {
    _debugPort->print(F("VEML7700::begin: I2C error: "));
    _debugPort->println(err);
  }

  return (err == VEML7700_ERROR_SUCCESS);
}

/**************************************************************************/
/*!
    @brief  Enable debug messages on the chosen Serial port (Stream)
    @param  debugPort
            <br>The Serial port (Stream) the debug messages will be printed to.
            <br>Default is Serial.
*/
/**************************************************************************/
void VEML7700::enableDebugging(Stream &debugPort)
{
  _debugPort = &debugPort;
  _debugEnabled = true;
}

/**************************************************************************/
/*!
    @brief  Disable debug messages
*/
/**************************************************************************/
void VEML7700::disableDebugging()
{
  _debugEnabled = false;
}

/**************************************************************************/
/*!
    @brief  Check that the VEML7700 is awake and communicating.
    @return True if communication with the VEML7700 was successful, otherwise false.
*/
/**************************************************************************/
boolean VEML7700::isConnected()
{
  if (_connected() != VEML7700_ERROR_SUCCESS)
  {
    return false;
  }
  return true;
}

VEML7700_error_t VEML7700::_connected()
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);

  if (err != VEML7700_ERROR_SUCCESS)
  {
    if (_debugEnabled)
    {
  		_debugPort->print(F("VEML7700::_connected: error: "));
      _debugPort->println(err);
    }
    return err;
  }

  if (_debugEnabled) _debugPort->println("VEML7700::_connected: success!");

  return VEML7700_ERROR_SUCCESS;
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's shut down setting (ALS_SD)
    @param  sd
            <br>The shut down setting. Possible values are:
            <br>VEML7700_POWER_ON
            <br>VEML7700_SHUT_DOWN
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::setShutdown(VEML7700_shutdown_t sd)
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  if (err != VEML7700_ERROR_SUCCESS)
  {
    return err;
  }

  _configurationRegister.CONFIG_REG_SD = (VEML7700_t)sd;

  return writeI2CRegister(_configurationRegister.all, VEML7700_CONFIGURATION_REGISTER);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's shut down setting (ALS_SD)
    @return VEML7700_POWER_ON or VEML7700_SHUT_DOWN if successful, VEML7700_SHUTDOWN_INVALID otherwise
*/
/**************************************************************************/
VEML7700_shutdown_t VEML7700::getShutdown()
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  if (err != VEML7700_ERROR_SUCCESS)
  {
    return VEML7700_SHUTDOWN_INVALID;
  }

  return ((VEML7700_shutdown_t)_configurationRegister.CONFIG_REG_SD);
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's interrupt enable setting (ALS_INT_EN)
    @param  ie
            <br>The interrupt enable setting. Possible values are:
            <br>VEML7700_INT_DISABLE
            <br>VEML7700_INT_ENABLE
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::setInterruptEnable(VEML7700_interrupt_enable_t ie)
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  if (err != VEML7700_ERROR_SUCCESS)
  {
    return err;
  }

  _configurationRegister.CONFIG_REG_INT_EN = (VEML7700_t)ie;

  return writeI2CRegister(_configurationRegister.all, VEML7700_CONFIGURATION_REGISTER);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's interrupt enable setting (ALS_INT_EN)
    @param  ie
            <br>Will be set to the interrupt enable setting on return
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::getInterruptEnable(VEML7700_interrupt_enable_t *ie)
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  
  if (err == VEML7700_ERROR_SUCCESS)
  {
    *ie = (VEML7700_interrupt_enable_t)_configurationRegister.CONFIG_REG_INT_EN;
  }
  else
  {
    *ie = VEML7700_INT_INVALID;
  }

  return (err);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's interrupt enable setting (ALS_INT_EN)
    @return  VEML7700_INT_DISABLE or VEML7700_INT_ENABLE if successful, VEML7700_INT_INVALID otherwise
*/
/**************************************************************************/
VEML7700_interrupt_enable_t VEML7700::getInterruptEnable()
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  if (err != VEML7700_ERROR_SUCCESS)
  {
    return VEML7700_INT_INVALID;
  }

  return ((VEML7700_interrupt_enable_t)_configurationRegister.CONFIG_REG_INT_EN);
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's persistence protect number setting (ALS_PERS)
    @param  pp
            <br>The persistence protect setting. Possible values are:
            <br>VEML7700_PERSISTENCE_1
            <br>VEML7700_PERSISTENCE_2
            <br>VEML7700_PERSISTENCE_4
            <br>VEML7700_PERSISTENCE_8
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::setPersistenceProtect(VEML7700_persistence_protect_t pp)
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  if (err != VEML7700_ERROR_SUCCESS)
  {
    return err;
  }

  _configurationRegister.CONFIG_REG_PERS = (VEML7700_t)pp;

  return writeI2CRegister(_configurationRegister.all, VEML7700_CONFIGURATION_REGISTER);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's persistence protect number setting (ALS_PERS)
    @param  pp
            <br>Will be set to the persistence protect number on return
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::getPersistenceProtect(VEML7700_persistence_protect_t *pp)
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  
  if (err == VEML7700_ERROR_SUCCESS)
  {
    *pp = (VEML7700_persistence_protect_t)_configurationRegister.CONFIG_REG_PERS;
  }
  else
  {
    *pp = VEML7700_PERSISTENCE_INVALID;
  }

  return (err);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's persistence protect number setting (ALS_PERS)
    @return If successful:
            <br>VEML7700_PERSISTENCE_1
            <br>VEML7700_PERSISTENCE_2
            <br>VEML7700_PERSISTENCE_4
            <br>VEML7700_PERSISTENCE_8
            <br>Otherwise:
            <br>VEML7700_PERSISTENCE_INVALID
*/
/**************************************************************************/
VEML7700_persistence_protect_t VEML7700::getPersistenceProtect()
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  if (err != VEML7700_ERROR_SUCCESS)
  {
    return VEML7700_PERSISTENCE_INVALID;
  }

  return ((VEML7700_persistence_protect_t)_configurationRegister.CONFIG_REG_PERS);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's persistence protect number setting (ALS_PERS) as printable text
*/
/**************************************************************************/
const char * VEML7700::getPersistenceProtectStr()
{
  VEML7700_persistence_protect_t pp;

  getPersistenceProtect(&pp);

  return (VEML7700_PERSISTENCE_PROTECT_SETTINGS[pp]);
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's integration time setting (ALS_IT)
            <br>Note: these are defined here in simple sequential order
            <br>The actual register settings are defined in VEML7700_config_integration_time_t
    @param  it
            <br>The integration time setting. Possible values are:
            <br>VEML7700_INTEGRATION_25ms
            <br>VEML7700_INTEGRATION_50ms
            <br>VEML7700_INTEGRATION_100ms
            <br>VEML7700_INTEGRATION_200ms
            <br>VEML7700_INTEGRATION_400ms
            <br>VEML7700_INTEGRATION_800ms
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::setIntegrationTime(VEML7700_integration_time_t it)
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  if (err != VEML7700_ERROR_SUCCESS)
  {
    return err;
  }

  _configurationRegister.CONFIG_REG_IT = (VEML7700_t)integrationTimeConfig(it);

  return writeI2CRegister(_configurationRegister.all, VEML7700_CONFIGURATION_REGISTER);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's integration time setting (ALS_IT)
    @param  it
            <br>Will be set to the intergration time setting on return
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::getIntegrationTime(VEML7700_integration_time_t *it)
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);

  if (err == VEML7700_ERROR_SUCCESS)
  {
    *it = (VEML7700_integration_time_t)integrationTimeFromConfig((VEML7700_config_integration_time_t)_configurationRegister.CONFIG_REG_IT);
  }
  else
  {
    *it = VEML7700_INTEGRATION_INVALID;
  }

  return (err);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's integration time setting (ALS_IT)
    @return If successful:
            <br>VEML7700_INTEGRATION_25ms
            <br>VEML7700_INTEGRATION_50ms
            <br>VEML7700_INTEGRATION_100ms
            <br>VEML7700_INTEGRATION_200ms
            <br>VEML7700_INTEGRATION_400ms
            <br>VEML7700_INTEGRATION_800ms
            <br>Otherwise:
            <br>VEML7700_INTEGRATION_INVALID
*/
/**************************************************************************/
VEML7700_integration_time_t VEML7700::getIntegrationTime()
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  if (err != VEML7700_ERROR_SUCCESS)
  {
    return VEML7700_INTEGRATION_INVALID;
  }

  return ((VEML7700_integration_time_t)integrationTimeFromConfig((VEML7700_config_integration_time_t)_configurationRegister.CONFIG_REG_IT));
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's integration time setting (ALS_IT) as printable text
*/
/**************************************************************************/
const char * VEML7700::getIntegrationTimeStr()
{
  VEML7700_integration_time_t it;

  getIntegrationTime(&it);

  return (VEML7700_INTEGRATION_TIMES[it]);
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's sensitivity mode selection (ALS_SM)
    @param  it
            <br>The sensitivity mode selection. Possible values are:
            <br>VEML7700_SENSITIVITY_x1
            <br>VEML7700_SENSITIVITY_x2
            <br>VEML7700_SENSITIVITY_x1_8
            <br>VEML7700_SENSITIVITY_x1_4
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::setSensitivityMode(VEML7700_sensitivity_mode_t sm)
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  if (err != VEML7700_ERROR_SUCCESS)
  {
    return err;
  }

  _configurationRegister.CONFIG_REG_SM = (VEML7700_t)sm;

  return writeI2CRegister(_configurationRegister.all, VEML7700_CONFIGURATION_REGISTER);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's sensitivity mode selection (ALS_SM)
    @param  sm
            <br>Will be set to the sensitivity mode selection on return
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::getSensitivityMode(VEML7700_sensitivity_mode_t *sm)
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  
  if (err == VEML7700_ERROR_SUCCESS)
  {
    *sm = (VEML7700_sensitivity_mode_t)_configurationRegister.CONFIG_REG_SM;
  }
  else
  {
    *sm = VEML7700_SENSITIVITY_INVALID;
  }

  return (err);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's sensitivity mode selection (ALS_SM)
    @return If successful:
            <br>VEML7700_SENSITIVITY_x1
            <br>VEML7700_SENSITIVITY_x2
            <br>VEML7700_SENSITIVITY_x1_8
            <br>VEML7700_SENSITIVITY_x1_4
            <br>Otherwise:
            <br>VEML7700_SENSITIVITY_INVALID
*/
/**************************************************************************/
VEML7700_sensitivity_mode_t VEML7700::getSensitivityMode()
{
  VEML7700_error_t err;

  err = readI2CRegister((VEML7700_t *)&_configurationRegister, VEML7700_CONFIGURATION_REGISTER);
  if (err != VEML7700_ERROR_SUCCESS)
  {
    return VEML7700_SENSITIVITY_INVALID;
  }

  return ((VEML7700_sensitivity_mode_t)_configurationRegister.CONFIG_REG_SM);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's sensitivity mode selection (ALS_SM) as printable text
*/
/**************************************************************************/
const char * VEML7700::getSensitivityModeStr()
{
  VEML7700_sensitivity_mode_t sm;

  getSensitivityMode(&sm);

  return (VEML7700_GAIN_SETTINGS[sm]);
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's ALS high threshold window setting (ALS_WH)
    @param  threshold
            <br>The threshold setting: 0x0000 to 0xFFFF
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::setHighThreshold(uint16_t threshold)
{
  return (writeI2CRegister((VEML7700_t)threshold, VEML7700_HIGH_THRESHOLD));
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's ALS high threshold window setting (ALS_WH)
    @param  threshold
            <br>Will be set to the threshold setting on return
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::getHighThreshold(uint16_t *threshold)
{
  return (readI2CRegister((VEML7700_t *)threshold, VEML7700_HIGH_THRESHOLD));
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's ALS high threshold window setting (ALS_WH)
    @return The threshold setting
*/
/**************************************************************************/
uint16_t VEML7700::getHighThreshold()
{
  uint16_t threshold;
  getHighThreshold(&threshold);
  return (threshold);
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's ALS low threshold window setting (ALS_WL)
    @param  threshold
            <br>The threshold setting: 0x0000 to 0xFFFF
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::setLowThreshold(uint16_t threshold)
{
  return (writeI2CRegister((VEML7700_t)threshold, VEML7700_LOW_THRESHOLD));
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's ALS low threshold window setting (ALS_WL)
    @param  threshold
            <br>Will be set to the threshold setting on return
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::getLowThreshold(uint16_t *threshold)
{
  return (readI2CRegister((VEML7700_t *)threshold, VEML7700_LOW_THRESHOLD));
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's ALS low threshold window setting (ALS_WL)
    @return The threshold setting
*/
/**************************************************************************/
uint16_t VEML7700::getLowThreshold()
{
  uint16_t threshold;
  getLowThreshold(&threshold);
  return (threshold);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's ambient light sensor data (ALS)
    @param  ambient
            <br>Will be set to the ambient level on return
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::getAmbientLight(uint16_t *ambient)
{
  return (readI2CRegister((VEML7700_t *)ambient, VEML7700_ALS_OUTPUT));
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's ambient light sensor data (ALS)
    @return The ambient light reading
*/
/**************************************************************************/
uint16_t VEML7700::getAmbientLight()
{
  uint16_t ambient;
  getAmbientLight(&ambient);
  return (ambient);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's white level data (WHITE)
    @param  whiteLevel
            <br>Will be set to the white level on return
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::getWhiteLevel(uint16_t *whiteLevel)
{
  return (readI2CRegister((VEML7700_t *)whiteLevel, VEML7700_WHITE_OUTPUT));
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's white level data (WHITE)
    @return The white level reading
*/
/**************************************************************************/
uint16_t VEML7700::getWhiteLevel()
{
  uint16_t whiteLevel;
  getWhiteLevel(&whiteLevel);
  return (whiteLevel);
}

/**************************************************************************/
/*!
    @brief  Read the sensor data and calculate the lux
    @param  lux
            <br>Will be set to the lux on return
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::getLux(float *lux)
{
  /** First, we need to extract the correct resolution from the VEML7700_LUX_RESOLUTION
      gain and integration time look up table. Let's begin by reading the gain
      (sensitivity) and integration time. */

  VEML7700_error_t err;
  VEML7700_sensitivity_mode_t sm;

  err = getSensitivityMode(&sm);

  if (err != VEML7700_ERROR_SUCCESS)
    return (err);

  if (_debugEnabled)
  {
    _debugPort->print(F("VEML7700::getLux: gain / sensitivity: "));
    _debugPort->println(VEML7700_GAIN_SETTINGS[sm]);
  }

  VEML7700_integration_time_t it;

  err = getIntegrationTime(&it);

  if (err != VEML7700_ERROR_SUCCESS)
    return (err);

  if (_debugEnabled)
  {
    _debugPort->print(F("VEML7700::getLux: integration time: "));
    _debugPort->println(VEML7700_INTEGRATION_TIMES[it]);
  }

  /** Now we can extract the correct resolution from the look up table. */
  float resolution = VEML7700_LUX_RESOLUTION[sm][it];

  if (_debugEnabled)
  {
    _debugPort->print(F("VEML7700::getLux: resolution: "));
    _debugPort->println(resolution, 4);
  }

  /** Now we read the ambient level and multiply it by the resolution */
  uint16_t ambient;

  err = getAmbientLight(&ambient);

  if (err != VEML7700_ERROR_SUCCESS)
    return (err);

  if (_debugEnabled)
  {
    _debugPort->print(F("VEML7700::getLux: ambient: "));
    _debugPort->println(ambient);
  }

  *lux = (float)ambient * resolution;

  if (_debugEnabled)
  {
    _debugPort->print(F("VEML7700::getLux: lux: "));
    _debugPort->println(*lux, 4);
  }

  return (VEML7700_ERROR_SUCCESS);
}

/**************************************************************************/
/*!
    @brief  Read the sensor data and calculate the lux
    @return The lux
*/
/**************************************************************************/
float VEML7700::getLux()
{
  float lux = 0.0;
  getLux(&lux);
  return (lux);
}

/**************************************************************************/
/*!
    @brief  Read the VEML7700's interrupt status register
            <br>Note: reading the interrupt status register clears the interrupts.
            <br>      So, we need to check both interrupt flags in a single read.
    @param  status
            <br>Will be set to the logical OR of ALS_IF_L and ALS_IF_H on return
            <br>Possible values are:
            <br>VEML7700_INT_STATUS_NONE
            <br>VEML7700_INT_STATUS_HIGH
            <br>VEML7700_INT_STATUS_LOW
            <br>VEML7700_INT_STATUS_BOTH
            <br>If an I2C error occurred, status will be:
            <br>VEML7700_INT_STATUS_INVALID
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_error_t VEML7700::getInterruptStatus(VEML7700_interrupt_status_t *status)
{
  VEML7700_error_t err;
  VEML7700_INTERRUPT_STATUS_REGISTER_t isr;

  err = readI2CRegister((VEML7700_t *)&isr, VEML7700_INTERRUPT_STATUS);

  if (err == VEML7700_ERROR_SUCCESS)
  {
    *status = (VEML7700_interrupt_status_t)isr.INT_STATUS_REG_INT_FLAGS;
  }
  else
  {
    *status = VEML7700_INT_STATUS_INVALID;
  }

  return (err);
}

/**************************************************************************/
/*!
    @brief  Read the VEML7700's interrupt status register
            <br>Note: reading the interrupt status register clears the interrupts.
            <br>      So, we need to check both interrupt flags in a single read.
    @return If successful:
            <br>VEML7700_INT_STATUS_NONE
            <br>VEML7700_INT_STATUS_HIGH
            <br>VEML7700_INT_STATUS_LOW
            <br>VEML7700_INT_STATUS_BOTH
            <br>Otherwise:
            <br>VEML7700_INT_STATUS_INVALID
*/
/**************************************************************************/
VEML7700_interrupt_status_t VEML7700::getInterruptStatus()
{
  VEML7700_error_t err;
  VEML7700_INTERRUPT_STATUS_REGISTER_t isr;

  err = readI2CRegister((VEML7700_t *)&isr, VEML7700_INTERRUPT_STATUS);

  if (err != VEML7700_ERROR_SUCCESS)
    return (VEML7700_INT_STATUS_INVALID);

  return ((VEML7700_interrupt_status_t)isr.INT_STATUS_REG_INT_FLAGS);  
}

VEML7700_error_t VEML7700::readI2CBuffer(uint8_t *dest, VEML7700_registers_t startRegister, uint16_t len)
{
  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(startRegister);
  if (_i2cPort->endTransmission(false) != 0)
  {
    if (_debugEnabled) _debugPort->println(F("VEML7700::readI2CBuffer: endTransmission error"));
    return VEML7700_ERROR_READ;
  }

  _i2cPort->requestFrom(_deviceAddress, (uint8_t)len);
  if (_debugEnabled)
  {
    _debugPort->print(F("VEML7700::readI2CBuffer: register: 0x"));
    _debugPort->println(startRegister, HEX);
    _debugPort->print(F("VEML7700::readI2CBuffer: device returned:"));
  }
  for (uint16_t i = 0; i < len; i++)
  {
    dest[i] = _i2cPort->read();
    if (_debugEnabled)
    {
    _debugPort->print(F(" 0x"));
    _debugPort->print(dest[i], HEX);
    }
  }
  if (_debugEnabled) _debugPort->println(F(""));

  return VEML7700_ERROR_SUCCESS;
}

VEML7700_error_t VEML7700::writeI2CBuffer(uint8_t *src, VEML7700_registers_t startRegister, uint16_t len)
{
  _i2cPort->beginTransmission(_deviceAddress);
  _i2cPort->write(startRegister);
  for (uint16_t i = 0; i < len; i++)
  {
    _i2cPort->write(src[i]);
  }
  if (_i2cPort->endTransmission(true) != 0)
  {
    return VEML7700_ERROR_WRITE;
  }
  return VEML7700_ERROR_SUCCESS;
}

VEML7700_error_t VEML7700::readI2CRegister(VEML7700_t *dest, VEML7700_registers_t registerAddress)
{
  VEML7700_error_t err;
  uint8_t tempDest[2];
  err = readI2CBuffer(tempDest, registerAddress, VEML7700_REGISTER_LENGTH);
  if (err == VEML7700_ERROR_SUCCESS)
  {
    *dest = (tempDest[0]) | ((VEML7700_t)tempDest[1] << 8);
  }
  return err;
}

VEML7700_error_t VEML7700::writeI2CRegister(VEML7700_t data, VEML7700_registers_t registerAddress)
{
  uint8_t d[2];
  // Write LSB first
  d[0] = (uint8_t)(data & 0x00FF);
  d[1] = (uint8_t)((data & 0xFF00) >> 8);
  return writeI2CBuffer(d, registerAddress, VEML7700_REGISTER_LENGTH);
}

VEML7700::VEML7700_config_integration_time_t VEML7700::integrationTimeConfig(VEML7700_integration_time_t it)
{
  switch (it)
  {
    case VEML7700_INTEGRATION_25ms:
      return (VEML7700_CONFIG_INTEGRATION_25ms);
      break;
    case VEML7700_INTEGRATION_50ms:
      return (VEML7700_CONFIG_INTEGRATION_50ms);
      break;
    case VEML7700_INTEGRATION_100ms:
      return (VEML7700_CONFIG_INTEGRATION_100ms);
      break;
    case VEML7700_INTEGRATION_200ms:
      return (VEML7700_CONFIG_INTEGRATION_200ms);
      break;
    case VEML7700_INTEGRATION_400ms:
      return (VEML7700_CONFIG_INTEGRATION_400ms);
      break;
    case VEML7700_INTEGRATION_800ms:
      return (VEML7700_CONFIG_INTEGRATION_800ms);
      break;
    default:
      break;
  }
  return (VEML7700_CONFIG_INTEGRATION_INVALID);
}

VEML7700_integration_time_t VEML7700::integrationTimeFromConfig(VEML7700::VEML7700_config_integration_time_t it)
{
  switch (it)
  {
    case VEML7700_CONFIG_INTEGRATION_25ms:
      return (VEML7700_INTEGRATION_25ms);
      break;
    case VEML7700_CONFIG_INTEGRATION_50ms:
      return (VEML7700_INTEGRATION_50ms);
      break;
    case VEML7700_CONFIG_INTEGRATION_100ms:
      return (VEML7700_INTEGRATION_100ms);
      break;
    case VEML7700_CONFIG_INTEGRATION_200ms:
      return (VEML7700_INTEGRATION_200ms);
      break;
    case VEML7700_CONFIG_INTEGRATION_400ms:
      return (VEML7700_INTEGRATION_400ms);
      break;
    case VEML7700_CONFIG_INTEGRATION_800ms:
      return (VEML7700_INTEGRATION_800ms);
      break;
    default:
      break;
  }
  return (VEML7700_INTEGRATION_INVALID);
}
