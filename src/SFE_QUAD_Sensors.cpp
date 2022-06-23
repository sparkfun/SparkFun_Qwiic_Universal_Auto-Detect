#include "SFE_QUAD_Sensors.h"

char *SFE_QUAD_Sensors_sprintf::_dtostrf(double value, char *buffer)
{
  bool negative = false;

  if (isnan(value))
  {
    strcpy(buffer, "nan");
    return (buffer + 3);
  }
  if (isinf(value))
  {
    strcpy(buffer, "inf");
    return (buffer + 3);
  }

  char *out = buffer;

  // Handle negative numbers
  if (value < 0.0)
  {
    negative = true;
    value = -value;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  // I optimized out most of the divisions
  double rounding = 2.0;
  for (uint8_t i = 0; i < _prec; ++i)
    rounding *= 10.0;
  rounding = 1.0 / rounding;

  value += rounding;

  // Figure out how big our number really is
  double tenpow = 1.0;
  int digitcount = 1;
  while (value >= 10.0 * tenpow)
  {
    tenpow *= 10.0;
    digitcount++;
  }

  value /= tenpow;

  // Handle negative sign
  if (negative)
    *out++ = '-';

  // Print the digits, and if necessary, the decimal point
  digitcount += _prec;
  int8_t digit = 0;
  while (digitcount-- > 0)
  {
    digit = (int8_t)value;
    if (digit > 9)
      digit = 9; // insurance
    *out++ = (char)('0' | digit);
    if ((digitcount == _prec) && (_prec > 0))
    {
      *out++ = '.';
    }
    value -= digit;
    value *= 10.0;
  }

  // make sure the string is terminated
  *out = 0;
  return out;
}

char *SFE_QUAD_Sensors_sprintf::_etoa(double value, char *buffer)
{
  if (sizeof(double) != sizeof(uint64_t))
  {
    // Kludge for platforms that do not support 64-bit double
    int expval = 0;

    if (value < 0.0)
    {
      if (value> -1.0)
      {
        while (value > -1.0)
        {
          value *= 10.0;
          expval--;
        }
      }
      else if (value <= -10.0)
      {
        while (value <= -10.0)
        {
          value /= 10.0;
          expval++;
        }
      }
    }
    else if (value > 0.0)
    {
      if (value < 1.0)
      {
        while (value < 1.0)
        {
          value *= 10.0;
          expval--;
        }
      }
      else if (value >= 10.0)
      {
        while (value >= 10.0)
        {
          value /= 10.0;
          expval++;
        }
      }
    }

    // output the floating part
    char *out = _dtostrf(value, buffer);

    // output the exponent part
    // output the exponential symbol
    *out++ = 'e';
    // output the exponent value
    *out++ = expval < 0 ? '-' : '+';
    unsigned char prec = _prec;
    _prec = 0; // Hack! Set _prec to zero to avoid printing a decimal point in the exponent
    out = _dtostrf(expval < 0 ? -expval : expval, out);
    _prec = prec; // Restore _prec
    return (out);
  }

  // 64-bit Platforms

  if (isnan(value))
  {
    strcpy(buffer, "nan");
    return (buffer + 3);
  }
  if (isinf(value))
  {
    strcpy(buffer, "inf");
    return (buffer + 3);
  }

  // determine the sign
  bool negative = value < 0;
  if (negative)
  {
    value = -value;
  }

  // determine the decimal exponent
  // based on the algorithm by David Gay (https://www.ampl.com/netlib/fp/dtoa.c)
  
  union
  {
    uint64_t U;
    double F;
  } conv;

  conv.F = value;
  int exp2 = (int)((conv.U >> 52U) & 0x07FFU) - 1023;          // effectively log2
  conv.U = (conv.U & ((1ULL << 52U) - 1U)) | (1023ULL << 52U); // drop the exponent so conv.F is now in [1,2)
  // now approximate log10 from the log2 integer part and an expansion of ln around 1.5
  int expval = (int)(0.1760912590558 + exp2 * 0.301029995663981 + (conv.F - 1.5) * 0.289529654602168);
  // now we want to compute 10^expval but we want to be sure it won't overflow
  exp2 = (int)(expval * 3.321928094887362 + 0.5);
  const double z = expval * 2.302585092994046 - exp2 * 0.6931471805599453;
  const double z2 = z * z;
  conv.U = (uint64_t)(exp2 + 1023) << 52U;
  // compute exp(z) using continued fractions, see https://en.wikipedia.org/wiki/Exponential_function#Continued_fractions_for_ex
  conv.F *= 1 + 2 * z / (2 - z + (z2 / (6 + (z2 / (10 + z2 / 14)))));
  // correct for rounding errors
  if (value < conv.F)
  {
    expval--;
    conv.F /= 10;
  }

  // rescale the float value
  if (expval)
  {
    value /= conv.F;
  }

  // output the floating part
  char *out = _dtostrf(negative ? -value : value, buffer);

  // output the exponent part
  // output the exponential symbol
  *out++ = 'e';
  // output the exponent value
  *out++ = expval < 0 ? '-' : '+';
  unsigned char prec = _prec;
  _prec = 0; // Hack! Set _prec to zero to avoid printing a decimal point in the exponent
  out = _dtostrf(expval < 0 ? -expval : expval, out);
  _prec = prec; // Restore _prec

  return out;
}

bool SFE_QUAD_Sensors_sprintf::expStrToDouble(const char *str, double *value)
{
  *value = 0.0;
  int dp = 0;
  double posNeg = 1.0;
  int exp = 0;
  int expPosNeg = 1;
  bool expSeen = false;
  const char *ptr = str;

  size_t strLen = strlen(str);
  if ((strLen == 0) || (strLen > 32))
    return (false);

  for (size_t i = 0; i < strLen; i++)
  {
    char c = *ptr;

    if ((dp == 0) && (expSeen == false)) // If a decimal point has not been seen and an 'e' has not been seen
    {
      if (c == '-')
      {
        posNeg = -1.0;
      }
      else if ((c >= '0') && (c <= '9'))
      {
        *value = *value * 10.0;
        *value = *value + (posNeg * (double)(c - '0'));
      }
      else if (c == '.')
      {
        dp = -1;
      }
      else if ((c == 'e') || (c == 'E'))
      {
        expSeen = true;
      }
    }
    else if ((dp != 0) && (expSeen == false)) // If a decimal point has been seen and an 'e' has not been seen
    {
      if ((c >= '0') && (c <= '9'))
      {
        *value = *value + (posNeg * ((double)(c - '0')) * pow(10, dp));
        dp -= 1;
      }
      else if ((c == 'e') || (c == 'E'))
      {
        expSeen = true;
      }
    }
    else // if (expSeen == true) // If an 'e' has been seen
    {
      if (c == '-')
      {
        expPosNeg = -1;
      }
      else if ((c >= '0') && (c <= '9'))
      {
        exp *= 10;
        exp += expPosNeg * ((int)(c - '0'));
      }
    }

    ptr++;
  }

  if (expSeen == true)
    *value = (*value) * pow(10, exp);

  return (true);
}

SFE_QUAD_Sensors::SFE_QUAD_Sensors(void)
{
  _i2cPort = NULL;
  _debugPort = NULL;
  _printDebug = false;
  _menuPort = NULL;

  readings = new char[1]; // Initialize readings
  *readings = 0;

  configuration = new char[1]; // Initialize configuration
  *configuration = 0;

  _head = NULL; // Initialize the sensor linked list head
}

SFE_QUAD_Sensors::~SFE_QUAD_Sensors(void)
{
  if (readings != NULL)
    delete[] readings;

  if (configuration != NULL)
    delete[] configuration;

  while (_head != NULL) // Have we found any sensors?
  {
    if (_head->_next == NULL) // Is the the last / only sensor?
    {
      _head->deleteSensorStorage();
      delete _head;
      _head = NULL;
    }
    else
    {
      SFE_QUAD_Sensor *thisSensor = _head;             // Start at the head
      SFE_QUAD_Sensor *nextSensor = thisSensor->_next; // Point to the next sensor
      while (nextSensor->_next != NULL)                // Keep going until we reach the end of the list
      {
        thisSensor = nextSensor;
        nextSensor = nextSensor->_next;
      }
      nextSensor->deleteSensorStorage();
      delete nextSensor; // Delete the sensor at the end of the list
      thisSensor->_next = NULL;
    }
  }
}

void SFE_QUAD_Sensors::setWirePort(TwoWire &port)
{
  _i2cPort = &port;
}

void SFE_QUAD_Sensors::enableDebugging(Stream &port)
{
  _debugPort = &port;
  _printDebug = true;
  theMenu.setDebugPort(port);
}

void SFE_QUAD_Sensors::setMenuPort(Stream &port)
{
  _menuPort = &port;
  theMenu.setMenuPort(port);
}

bool SFE_QUAD_Sensors::detectSensors(void)
{
  if (_i2cPort == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("detectSensors: _i2cPort is NULL. Did you forget to call setWirePort?"));
    return (false);
  }

  unsigned long detectStart = millis();

  // Begin by checking for a SHTC3 on the main branch
  bool shtc3OnMain = false;
  SFE_QUAD_Sensor *tryThisSensorType;
#if defined(INCLUDE_SFE_QUAD_SENSOR_ALL) || defined(INCLUDE_SFE_QUAD_SENSOR_SHTC3)
  tryThisSensorType = sensorFactory(Sensor_SHTC3);
  if (tryThisSensorType != NULL)
    if (tryThisSensorType->_classPtr != NULL)               // Check if sensor can be included
      if (tryThisSensorType->detectSensor(0x70, *_i2cPort)) // Check if the device is detected
      {
        shtc3OnMain = true;
        // if (_printDebug)
        //   _debugPort->println(F("detectSensors: SHTC3 found on main branch"));
      }
#endif

  // Next, detect any multiplexers with addresses 0x70 to 0x75 only
  // Leave 0x76 and 0x77 for the MS5637 / MS8607 / BME280
  bool *muxAddrs = new bool[6];
  if (muxAddrs == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("detectSensors: could not allocate memory for muxAddrs!"));
    return (false);
  }
  QWIICMUX *thisMux = new QWIICMUX;
  if (thisMux == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("detectSensors: could not allocate memory for thisMux!"));
    delete[] muxAddrs;
    return (false);
  }
  for (uint8_t muxAddr = 0x70; muxAddr <= 0x75; muxAddr++)
  {
    if (shtc3OnMain && (muxAddr == 0x70))
    {
      if (_printDebug)
      {
        _debugPort->println(F("detectSensors: SHTC3 found on main branch. Skipping mux detection for 0x70"));
      }
      muxAddrs[muxAddr - 0x70] = false;
    }
    else if (thisMux->begin(muxAddr, *_i2cPort))
    {
      if (_printDebug)
      {
        _debugPort->print(F("detectSensors: mux found at address 0x"));
        _debugPort->println(muxAddr, HEX);
      }
      muxAddrs[muxAddr - 0x70] = true;
    }
    else
    {
      muxAddrs[muxAddr - 0x70] = false;
    }
  }
  // QWIICMUX.begin disables the mux ports. We do not need to do it here.

  // if (_printDebug)
  // {
  //   _debugPort->print(F("detectSensors: mux detection took (ms): "));
  //   _debugPort->println(millis() - detectStart);
  //   detectStart = millis();
  // }

  // Check the main branch for any sensors, then check each port on each mux
  // Use a fake muxAddr of 0x6F to indicate the main branch. Replace this with zero below.
  for (uint8_t muxAddr = 0x6F; muxAddr <= 0x75; muxAddr++)
  {
    // Select the mux
    if ((muxAddr >= 0x70) && (muxAddrs[muxAddr - 0x70]))
    {
      if (!(shtc3OnMain && (muxAddr == 0x70)))
        thisMux->begin(muxAddr, *_i2cPort);
    }

    for (uint8_t muxPort = 0; muxPort <= 7; muxPort++)
    {
      // Set the mux port
      if ((muxAddr >= 0x70) && (muxAddrs[muxAddr - 0x70]))
      {
        if (!(shtc3OnMain && (muxAddr == 0x70)))
          thisMux->setPort(muxPort);
      }

      if (((muxAddr == 0x6F) && (muxPort == 0)) || ((muxAddr >= 0x70) && (muxAddrs[muxAddr - 0x70])))
      {
        // Check each sensor
        for (uint16_t type = 0; type < (uint16_t)SFE_QUAD_Sensor_Number_Of_Sensors; type++)
        {
          // Create a new sensor with the desired type
          tryThisSensorType = sensorFactory((SFEQUADSensorType)type);

          if (tryThisSensorType != NULL)
          {
            if (tryThisSensorType->_classPtr != NULL) // Check if sensor can be included
            {

              uint8_t numAddresses = tryThisSensorType->getNumI2cAddresses();

              for (uint8_t addressIndex = 0; addressIndex < numAddresses; addressIndex++) // For each possible sensor address
              {
                uint8_t tryThisAddress = tryThisSensorType->getI2cAddress(addressIndex);

                // Check if we have already found this sensor on the main branch. Skip it if we have
                if (sensorExists(tryThisSensorType->getSensorName(), tryThisAddress, 0, 0) != NULL)
                {
                  if (_printDebug)
                  {
                    _debugPort->print(F("detectSensors: skipping sensor "));
                    _debugPort->print(tryThisSensorType->getSensorName());
                    _debugPort->print(F(" address 0x"));
                    _debugPort->print(tryThisAddress, HEX);
                    _debugPort->println(F(" as it was found on the main branch"));
                  }
                }
                // If this is a MS5637, check if we have already found a MS8607. Skip it if we have
                else if ((strcmp(tryThisSensorType->getSensorName(), "MS5637") == 0) && ((sensorExists("MS8607", 0x40, 0, 0) != NULL) || (sensorExists("MS8607", 0x40, muxAddr == 0x6F ? 0 : muxAddr, muxPort) != NULL)))
                {
                  if (_printDebug)
                  {
                    _debugPort->print(F("detectSensors: skipping MS5637 detection for muxAddr 0x"));
                    _debugPort->print(muxAddr == 0x6F ? 0 : muxAddr, HEX);
                    _debugPort->print(F(" muxPort 0x"));
                    _debugPort->println(muxPort);
                  }
                }
                // If this is a VEML7700, check if we have already found a VEML6075. Skip it if we have
                else if ((strcmp(tryThisSensorType->getSensorName(), "VEML7700") == 0) && ((sensorExists("VEML6075", 0x10, 0, 0) != NULL) || (sensorExists("VEML6075", 0x10, muxAddr == 0x6F ? 0 : muxAddr, muxPort) != NULL)))
                {
                  if (_printDebug)
                  {
                    _debugPort->print(F("detectSensors: skipping VEML7700 detection for muxAddr 0x"));
                    _debugPort->print(muxAddr == 0x6F ? 0 : muxAddr, HEX);
                    _debugPort->print(F(" muxPort 0x"));
                    _debugPort->println(muxPort);
                  }
                }
                else
                {
                  if (tryThisSensorType->detectSensor(tryThisAddress, *_i2cPort)) // Check if the device is detected
                  {
                    if (_printDebug)
                    {
                      _debugPort->print(F("detectSensors: found sensor "));
                      _debugPort->print(tryThisSensorType->getSensorName());
                      _debugPort->print(F(" at address 0x"));
                      _debugPort->print(tryThisAddress, HEX);
                      if (muxAddr >= 0x70)
                      {
                        _debugPort->print(F(", mux address 0x"));
                        _debugPort->print(muxAddr, HEX);
                        _debugPort->print(F(" port "));
                        _debugPort->println(muxPort);
                      }
                      else
                        _debugPort->println();
                    }

                    if (_head == NULL) // Is this the first sensor we've found?
                    {
                      _head = sensorFactory((SFEQUADSensorType)type);
                      _head->_sensorAddress = tryThisAddress;
                      _head->_muxAddress = muxAddr == 0x6F ? 0 : muxAddr;
                      _head->_muxPort = muxAddr == 0x6F ? 0 : muxPort;
                      _head->_sprintf._prec = _sprintf._prec; // Inherit _prec from the Sensors
                    }
                    else
                    {
                      SFE_QUAD_Sensor *nextSensor = _head; // Start at the head
                      while (nextSensor->_next != NULL)    // Keep going until we reach the end of the list
                      {
                        nextSensor = nextSensor->_next;
                      }
                      nextSensor->_next = sensorFactory((SFEQUADSensorType)type);
                      nextSensor->_next->_sensorAddress = tryThisAddress;
                      nextSensor->_next->_muxAddress = muxAddr == 0x6F ? 0 : muxAddr;
                      nextSensor->_next->_muxPort = muxAddr == 0x6F ? 0 : muxPort;
                      nextSensor->_next->_sprintf._prec = _sprintf._prec; // Inherit _prec from the Sensors
                    }
                  }
                }
              }

              // if (_printDebug && (muxAddr == 0x6F))
              // {
              //   _debugPort->print(F("detectSensors: main branch detection of "));
              //   _debugPort->print(tryThisSensorType->getSensorName());
              //   _debugPort->print(F(" took (ms): "));
              //   _debugPort->println(millis() - detectStart);
              //   detectStart = millis();
              // }
            }

            else
            {
              if (_printDebug)
              {
                _debugPort->print(F("detectSensors: tryThisSensorType->_classPtr is NULL for sensor type"));
                _debugPort->println(type);
              }
            }

            tryThisSensorType->deleteSensorStorage(); // Be nice. Release the memory
            delete tryThisSensorType;
          }

          else
          {
            if (_printDebug)
            {
              _debugPort->print(F("detectSensors: tryThisSensorType is NULL for sensor type "));
              _debugPort->println(type);
            }
          }
        }
      }
    }
  }

  delete thisMux;
  delete[] muxAddrs;
  return (true);
}

bool SFE_QUAD_Sensors::beginSensors(void)
{
  if (_head == NULL) // If head is NULL no sensors have been found
  {
    if (_printDebug)
      _debugPort->println(F("beginSensors: no sensors found!"));
    return (false);
  }

  SFE_QUAD_Sensor *thisSensor = _head; // Point to the first sensor
  bool keepGoing = true;

  while (keepGoing)
  {
    // If required, configure the mux port
    if (thisSensor->_muxAddress >= 0x70)
    {
      QWIICMUX *thisMux = new QWIICMUX;
      if (thisMux == NULL)
      {
        if (_printDebug)
          _debugPort->println(F("beginSensors: could not allocate memory for thisMux!"));
        return (false);
      }
      thisMux->begin(thisSensor->_muxAddress, *_i2cPort);
      thisMux->setPort(thisSensor->_muxPort);
      delete thisMux;
    }

    thisSensor->beginSensor(thisSensor->_sensorAddress, *_i2cPort);

    if (thisSensor->_next != NULL)    // Have we reached the end of the sensor list?
      thisSensor = thisSensor->_next; // Point to the next sensor
    else
      keepGoing = false;
  }

  return (true);
}

bool SFE_QUAD_Sensors::initializeSensors(void)
{
  if (_head == NULL) // If head is NULL no sensors have been found
  {
    if (_printDebug)
      _debugPort->println(F("initializeSensors: no sensors found!"));
    return (false);
  }

  SFE_QUAD_Sensor *thisSensor = _head; // Point to the first sensor
  bool keepGoing = true;

  while (keepGoing)
  {
    // If required, configure the mux port
    if (thisSensor->_muxAddress >= 0x70)
    {
      QWIICMUX *thisMux = new QWIICMUX;
      if (thisMux == NULL)
      {
        if (_printDebug)
          _debugPort->println(F("initializeSensors: could not allocate memory for thisMux!"));
        return (false);
      }
      thisMux->begin(thisSensor->_muxAddress, *_i2cPort);
      thisMux->setPort(thisSensor->_muxPort);
      delete thisMux;
    }

    thisSensor->initializeSensor(thisSensor->_sensorAddress, *_i2cPort);

    if (thisSensor->_next != NULL)    // Have we reached the end of the sensor list?
      thisSensor = thisSensor->_next; // Point to the next sensor
    else
      keepGoing = false;
  }

  return (true);
}

bool SFE_QUAD_Sensors::setCustomInitialize(void (*pointer)(uint8_t sensorAddress, TwoWire &port, void *_classPtr), const char *sensorName, uint8_t i2cAddress, uint8_t muxAddress, uint8_t muxPort)
{
  SFE_QUAD_Sensor *theSensor = sensorExists(sensorName, i2cAddress, muxAddress, muxPort);

  if (theSensor == NULL)
    return (false);

  theSensor->setCustomInitializePtr(pointer);

  return (true);
}

bool SFE_QUAD_Sensors::setCustomInitialize(void (*pointer)(uint8_t sensorAddress, TwoWire &port, void *_classPtr), const char *sensorName)
{
  if (_head == NULL) // If head is NULL no sensors have been found
  {
    if (_printDebug)
      _debugPort->println(F("setCustomInitialize: no sensors found!"));
    return (false);
  }

  SFE_QUAD_Sensor *thisSensor = _head; // Point to the first sensor
  bool keepGoing = true;

  while (keepGoing)
  {
    SFE_QUAD_Sensor *theSensor = sensorExists(sensorName, thisSensor->_sensorAddress, thisSensor->_muxAddress, thisSensor->_muxPort);

    if (theSensor != NULL)
      theSensor->setCustomInitializePtr(pointer);

    if (thisSensor->_next != NULL)    // Have we reached the end of the sensor list?
      thisSensor = thisSensor->_next; // Point to the next sensor
    else
      keepGoing = false;
  }

  return (true);
}

bool SFE_QUAD_Sensors::getSensorReadings(void)
{
  if (_head == NULL) // If head is NULL no sensors have been found
  {
    if (_printDebug)
      _debugPort->println(F("getSensorReadings: no sensors found!"));
    return (false);
  }

  if (readings != NULL)
    delete[] readings;    // Delete the old readings
  readings = new char[1]; // Initialize readings
  *readings = 0;

  SFE_QUAD_Sensor *thisSensor = _head; // Point to the first sensor

  bool keepGoing = true;

  while (keepGoing)
  {
    if (thisSensor->_logSense != NULL) // Check if memory has been allocated for _logSense
    {
      if (thisSensor->_logSense[0]) // Is logging enable for this sensor?
      {
        // If required, configure the mux port
        if (thisSensor->_muxAddress >= 0x70)
        {
          QWIICMUX *thisMux = new QWIICMUX;
          if (thisMux == NULL)
          {
            if (_printDebug)
              _debugPort->println(F("getSensorReadings: could not allocate memory for thisMux!"));
            return (false);
          }
          thisMux->begin(thisSensor->_muxAddress, *_i2cPort);
          thisMux->setPort(thisSensor->_muxPort);
          delete thisMux;
        }

        uint8_t senseCount;
        thisSensor->getSenseCount(&senseCount);
        for (uint8_t sense = 0; sense < senseCount; sense++)
        {
          if (thisSensor->_logSense[sense + 1]) // Is logging enabled for this sense?
          {
            // Log this sense
            char *scratchpad = new char[32]; // Allocate memory to hold the sense reading temporarily
            if (scratchpad == NULL)          // Did the memory allocation fail?
            {
              if (_printDebug)
                _debugPort->println(F("getSensorReadings: scratchpad memory allocation failed!"));
              return (false);
            }
            thisSensor->getSenseReading(sense, scratchpad); // Get the reading. Store it in scratchpad
            size_t readingsLen = strlen(readings);          // Get the current readings length
            readingsLen += strlen(scratchpad);              // Get the length of the reading
            char *newReadings = new char[readingsLen + 2];  // Allocate memory to hold readings plus scrtachpad plus comma and null
            if (newReadings == NULL)                        // Did the memory allocation fail?
            {
              if (_printDebug)
                _debugPort->println(F("getSensorReadings: newReadings memory allocation failed!"));
              delete[] scratchpad; // Delete the scratchpad
              return (false);
            }
            memset(newReadings, 0, readingsLen + 2);         // Clear the memory to make sure it is null-terminated
            memcpy(newReadings, readings, strlen(readings)); // Copy in the existing readings
            strcat(newReadings, scratchpad);                 // Append the new reading from scratchpad
            strcat(newReadings, ",");                        // Add the comma (the fiaal comma will be deleted below)
            delete[] readings;                               // Delete readings
            readings = newReadings;                          // Make readings point to newReadings
            delete[] scratchpad;                             // Delete the scratchpad
          }
        }
      }
    }
    else
    {
      if (_printDebug)
      {
        _debugPort->print(F("getSensorReadings: _logSense is NULL for sensor "));
        _debugPort->print(thisSensor->getSensorName());
        _debugPort->print(F(" at address 0x"));
        _debugPort->print(thisSensor->_sensorAddress, HEX);
        if (thisSensor->_muxAddress >= 0x70)
        {
          _debugPort->print(F(", mux address 0x"));
          _debugPort->print(thisSensor->_muxAddress, HEX);
          _debugPort->print(F(" port "));
          _debugPort->println(thisSensor->_muxPort);
        }
        else
          _debugPort->println();
      }
    }

    if (thisSensor->_next == NULL) // Have we reached the end of the sensor list?
    {
      if (strlen(readings) > 0) // Delete the final comma
        memset(&readings[strlen(readings) - 1], 0, 1);
      keepGoing = false;
    }
    else
      thisSensor = thisSensor->_next; // Point to the next sensor
  }

  return (true);
}

bool SFE_QUAD_Sensors::getSensorNames(void)
{
  if (_head == NULL) // If head is NULL no sensors have been found
  {
    if (_printDebug)
      _debugPort->println(F("getSensorNames: no sensors found!"));
    return (false);
  }

  if (readings != NULL)
    delete[] readings;    // Delete the old readings
  readings = new char[1]; // Initialize readings
  *readings = 0;

  SFE_QUAD_Sensor *thisSensor = _head; // Point to the first sensor

  bool keepGoing = true;

  while (keepGoing)
  {
    if (thisSensor->_logSense != NULL) // Check if memory has been allocated for _logSense
    {
      if (thisSensor->_logSense[0]) // Is logging enable for this sensor?
      {
        uint8_t senseCount;
        thisSensor->getSenseCount(&senseCount);
        for (uint8_t sense = 0; sense < senseCount; sense++)
        {
          if (thisSensor->_logSense[sense + 1]) // Is logging enabled for this sense?
          {
            // Log this name
            char *scratchpad = new char[32]; // Allocate memory to hold the sensor name etc. temporarily
            if (scratchpad == NULL)          // Did the memory allocation fail?
            {
              if (_printDebug)
                _debugPort->println(F("getSensorNames: scratchpad memory allocation failed!"));
              return (false);
            }
            sprintf(scratchpad, "%s_%d_%d_%d,", thisSensor->getSensorName(), thisSensor->_sensorAddress,
                    thisSensor->_muxAddress, thisSensor->_muxPort); // Get the sensor name. Store it in scratchpad
            size_t readingsLen = strlen(readings);                  // Get the current readings length
            readingsLen += strlen(scratchpad);                      // Get the length of the reading
            char *newReadings = new char[readingsLen + 1];          // Allocate memory to hold readings plus scrtachpad plus comma and null
            if (newReadings == NULL)                                // Did the memory allocation fail?
            {
              if (_printDebug)
                _debugPort->println(F("getSensorNames: newReadings memory allocation failed!"));
              delete[] scratchpad; // Delete the scratchpad
              return (false);
            }
            memset(newReadings, 0, readingsLen + 1);         // Clear the memory to make sure it is null-terminated
            memcpy(newReadings, readings, strlen(readings)); // Copy in the existing readings
            strcat(newReadings, scratchpad);                 // Append the new reading from scratchpad
            delete[] readings;                               // Delete readings
            readings = newReadings;                          // Make readings point to newReadings
            delete[] scratchpad;                             // Delete the scratchpad
          }
        }
      }
    }

    if (thisSensor->_next == NULL) // Have we reached the end of the sensor list?
    {
      if (strlen(readings) > 0) // Delete the final comma
        memset(&readings[strlen(readings) - 1], 0, 1);
      keepGoing = false;
    }
    else
      thisSensor = thisSensor->_next; // Point to the next sensor
  }

  return (true);
}

bool SFE_QUAD_Sensors::getSenseNames(void)
{
  if (_head == NULL) // If head is NULL no sensors have been found
  {
    if (_printDebug)
      _debugPort->println(F("getSenseNames: no sensors found!"));
    return (false);
  }

  if (readings != NULL)
    delete[] readings;    // Delete the old readings
  readings = new char[1]; // Initialize readings
  *readings = 0;

  SFE_QUAD_Sensor *thisSensor = _head; // Point to the first sensor

  bool keepGoing = true;

  while (keepGoing)
  {
    if (thisSensor->_logSense != NULL) // Check if memory has been allocated for _logSense
    {
      if (thisSensor->_logSense[0]) // Is logging enable for this sensor?
      {
        uint8_t senseCount;
        thisSensor->getSenseCount(&senseCount);
        for (uint8_t sense = 0; sense < senseCount; sense++)
        {
          if (thisSensor->_logSense[sense + 1]) // Is logging enabled for this sense?
          {
            // Log this name
            char *scratchpad = new char[thisSensor->getSenseNameMaxLen() + 2]; // Allocate memory to hold the sense name temporarily
            if (scratchpad == NULL)                                            // Did the memory allocation fail?
            {
              if (_printDebug)
                _debugPort->println(F("getSenseNames: scratchpad memory allocation failed!"));
              return (false);
            }
            sprintf(scratchpad, "%s,", thisSensor->getSenseName(sense)); // Get the sensor name. Store it in scratchpad
            size_t readingsLen = strlen(readings);                       // Get the current readings length
            readingsLen += strlen(scratchpad);                           // Get the length of the reading
            char *newReadings = new char[readingsLen + 1];               // Allocate memory to hold readings plus scrtachpad plus comma and null
            if (newReadings == NULL)                                     // Did the memory allocation fail?
            {
              if (_printDebug)
                _debugPort->println(F("getSenseNames: newReadings memory allocation failed!"));
              delete[] scratchpad; // Delete the scratchpad
              return (false);
            }
            memset(newReadings, 0, readingsLen + 1);         // Clear the memory to make sure it is null-terminated
            memcpy(newReadings, readings, strlen(readings)); // Copy in the existing readings
            strcat(newReadings, scratchpad);                 // Append the new reading from scratchpad
            delete[] readings;                               // Delete readings
            readings = newReadings;                          // Make readings point to newReadings
            delete[] scratchpad;                             // Delete the scratchpad
          }
        }
      }
    }

    if (thisSensor->_next == NULL) // Have we reached the end of the sensor list?
    {
      if (strlen(readings) > 0) // Delete the final comma
        memset(&readings[strlen(readings) - 1], 0, 1);
      keepGoing = false;
    }
    else
      thisSensor = thisSensor->_next; // Point to the next sensor
  }

  return (true);
}

SFE_QUAD_Sensor *SFE_QUAD_Sensors::sensorExists(const char *sensorName, uint8_t i2cAddress, uint8_t muxAddress, uint8_t muxPort)
{
  if (_head == NULL) // If head is NULL no sensors have been found
  {
    // if (_printDebug)
    //   _debugPort->println(F("sensorExists: no sensors found!"));
    return (NULL);
  }

  SFE_QUAD_Sensor *thisSensor = _head; // Point to the first sensor
  bool keepGoing = true;

  while (keepGoing)
  {
    if ((strcmp(sensorName, thisSensor->getSensorName()) == 0) // Check for a name match
        && (i2cAddress == thisSensor->_sensorAddress)          // Check for an address match
        && (muxAddress == thisSensor->_muxAddress)             // Check for a mux address match
        && (muxPort == thisSensor->_muxPort))                  // Check for a port match
      return (thisSensor);

    if (thisSensor->_next != NULL)    // Have we reached the end of the sensor list?
      thisSensor = thisSensor->_next; // Point to the next sensor
    else
      keepGoing = false;
  }

  return (NULL);
}

bool SFE_QUAD_Sensors::loggingMenu(void)
{
  if (_menuPort == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("loggingMenu: _menuPort is NULL. Did you forget to call setMenuPort?"));
    return (false);
  }

  while (_menuPort->available()) // Clear the menu serial buffer
    _menuPort->read();

  if (_head == NULL) // If head is NULL no sensors have been found
  {
    _menuPort->println(F("loggingMenu: no sensors found!"));
    return (false);
  }

  while (1)
  {
    SFE_QUAD_Sensor *thisSensor = _head; // Point to the first sensor
    uint32_t menuItems = 1;
    bool keepGoing = true;

    _menuPort->println();

    while (keepGoing)
    {
      _menuPort->print(F("Sensor "));
      _menuPort->print(thisSensor->getSensorName());
      _menuPort->print(F(" at address 0x"));
      _menuPort->print(thisSensor->_sensorAddress, HEX);

      if (thisSensor->_muxAddress >= 0x70)
      {
        _menuPort->print(F(", mux address 0x"));
        _menuPort->print(thisSensor->_muxAddress, HEX);
        _menuPort->print(F(" port "));
        _menuPort->println(thisSensor->_muxPort);
      }
      else
        _menuPort->println();

      _menuPort->print(menuItems);
      _menuPort->print(F("\t: Log this sensor "));
      uint8_t maxSenseNameLen = thisSensor->getSenseNameMaxLen();
      if (maxSenseNameLen > strlen("this sensor "))
      {
        for (uint8_t i = 0; i < (maxSenseNameLen - strlen("this sensor ")); i++)
          _menuPort->print(F(" "));
      }
      _menuPort->println(thisSensor->_logSense[0] ? F(" : Yes") : F(" : No"));
      menuItems++;

      if (thisSensor->_logSense[0])
      {
        uint8_t senseCount;
        thisSensor->getSenseCount(&senseCount);
        for (uint8_t sense = 0; sense < senseCount; sense++)
        {
          _menuPort->print(menuItems);
          _menuPort->print(F("\t: Log "));
          _menuPort->print(thisSensor->getSenseName(sense));
          uint8_t thisSenseNameLen = strlen(thisSensor->getSenseName(sense));
          for (uint8_t i = 0; i < (maxSenseNameLen - thisSenseNameLen); i++)
            _menuPort->print(F(" "));
          if (maxSenseNameLen < strlen("this sensor "))
            for (uint8_t i = 0; i < (strlen("this sensor ") - maxSenseNameLen); i++)
              _menuPort->print(F(" "));
          _menuPort->println(thisSensor->_logSense[sense + 1] ? F(" : Yes") : F(" : No"));
          menuItems++;
        }
      }

      if (thisSensor->_next == NULL) // Have we reached the end of the sensor list?
        keepGoing = false;
      else
        thisSensor = thisSensor->_next; // Point to the next sensor
    }

    _menuPort->println(F("Enter a number to toggle the setting, or enter 0 to exit:"));

    uint32_t menuChoice = getMenuChoice(10000UL); // Get menu choice. Timeout after 10 seconds

    if (menuChoice == 0)
    {
      _menuPort->println(F("loggingMenu: exiting..."));
      return (true);
    }

    thisSensor = _head; // Point to the first sensor
    menuItems = 1;
    keepGoing = true;

    while (keepGoing)
    {
      if (menuChoice == menuItems)
      {
        thisSensor->_logSense[0] ^= 1; // Toggle "Log this sensor"
        keepGoing = false;
      }
      else
      {
        menuItems++;

        if (thisSensor->_logSense[0])
        {
          uint8_t senseCount;
          thisSensor->getSenseCount(&senseCount);
          for (uint8_t sense = 0; sense < senseCount; sense++)
          {
            if (menuChoice == menuItems)
            {
              thisSensor->_logSense[sense + 1] ^= 1; // Toggle the sense
              keepGoing = false;
            }
            menuItems++;
          }
        }

        if (thisSensor->_next == NULL) // Have we reached the end of the sensor list?
          keepGoing = false;
        else
          thisSensor = thisSensor->_next; // Point to the next sensor
      }
    }
  }
}

uint32_t SFE_QUAD_Sensors::getMenuChoice(unsigned long timeout)
{
  if (_menuPort == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("getMenuChoice: _menuPort is NULL. Did you forget to call setMenuPort?"));
    return (0);
  }

  unsigned long startTime = millis();
  bool keepGoing = true;
  uint32_t choice = 0;

  while (_menuPort->available()) // Clear the menu serial buffer
    _menuPort->read();

  while (keepGoing && (millis() < (startTime + timeout)))
  {
    if (_menuPort->available())
    {
      startTime = millis(); // Update startTime each time we get a character
      char c = _menuPort->read();

      if ((c >= '0') && (c <= '9'))
      {
        choice *= 10;
        choice += c - '0';
      }
      else if ((c == '\r') || (c == '\n'))
        keepGoing = false;
    }
  }

  while (_menuPort->available()) // Clear the menu serial buffer
    _menuPort->read();

  if (keepGoing) // Did we time out?
  {
    _menuPort->println(F("No user input received."));
    return (0);
  }

  return (choice);
}

bool SFE_QUAD_Sensors::settingMenu(void)
{
  if (_menuPort == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("settingMenu: _menuPort is NULL. Did you forget to call setMenuPort?"));
    return (false);
  }

  while (_menuPort->available()) // Clear the menu serial buffer
    _menuPort->read();

  if (_head == NULL) // If head is NULL no sensors have been found
  {
    _menuPort->println(F("settingMenu: no sensors found!"));
    return (false);
  }

  while (1)
  {
    SFE_QUAD_Sensor *thisSensor = _head; // Point to the first sensor
    uint32_t menuItems = 1;
    bool keepGoing = true;

    _menuPort->println();

    while (keepGoing)
    {
      uint8_t settingCount;
      thisSensor->getSettingCount(&settingCount);

      if (settingCount > 0)
      {
        _menuPort->print(F("Sensor "));
        _menuPort->print(thisSensor->getSensorName());
        _menuPort->print(F(" at address 0x"));
        _menuPort->print(thisSensor->_sensorAddress, HEX);

        if (thisSensor->_muxAddress >= 0x70)
        {
          _menuPort->print(F(", mux address 0x"));
          _menuPort->print(thisSensor->_muxAddress, HEX);
          _menuPort->print(F(" port "));
          _menuPort->println(thisSensor->_muxPort);
        }
        else
          _menuPort->println();

        for (uint8_t setting = 0; setting < settingCount; setting++)
        {
          _menuPort->print(menuItems);
          _menuPort->print(F("\t: "));
          _menuPort->println(thisSensor->getSettingName(setting));
          menuItems++;
        }
      }

      if (thisSensor->_next == NULL) // Have we reached the end of the sensor list?
        keepGoing = false;
      else
        thisSensor = thisSensor->_next; // Point to the next sensor
    }

    if (menuItems == 1)
    {
      _menuPort->println(F("settingMenu: the connected sensors have no settings! Exiting..."));
      return (false);
    }

    _menuPort->println(F("Enter a number to change the setting, or enter 0 to exit:"));

    uint32_t menuChoice = getMenuChoice(10000UL); // Get menu choice. Timeout after 10 seconds

    if (menuChoice == 0)
    {
      _menuPort->println(F("settingMenu: exiting..."));
      return (true);
    }

    thisSensor = _head; // Point to the first sensor
    menuItems = 1;
    keepGoing = true;
    bool result;

    while (keepGoing)
    {
      uint8_t settingCount;
      result = thisSensor->getSettingCount(&settingCount); // getSettingCount will return false if the count is not defined
      if (result)
      {
        for (uint8_t setting = 0; setting < settingCount; setting++)
        {
          if (menuChoice == menuItems)
          {
            // If required, configure the mux port
            if (thisSensor->_muxAddress >= 0x70)
            {
              QWIICMUX *thisMux = new QWIICMUX;
              if (thisMux == NULL)
              {
                if (_printDebug)
                  _debugPort->println(F("settingMenu: could not allocate memory for thisMux!"));
                return (false);
              }
              thisMux->begin(thisSensor->_muxAddress, *_i2cPort);
              thisMux->setPort(thisSensor->_muxPort);
              delete thisMux;
            }

            SFE_QUAD_Sensor::SFE_QUAD_Sensor_Setting_Type_e settingType;
            result = thisSensor->getSettingType(setting, &settingType);
            if (result)
            {
              switch (settingType)
              {
                SFE_QUAD_Sensor::SFE_QUAD_Sensor_Every_Type_t value;
              case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_NONE:
                result = thisSensor->setSetting(setting, &value);
                break;
              case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_BOOL:
                _menuPort->println(F("Enter a bool (0 or 1) (or wait 10 seconds to abort): "));
                result = getSettingValueDouble(&value.DOUBLE, 10000UL); // Get setting value. Timeout after 10 seconds
                if (result)
                {
                  value.BOOL = value.DOUBLE == 0.0 ? false : true;
                  result = thisSensor->setSetting(setting, &value);
                }
                break;
              case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_FLOAT:
                _menuPort->println(F("Enter a floating point value (or wait 10 seconds to abort): "));
                result = getSettingValueDouble(&value.DOUBLE, 10000UL); // Get setting value. Timeout after 10 seconds
                if (result)
                {
                  value.FLOAT = (float)value.DOUBLE;
                  result = thisSensor->setSetting(setting, &value);
                }
                break;
              case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_DOUBLE:
                _menuPort->println(F("Enter a floating point value (or wait 10 seconds to abort): "));
                result = getSettingValueDouble(&value.DOUBLE, 10000UL); // Get setting value. Timeout after 10 seconds
                if (result)
                {
                  result = thisSensor->setSetting(setting, &value);
                }
                break;
              case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_INT:
                _menuPort->println(F("Enter an integer value (signed) (or wait 10 seconds to abort): "));
                result = getSettingValueDouble(&value.DOUBLE, 10000UL); // Get setting value. Timeout after 10 seconds
                if (result)
                {
                  value.INT = (int)value.DOUBLE;
                  result = thisSensor->setSetting(setting, &value);
                }
                break;
              case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_UINT8_T:
                _menuPort->println(F("Enter an integer value (unsigned, 8-bit) (or wait 10 seconds to abort): "));
                result = getSettingValueDouble(&value.DOUBLE, 10000UL); // Get setting value. Timeout after 10 seconds
                if (result)
                {
                  value.UINT8_T = (uint8_t)value.DOUBLE;
                  result = thisSensor->setSetting(setting, &value);
                }
                break;
              case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_UINT16_T:
                _menuPort->println(F("Enter an integer value (unsigned, 16-bit) (or wait 10 seconds to abort): "));
                result = getSettingValueDouble(&value.DOUBLE, 10000UL); // Get setting value. Timeout after 10 seconds
                if (result)
                {
                  value.UINT16_T = (uint16_t)value.DOUBLE;
                  result = thisSensor->setSetting(setting, &value);
                }
                break;
              case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_UINT32_T:
                _menuPort->println(F("Enter an integer value (unsigned, 32-bit) (or wait 10 seconds to abort): "));
                result = getSettingValueDouble(&value.DOUBLE, 10000UL); // Get setting value. Timeout after 10 seconds
                if (result)
                {
                  value.UINT32_T = (uint32_t)value.DOUBLE;
                  result = thisSensor->setSetting(setting, &value);
                }
                break;
              default:
                result = false;
                break;
              }
              if (result)
              {
                _menuPort->print(thisSensor->getSettingName(setting));
                _menuPort->println(F(" was successful"));
              }
              else
              {
                _menuPort->print(thisSensor->getSettingName(setting));
                _menuPort->println(F(" failed!"));
              }
            }
            keepGoing = false;
          }
          menuItems++;
        }
      }

      if (thisSensor->_next == NULL) // Have we reached the end of the sensor list?
        keepGoing = false;
      else
        thisSensor = thisSensor->_next; // Point to the next sensor
    }
  }
}

bool SFE_QUAD_Sensors::getSettingValueDouble(double *value, unsigned long timeout)
{
  if (_menuPort == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("getSettingValueDouble: _menuPort is NULL. Did you forget to call setMenuPort?"));
    return (false);
  }

  unsigned long startTime = millis();
  bool keepGoing = true;
  *value = 0.0;
  int dp = 0;
  double posNeg = 1.0;
  int exp = 0;
  int expPosNeg = 1;
  bool expSeen = false;

  while (_menuPort->available()) // Clear the menu serial buffer
    _menuPort->read();

  while (keepGoing && (millis() < (startTime + timeout)))
  {
    if (_menuPort->available())
    {
      startTime = millis(); // Update startTime each time we get a character
      char c = _menuPort->read();

      if ((c == '\r') || (c == '\n'))
        keepGoing = false;
      else if ((dp == 0) && (expSeen == false)) // If a decimal point has not been seen and an 'e' has not been seen
      {
        if (c == '-')
        {
          posNeg = -1.0;
        }
        else if ((c >= '0') && (c <= '9'))
        {
          *value = *value * 10.0;
          *value = *value + (posNeg * (double)(c - '0'));
        }
        else if (c == '.')
        {
          dp = -1;
        }
        else if ((c == 'e') || (c == 'E'))
        {
          expSeen = true;
        }
      }
      else if ((dp != 0) && (expSeen == false)) // If a decimal point has been seen and an 'e' has not been seen
      {
        if ((c >= '0') && (c <= '9'))
        {
          *value = *value + (posNeg * ((double)(c - '0')) * pow(10, dp));
          dp -= 1;
        }
        else if ((c == 'e') || (c == 'E'))
        {
          expSeen = true;
        }
      }
      else // if (expSeen == true) // If an 'e' has been seen
      {
        if (c == '-')
        {
          expPosNeg = -1;
        }
        else if ((c >= '0') && (c <= '9'))
        {
          exp *= 10;
          exp += expPosNeg * ((int)(c - '0'));
        }
      }
    }
  }

  if (_printDebug)
  {
    _debugPort->print(F("getSettingValueDouble: value is "));
    _debugPort->println(*value);
    _debugPort->print(F("getSettingValueDouble: expSeen is "));
    _debugPort->println(expSeen);
    _debugPort->print(F("getSettingValueDouble: exp is "));
    _debugPort->println(exp);
  }

  if (expSeen == true)
    *value = (*value) * pow(10, exp);

  if (_printDebug)
  {
    _debugPort->print(F("getSettingValueDouble: value is "));
    _debugPort->println(*value);
  }

  while (_menuPort->available()) // Clear the menu serial buffer
    _menuPort->read();

  if (keepGoing) // Did we time out?
  {
    _menuPort->println(F("No user input received."));
    return (false);
  }

  return (true);
}

bool SFE_QUAD_Sensors::getSensorAndMenuConfiguration(void)
{
  if ((_head == NULL)             // If head is NULL no sensors have been found
      && (theMenu._head == NULL)) // If theMenu _head is NULL no menu items have been added
  {
    if (_printDebug)
      _debugPort->println(F("getSensorAndMenuConfiguration: no sensor or menu items found!"));
    return (false);
  }

  if (_head == NULL) // If head is NULL no sensors have been found
  {
    if (_printDebug)
      _debugPort->println(F("getSensorAndMenuConfiguration: no sensors found!"));
  }
  else
  {
    if (configuration != NULL)
      delete[] configuration;    // Delete the old configuration
    configuration = new char[1]; // Initialize configuration
    *configuration = 0;

    SFE_QUAD_Sensor *thisSensor = _head; // Point to the first sensor

    bool keepGoing = true;

    while (keepGoing)
    {
      // First, record the logging settings
      uint8_t senseCount;
      thisSensor->getSenseCount(&senseCount);
      char loggingStr[strlen(thisSensor->getSensorName()) + 32 + (senseCount * 2)]; // TODO: find a better way to do this!
      // Use -1 to indicate that these are the logging settings, not a configuration item
      sprintf(loggingStr, "%s,%d,%d,%d,-1,", thisSensor->getSensorName(), thisSensor->_sensorAddress, thisSensor->_muxAddress, thisSensor->_muxPort);
      for (uint8_t sense = 0; sense <= senseCount; sense++)
      {
        if (thisSensor->_logSense[sense] == 0)
          strcat(loggingStr, "0");
        else
          strcat(loggingStr, "1");
      }
      strcat(loggingStr, "\r\n");

      if (_printDebug)
      {
        _debugPort->print(F("getSensorAndMenuConfiguration: logging settings for : "));
        _debugPort->print(loggingStr);
      }

      size_t configLen = 0;
      configLen = strlen(configuration);         // Get the current configuration length
      configLen += strlen(loggingStr);           // Get the length of the logging settings
      char *newConfig = new char[configLen + 1]; // Allocate memory to hold configuration plus scratchpad plus a null
      if (newConfig == NULL)                     // Did the memory allocation fail?
      {
        if (_printDebug)
          _debugPort->println(F("getSensorAndMenuConfiguration: newConfig memory allocation failed!"));
        return (false);
      }
      memset(newConfig, 0, configLen + 1);                     // Clear the memory to make sure it is null-terminated
      memcpy(newConfig, configuration, strlen(configuration)); // Copy in the existing readings
      strcat(newConfig, loggingStr);                           // Append the logging string
      delete[] configuration;                                  // Delete configuration
      configuration = newConfig;                               // Make config point to newConfig

      // If required, configure the mux port
      if (thisSensor->_muxAddress >= 0x70)
      {
        QWIICMUX *thisMux = new QWIICMUX;
        if (thisMux == NULL)
        {
          if (_printDebug)
            _debugPort->println(F("getSensorAndMenuConfiguration: could not allocate memory for thisMux!"));
          return (false);
        }
        thisMux->begin(thisSensor->_muxAddress, *_i2cPort);
        thisMux->setPort(thisSensor->_muxPort);
        delete thisMux;
      }

      uint8_t configCount;
      bool result = thisSensor->getConfigurationItemCount(&configCount);

      if (result && (configCount > 0))
      {
        for (uint8_t configItem = 0; configItem < configCount; configItem++)
        {
          if (_printDebug)
          {
            _debugPort->print(F("getSensorAndMenuConfiguration: configuration for : "));
            _debugPort->println(thisSensor->getSensorName());
          }

          configLen = 0;
          configLen += strlen(thisSensor->getSensorName()) + 1;

          char tempStr[32]; // TODO: find a better way to do this!
          sprintf(tempStr, "%d,%d,%d,%d,", thisSensor->_sensorAddress, thisSensor->_muxAddress, thisSensor->_muxPort, configItem);
          configLen += strlen(tempStr);

          SFE_QUAD_Sensor::SFE_QUAD_Sensor_Setting_Type_e type;
          thisSensor->getConfigurationItemType(configItem, &type);
          SFE_QUAD_Sensor::SFE_QUAD_Sensor_Every_Type_t value;
          thisSensor->getConfigurationItem(configItem, &value);
          switch (type)
          {
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_BOOL:
            sprintf(tempStr, "%d", value.BOOL);
            configLen += strlen(tempStr);
            break;
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_FLOAT:
            _sprintf._dtostrf(value.FLOAT, tempStr);
            configLen += strlen(tempStr);
            break;
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_DOUBLE:
            _sprintf._dtostrf(value.DOUBLE, tempStr);
            configLen += strlen(tempStr);
            break;
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_INT:
            sprintf(tempStr, "%d", value.INT);
            configLen += strlen(tempStr);
            break;
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_UINT8_T:
            sprintf(tempStr, "%d", value.UINT8_T);
            configLen += strlen(tempStr);
            break;
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_UINT16_T:
            sprintf(tempStr, "%d", value.UINT16_T);
            configLen += strlen(tempStr);
            break;
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_UINT32_T:
            sprintf(tempStr, "%d", value.UINT32_T);
            configLen += strlen(tempStr);
            break;
          default:
            configLen += 1; // ?
            break;
          }

          configLen += 3; // \r \n NULL

          char *scratchpad = new char[configLen]; // Allocate memory to hold the configuration temporarily
          if (scratchpad == NULL)                 // Did the memory allocation fail?
          {
            if (_printDebug)
              _debugPort->println(F("getSensorAndMenuConfiguration: scratchpad memory allocation failed!"));
            return (false);
          }
          scratchpad[0] = 0;

          strcat(scratchpad, thisSensor->getSensorName());
          strcat(scratchpad, ",");

          sprintf(tempStr, "%d,%d,%d,%d,", thisSensor->_sensorAddress, thisSensor->_muxAddress, thisSensor->_muxPort, configItem);

          strcat(scratchpad, tempStr);

          switch (type)
          {
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_BOOL:
            sprintf(tempStr, "%d", value.BOOL);
            strcat(scratchpad, tempStr);
            break;
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_FLOAT:
            _sprintf._dtostrf(value.FLOAT, tempStr);
            strcat(scratchpad, tempStr);
            break;
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_DOUBLE:
            _sprintf._dtostrf(value.DOUBLE, tempStr);
            strcat(scratchpad, tempStr);
            break;
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_INT:
            sprintf(tempStr, "%d", value.INT);
            strcat(scratchpad, tempStr);
            break;
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_UINT8_T:
            sprintf(tempStr, "%d", value.UINT8_T);
            strcat(scratchpad, tempStr);
            break;
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_UINT16_T:
            sprintf(tempStr, "%d", value.UINT16_T);
            strcat(scratchpad, tempStr);
            break;
          case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_UINT32_T:
            sprintf(tempStr, "%d", value.UINT32_T);
            strcat(scratchpad, tempStr);
            break;
          default:
            strcat(scratchpad, "?");
            break;
          }

          strcat(scratchpad, "\r\n");

          if (_printDebug)
          {
            _debugPort->print(scratchpad);
          }

          // Now concatenate scratchpad onto configuration
          configLen = strlen(configuration);         // Get the current configuration length
          configLen += strlen(scratchpad);           // Get the length of this configuration item
          char *newConfig = new char[configLen + 1]; // Allocate memory to hold configuration plus scratchpad plus a null
          if (newConfig == NULL)                     // Did the memory allocation fail?
          {
            if (_printDebug)
              _debugPort->println(F("getSensorAndMenuConfiguration: newConfig memory allocation failed!"));
            delete[] scratchpad; // Delete the scratchpad
            return (false);
          }
          memset(newConfig, 0, configLen + 1);                     // Clear the memory to make sure it is null-terminated
          memcpy(newConfig, configuration, strlen(configuration)); // Copy in the existing readings
          strcat(newConfig, scratchpad);                           // Append the new configuration item from scratchpad
          delete[] configuration;                                  // Delete configuration
          configuration = newConfig;                               // Make config point to newConfig
          delete[] scratchpad;                                     // Delete the scratchpad
        }
      }

      if (thisSensor->_next == NULL) // Have we reached the end of the sensor list?
        keepGoing = false;
      else
        thisSensor = thisSensor->_next; // Point to the next sensor
    }
  }

  if (theMenu._head == NULL) // If theMenu _head is NULL no menu items have been added
  {
    if (_printDebug)
      _debugPort->println(F("getSensorAndMenuConfiguration: no menu items have been added"));
    return (true); // Return true because some sensors must have been found
  }

  uint16_t numVars = theMenu.getNumMenuVariables();

  if (numVars == 0)
  {
    if (_printDebug)
      _debugPort->println(F("getSensorAndMenuConfiguration: numVars is zero... Error?"));
    return (_head != NULL);
  }

  if (_head == NULL) // If head is NULL no sensors have been found, so delete the old configuration so we can add just the menu items
  {
    if (configuration != NULL)
      delete[] configuration;    // Delete the old configuration
    configuration = new char[1]; // Initialize configuration
    *configuration = 0;
  }

  char *store = new char[theMenu.getMenuVariablesMaxLen()]; // Allocate memory to hold the menu item line
  if (store == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("getSensorAndMenuConfiguration: failed to allocate memory for store"));
    return (_head != NULL);
  }

  for (uint16_t var = 0; var < numVars; var++)
  {
    if (theMenu.getMenuVariableAsCSV(var, store, theMenu.getMenuVariablesMaxLen())) // Get this menu variable as CSV text
    {
      size_t configLen = 0;
      configLen = strlen(configuration);         // Get the current configuration length
      configLen += strlen(store);                // Get the length of the menu variable CSV
      char *newConfig = new char[configLen + 3]; // Allocate memory to hold configuration plus scratchpad plus \r, \n and null
      if (newConfig == NULL)                     // Did the memory allocation fail?
      {
        if (_printDebug)
          _debugPort->println(F("getSensorAndMenuConfiguration: newConfig memory allocation failed!"));
        delete[] store;
        return (false);
      }
      memset(newConfig, 0, configLen + 3);                     // Clear the memory to make sure it is null-terminated
      memcpy(newConfig, configuration, strlen(configuration)); // Copy in the existing readings
      strcat(newConfig, store);                                // Append the menu variable
      strcat(newConfig, "\r\n");                               // Append \r\n
      delete[] configuration;                                  // Delete configuration
      configuration = newConfig;                               // Make config point to newConfig
    }
  }

  delete[] store;

  return (true);
}

bool SFE_QUAD_Sensors::applySensorAndMenuConfiguration(void)
{
  if ((_head == NULL)             // If head is NULL no sensors have been found
      && (theMenu._head == NULL)) // If theMenu _head is NULL no menu items have been added
  {
    if (_printDebug)
      _debugPort->println(F("applySensorAndMenuConfiguration: no sensor or menu items found!"));
    return (false);
  }

  // Go though the configuration, reading a line at a time
  // For each line, check for a matching sensor
  // If a match is found, set the configuration item
  // Also check for a matching menu variable
  // If a match is found, update the variable

  // A health warning about strtok:
  //
  // strtok will convert any delimiters it finds ("\n" in our case) into NULL characters.
  //
  // Also, be very careful that you do not use strtok within an strtok while loop.
  // The next call of strtok(NULL, ...) in the outer loop will use the pointer saved from the inner loop!
  // The solution is to use strtok_r - the reentrant version of strtok
  //
  // Also, if the string does not contain any delimiters, strtok will still return a pointer to the start of the string.
  // The entire string is considered the first token.

  int lineNumber = 0;
  char *preserve;
  char *line = strtok_r(configuration, "\n", &preserve); // Look for something ending in \n

  while (line != NULL)
  {
    char sensorName[64];      // TODO: find a better way to determine the maximum name length
    char configItemValue[64]; // TODO: find a better way to determine the maximum value length
    int sensorAddress, muxAddress, muxPort, configItem;

    int scanNum = sscanf(line, "%[^,],%d,%d,%d,%d,%[^,]\r",
                         sensorName, &sensorAddress, &muxAddress, &muxPort, &configItem, configItemValue);

    if (scanNum == 6) // Were all fields parsed successfully?
    {
      if (_printDebug)
      {
        _debugPort->print(F("applySensorAndMenuConfiguration: configuration line : "));
        _debugPort->print(lineNumber);
        _debugPort->print(F(" : "));
        _debugPort->println(line);
      }

      char *cr = strchr(line, '\r'); // Find the \r
      if (cr != NULL)
      {
        cr++;
        *cr = '\n'; // Replace the \n
      }

      SFE_QUAD_Sensor *thisSensor = sensorExists(sensorName, sensorAddress, muxAddress, muxPort);
      if (thisSensor != NULL) // Check for a match
      {
        // If required, configure the mux port
        if (thisSensor->_muxAddress >= 0x70)
        {
          QWIICMUX *thisMux = new QWIICMUX;
          if (thisMux == NULL)
          {
            if (_printDebug)
              _debugPort->println(F("applySensorAndMenuConfiguration: could not allocate memory for thisMux!"));
            return (false);
          }
          thisMux->begin(thisSensor->_muxAddress, *_i2cPort);
          thisMux->setPort(thisSensor->_muxPort);
          delete thisMux;
        }

        if (configItem == -1) // Is this the logging settings?
        {
          uint8_t senseCount;
          thisSensor->getSenseCount(&senseCount);
          for (uint8_t sense = 0; sense <= senseCount; sense++)
          {
            if (configItemValue[sense] == '0')
              thisSensor->_logSense[sense] = 0;
            else
              thisSensor->_logSense[sense] = 1;
          }
          if (_printDebug)
            _debugPort->println(F("applySensorAndMenuConfiguration: using logging settings"));
        }
        else
        {
          uint8_t configCount;
          bool result = thisSensor->getConfigurationItemCount(&configCount);

          if (result && (configCount > 0))
          {
            for (uint8_t thisConfigItem = 0; thisConfigItem < configCount; thisConfigItem++)
            {
              if (thisConfigItem == configItem)
              {
                if (_printDebug)
                {
                  _debugPort->print(F("applySensorAndMenuConfiguration: using line : "));
                  _debugPort->println(lineNumber);
                }

                SFE_QUAD_Sensor::SFE_QUAD_Sensor_Setting_Type_e type;
                thisSensor->getConfigurationItemType(configItem, &type);
                SFE_QUAD_Sensor::SFE_QUAD_Sensor_Every_Type_t value;
                switch (type)
                {
                case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_BOOL:
                  value.BOOL = (bool)strtoul(configItemValue, NULL, 10);
                  thisSensor->setConfigurationItem(configItem, &value);
                  break;
                case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_FLOAT:
                  value.FLOAT = atof(configItemValue);
                  thisSensor->setConfigurationItem(configItem, &value);
                  break;
                case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_DOUBLE:
                  value.DOUBLE = strtod(configItemValue, NULL);
                  thisSensor->setConfigurationItem(configItem, &value);
                  break;
                case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_INT:
                  value.INT = (int)strtol(configItemValue, NULL, 10);
                  thisSensor->setConfigurationItem(configItem, &value);
                  break;
                case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_UINT8_T:
                  value.UINT8_T = (uint8_t)strtoul(configItemValue, NULL, 10);
                  thisSensor->setConfigurationItem(configItem, &value);
                  break;
                case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_UINT16_T:
                  value.UINT16_T = (uint16_t)strtoul(configItemValue, NULL, 10);
                  thisSensor->setConfigurationItem(configItem, &value);
                  break;
                case SFE_QUAD_Sensor::SFE_QUAD_SETTING_TYPE_UINT32_T:
                  value.UINT32_T = (uint32_t)strtoul(configItemValue, NULL, 10);
                  thisSensor->setConfigurationItem(configItem, &value);
                  break;
                default:
                  break;
                }
              }
            }
          }
        }
      }
      else
      {
        if (_printDebug)
        {
          _debugPort->print(F("applySensorAndMenuConfiguration: no matching sensor for line : "));
          _debugPort->println(lineNumber);
        }
      }
    }
    else
    {
      if (_printDebug)
      {
        _debugPort->print(F("applySensorAndMenuConfiguration: attempting to update menu using line : "));
        _debugPort->print(lineNumber);
        _debugPort->print(F(" : "));
        _debugPort->println(line);
      }

      bool success = theMenu.updateMenuVariableFromCSV(line);

      if (_printDebug)
      {
        _debugPort->print(F("applySensorAndMenuConfiguration: attempt to update menu using line "));
        _debugPort->print(lineNumber);
        _debugPort->print(F(" was"));
        if (!success)
          _debugPort->print(F(" not"));
        _debugPort->println(F(" successful"));
      }
    }

    // Walk through any remaining lines
    line = strtok_r(NULL, "\n", &preserve);
    lineNumber++;
  }

  return (true);
}

// SD Support

#ifdef SFE_QUAD_SENSORS_SD

SFE_QUAD_Sensors__SD::~SFE_QUAD_Sensors__SD(void)
{
  if (_theStorageName != NULL) // Clear the previous file name (if any)
    delete _theStorageName;
}

bool SFE_QUAD_Sensors__SD::beginStorage(int csPin, const char *theFileName)
{
  _csPin = csPin; // Store the chip select pin number

  if (_theStorageName != NULL) // Clear the previous file name (if any)
  {
    delete[] _theStorageName;
    _theStorageName = NULL;
  }

  _theStorageName = new char[strlen(theFileName) + 1]; // Store the file name

  if (_theStorageName == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("SFE_QUAD_Sensors__SD::beginStorage: failed to allocate memory for _theStorageName!"));
    return (false);
  }

  memset(_theStorageName, 0, strlen(theFileName) + 1); // Clear the memory

  strcat(_theStorageName, theFileName);

  if (!SD.begin(_csPin))
  {
    if (_printDebug)
      _debugPort->println(F("SFE_QUAD_Sensors__SD::beginStorage: SD.begin failed!"));
    return (false);
  }

  if (_printDebug)
    _debugPort->println(F("SFE_QUAD_Sensors__SD::beginStorage: success"));

  return (true);
}

bool SFE_QUAD_Sensors__SD::writeConfigurationToStorage(bool append)
{
  if ((_theStorageName == NULL) || (_csPin == -1))
  {
    if (_printDebug)
      _debugPort->println(F("writeConfigurationToStorage: file name or CS pin not found. Did you forget to call beginStorage?"));
    return (false);
  }

#ifndef FILE_APPEND
#define FILE_APPEND O_CREAT | O_WRITE | O_APPEND
#endif

  if (append)
    _theStorage = SD.open(_theStorageName, FILE_APPEND);
  else
    _theStorage = SD.open(_theStorageName, FILE_WRITE);

  if (!_theStorage)
  {
    if (_printDebug)
      _debugPort->println(F("writeConfigurationToStorage: failed to open the file!"));
    return (false);
  }

  _theStorage.print(configuration); // Write the configuration to file

  _theStorage.close();

  if (_printDebug)
    _debugPort->println(F("writeConfigurationToStorage: complete"));

  return (true);
}

bool SFE_QUAD_Sensors__SD::readConfigurationFromStorage(void)
{
  if ((_theStorageName == NULL) || (_csPin == -1))
  {
    if (_printDebug)
      _debugPort->println(F("readConfigurationFromStorage: file name or CS pin not found. Did you forget to call beginStorage?"));
    return (false);
  }

  _theStorage = SD.open(_theStorageName, FILE_READ);

  if (!_theStorage)
  {
    if (_printDebug)
      _debugPort->println(F("readConfigurationFromStorage: failed to open the file!"));
    return (false);
  }

  if (configuration != NULL) // Delete the old configuration
  {
    delete[] configuration;
    configuration = NULL;
  }

  configuration = new char[_theStorage.size() + 1];

  if (configuration == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("readConfigurationFromStorage: failed to allocate memory for configuration!"));
    _theStorage.close();
    return (false);
  }

  memset(configuration, 0, _theStorage.size() + 1); // Clear the memory

  // Go though the file, reading a character at a time and writing it to configuration

  char c[2];
  c[0] = 0;
  c[1] = 0;

  while (_theStorage.available())
  {
    c[0] = _theStorage.read();
    strcat(configuration, c);
  }

  // if (_printDebug)
  // {
  //   _debugPort->println(F("readConfigurationFromStorage: configuration :"));
  //   _debugPort->print(configuration);
  // }

  _theStorage.close();

  if (_printDebug)
    _debugPort->println(F("readConfigurationFromStorage: complete"));

  return (true);
}

bool SFE_QUAD_Sensors__SD::endStorage(void)
{
  return (true);
}

#endif

// SdFat Support

#ifdef SFE_QUAD_SENSORS_SDFAT

SFE_QUAD_Sensors__SdFat::~SFE_QUAD_Sensors__SdFat(void)
{
  if (_theStorageName != NULL) // Clear the previous file name (if any)
    delete _theStorageName;
}

bool SFE_QUAD_Sensors__SdFat::beginStorage(int csPin, const char *theFileName)
{
  _csPin = csPin; // Store the chip select pin number

  if (_theStorageName != NULL) // Clear the previous file name (if any)
  {
    delete[] _theStorageName;
    _theStorageName = NULL;
  }

  _theStorageName = new char[strlen(theFileName) + 1]; // Store the file name

  if (_theStorageName == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("SFE_QUAD_Sensors__SdFat::beginStorage: failed to allocate memory for _theStorageName!"));
    return (false);
  }

  memset(_theStorageName, 0, strlen(theFileName) + 1); // Clear the memory

  strcat(_theStorageName, theFileName);

  if (!sd.begin(SFE_QUAD_SD_CONFIG))
  {
    if (_printDebug)
      _debugPort->println(F("SFE_QUAD_Sensors__SdFat::beginStorage: sd.begin failed!"));
    return (false);
  }

  // Change to root directory. All new file creation will be in root.
  if (sd.chdir() == false)
  {
    if (_printDebug)
      _debugPort->println(F("SFE_QUAD_Sensors__SdFat::beginStorage: sd.chdir failed!"));
    return (false);
  }

  if (_printDebug)
    _debugPort->println(F("SFE_QUAD_Sensors__SdFat::beginStorage: success"));

  return (true);
}

bool SFE_QUAD_Sensors__SdFat::writeConfigurationToStorage(bool append)
{
  if ((_theStorageName == NULL) || (_csPin == -1))
  {
    if (_printDebug)
      _debugPort->println(F("writeConfigurationToStorage: file name or CS pin not found. Did you forget to call beginStorage?"));
    return (false);
  }

  // O_CREAT - create the file if it does not exist
  // O_APPEND - seek to the end of the file prior to each write
  // O_WRITE - open for write

  bool fileOpen;
  if (append)
    fileOpen = _theStorage.open(_theStorageName, O_CREAT | O_APPEND | O_WRITE);
  else
    fileOpen = _theStorage.open(_theStorageName, O_CREAT | O_WRONLY | O_TRUNC);

  if (!fileOpen)
  {
    if (_printDebug)
      _debugPort->println(F("writeConfigurationToStorage: failed to open the file!"));
    return (false);
  }

  _theStorage.print(configuration); // Write the configuration to file

  _theStorage.sync();
  _theStorage.close();

  if (_printDebug)
    _debugPort->println(F("writeConfigurationToStorage: complete"));

  return (true);
}

bool SFE_QUAD_Sensors__SdFat::readConfigurationFromStorage(void)
{
  if ((_theStorageName == NULL) || (_csPin == -1))
  {
    if (_printDebug)
      _debugPort->println(F("readConfigurationFromStorage: file name or CS pin not found. Did you forget to call beginStorage?"));
    return (false);
  }

  bool fileOpen;
  fileOpen = _theStorage.open(_theStorageName, O_READ);

  if (!fileOpen)
  {
    if (_printDebug)
      _debugPort->println(F("readConfigurationFromStorage: failed to open the file!"));
    return (false);
  }

  if (configuration != NULL) // Delete the old configuration
  {
    delete[] configuration;
    configuration = NULL;
  }

  configuration = new char[_theStorage.size() + 1];

  if (configuration == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("readConfigurationFromStorage: failed to allocate memory for configuration!"));
    _theStorage.close();
    return (false);
  }

  memset(configuration, 0, _theStorage.size() + 1); // Clear the memory

  // Go though the file, reading a character at a time and writing it to configuration

  char c[2];
  c[0] = 0;
  c[1] = 0;

  while (_theStorage.available())
  {
    c[0] = _theStorage.read();
    strcat(configuration, c);
  }

  // if (_printDebug)
  // {
  //   _debugPort->println(F("readConfigurationFromStorage: configuration :"));
  //   _debugPort->print(configuration);
  // }

  _theStorage.close();

  if (_printDebug)
    _debugPort->println(F("readConfigurationFromStorage: complete"));

  return (true);
}

bool SFE_QUAD_Sensors__SdFat::endStorage(void)
{
  return (true);
}

#endif

// LittleFS Support

#ifdef SFE_QUAD_SENSORS_LITTLEFS

SFE_QUAD_Sensors__LittleFS::~SFE_QUAD_Sensors__LittleFS(void)
{
  if (_theStorageName != NULL) // Clear the previous file name (if any)
    delete _theStorageName;
}

bool SFE_QUAD_Sensors__LittleFS::beginStorage(const char *theFileName)
{
  if (_theStorageName != NULL) // Clear the previous file name (if any)
  {
    delete[] _theStorageName;
    _theStorageName = NULL;
  }

  _theStorageName = new char[strlen(theFileName) + 1]; // Store the file name

  if (_theStorageName == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("SFE_QUAD_Sensors__LittleFS::beginStorage: failed to allocate memory for _theStorageName!"));
    return (false);
  }

  memset(_theStorageName, 0, strlen(theFileName) + 1); // Clear the memory

  strcat(_theStorageName, theFileName);

  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
  {
    if (_printDebug)
      _debugPort->println(F("SFE_QUAD_Sensors__LittleFS::beginStorage: LittleFS.begin failed!"));
    return (false);
  }

  if (_printDebug)
    _debugPort->println(F("SFE_QUAD_Sensors__LittleFS::beginStorage: success"));

  return (true);
}

bool SFE_QUAD_Sensors__LittleFS::writeConfigurationToStorage(bool append)
{
  if (_theStorageName == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("writeConfigurationToStorage: file name not found. Did you forget to call beginStorage?"));
    return (false);
  }

  if (append)
    _theStorage = LittleFS.open(_theStorageName, FILE_APPEND);
  else
    _theStorage = LittleFS.open(_theStorageName, FILE_WRITE);

  if (!_theStorage)
  {
    if (_printDebug)
      _debugPort->println(F("writeConfigurationToStorage: failed to open the file!"));
    return (false);
  }

  _theStorage.print(configuration); // Write the configuration to file

  _theStorage.close();

  if (_printDebug)
    _debugPort->println(F("writeConfigurationToStorage: complete"));

  return (true);
}

bool SFE_QUAD_Sensors__LittleFS::readConfigurationFromStorage(void)
{
  if (_theStorageName == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("readConfigurationFromStorage: file name not found. Did you forget to call beginStorage?"));
    return (false);
  }

  _theStorage = LittleFS.open(_theStorageName, FILE_READ);

  if (!_theStorage)
  {
    if (_printDebug)
      _debugPort->println(F("readConfigurationFromStorage: failed to open the file!"));
    return (false);
  }

  if (configuration != NULL) // Delete the old configuration
  {
    delete[] configuration;
    configuration = NULL;
  }

  configuration = new char[_theStorage.size() + 1];

  if (configuration == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("readConfigurationFromStorage: failed to allocate memory for configuration!"));
    _theStorage.close();
    return (false);
  }

  memset(configuration, 0, _theStorage.size() + 1); // Clear the memory

  // Go though the file, reading a character at a time and writing it to configuration

  char c[2];
  c[0] = 0;
  c[1] = 0;

  while (_theStorage.available())
  {
    c[0] = _theStorage.read();
    strcat(configuration, c);
  }

  // if (_printDebug)
  // {
  //   _debugPort->println(F("readConfigurationFromStorage: configuration :"));
  //   _debugPort->print(configuration);
  // }

  _theStorage.close();

  if (_printDebug)
    _debugPort->println(F("readConfigurationFromStorage: complete"));

  return (true);
}

bool SFE_QUAD_Sensors__LittleFS::endStorage(void)
{
  LittleFS.end();
  return (true);
}

#endif

// EEPROM Support

// The configuration is stored in EEPROM as follows:
// EEPROM Address 0 is set to 'O'
// EEPROM Address 1 is set to 'L'
// EEPROM Address 2 is set to zero
// The configuration (if any) is stored at EEPROM Address 3 onwards
// EEPROM Address (Configuration Length + 3) is set to zero to NULL-terminate the configuration
// EEPROM Address (Configuration Length + 4) and (Configuration Length + 5) contain two RFC 1145 checksum bytes
// The checksum is calculated on all bytes from Address 0 to Address (Configuration Length + 3)

#ifdef SFE_QUAD_SENSORS_EEPROM

SFE_QUAD_Sensors__EEPROM::~SFE_QUAD_Sensors__EEPROM(void)
{
}

bool SFE_QUAD_Sensors__EEPROM::beginStorage(void)
{
#if defined(ARDUINO_ARCH_ESP32)
  if (!EEPROM.begin(4096)) // Allocate 4kBytes for EEPROM storage
  {
    if (_printDebug)
      _debugPort->println(F("SFE_QUAD_Sensors__EEPROM::beginStorage: EEPROM begin failed!"));
    return (false);
  }
#endif

#if defined(ARDUINO_ARCH_APOLLO3)
  EEPROM.init();
  EEPROM.setLength(2048); // Allocate 2kBytes for EEPROM storage. (4kB appears to cause a crash with Apollo3 v2.2.1)
#endif

  if (_printDebug)
  {
    _debugPort->print(F("SFE_QUAD_Sensors__EEPROM::beginStorage: EEPROM length is "));
    _debugPort->println(EEPROM.length());
  }

  if ((EEPROM.read(0) != 'O') || (EEPROM.read(1) != 'L') || (EEPROM.read(2) != 0) || (!checkStorageCRC()))
  {
    if (_printDebug)
      _debugPort->println(F("SFE_QUAD_Sensors__EEPROM::beginStorage: EEPROM contents are invalid! Formatting..."));
    EEPROM.write(0, 'O');
    EEPROM.write(1, 'L');
    EEPROM.write(2, 0);
    EEPROM.write(3, 0);
    return (writeStorageCRC());
  }

  return (true);
}

bool SFE_QUAD_Sensors__EEPROM::checkStorageCRC(int *crc1address, uint8_t *CRC1, uint8_t *CRC2)
{
  bool keepGoing = true;
  uint8_t crc1 = 0, crc2 = 0;
  bool zero1seen = false, zero2seen = false;
  int address = 0;

  while (keepGoing)
  {
    uint8_t value = EEPROM.read(address); // Read a byte
    address++;                            // Increment the address now
    if (!zero2seen)                       // Should we add this to the CRC?
    {
      crc1 += value;
      crc2 += crc1;
    }
    if (value == 0) // CHeck for the two zeros
    {
      if (!zero1seen)
        zero1seen = true;
      else if (!zero2seen)
        zero2seen = true;
    }
    if (zero2seen) // Have we seen the second zero?
    {
      if (crc1address != NULL) // Set crc1address
        *crc1address = address;
      if (CRC1 != NULL) // Set CRC1
        *CRC1 = crc1;
      if (CRC2 != NULL) // Set CRC2
        *CRC2 = crc2;
      return ((EEPROM.read(address) == crc1) && (EEPROM.read(address + 1))); // Is the checksum valid?
    }
    if ((address == 3) && (!zero1seen)) // Return if the first zero hasn't been seen
      keepGoing = false;
    if (address == EEPROM.length() - 3) // Return if we have reached the end of the EEPROM
      keepGoing = false;
  }
  return (false);
}

bool SFE_QUAD_Sensors__EEPROM::writeStorageCRC(void)
{
  int crc1address;
  uint8_t crc1, crc2;

  checkStorageCRC(&crc1address, &crc1, &crc2); // Calculate what the checksum should be (we don't care if the CRC is not yet valid)
  EEPROM.write(crc1address, crc1);             // Write the checksum bytes
  EEPROM.write(crc1address + 1, crc2);

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_SAMD)
  EEPROM.commit();
#endif

  return (checkStorageCRC()); // Check the checsum was updated correctly
}

bool SFE_QUAD_Sensors__EEPROM::writeConfigurationToStorage(bool append)
{
  if (!checkStorageCRC()) // Bail if the checksum is not valid
  {
    if (_printDebug)
      _debugPort->println(F("writeConfigurationToStorage: EEPROM CRC is invalid!"));
    return (false);
  }

  int address = 3; // If append is false, start writing the configuration at address 3

  if (append) // Set address to the location of the second zero
  {
    while ((EEPROM.read(address) != 0) && (address < (EEPROM.length() - 3)))
      address++;
  }

  // Check there is enough room for the configuration, NULL and CRC bytes
  if (strlen(configuration) >= (EEPROM.length() - (address + 3)))
  {
    if (_printDebug)
      _debugPort->println(F("writeConfigurationToStorage: Not enough memory to store configuration!"));
    return (false);
  }

  // Write the configuration
  for (int len = 0; len < strlen(configuration); len++)
  {
    EEPROM.write(address, configuration[len]);
    address++;
  }

  EEPROM.write(address, 0); // Write the second zero

  bool success = writeStorageCRC(); // Update the CRC

  if (success)
  {
    if (_printDebug)
      _debugPort->println(F("writeConfigurationToStorage: complete"));
  }
  else
  {
    if (_printDebug)
      _debugPort->println(F("writeConfigurationToStorage: failed!"));
  }

  return (success);
}

bool SFE_QUAD_Sensors__EEPROM::readConfigurationFromStorage(void)
{
  int crc1address;

  if (!checkStorageCRC(&crc1address)) // Check the checksum - and return the address of crc1
  {
    if (_printDebug)
      _debugPort->println(F("readConfigurationFromStorage: CRC is invalid!"));
    return (false);
  }

  if (configuration != NULL) // Delete the old configuration
  {
    delete[] configuration;
    configuration = NULL;
  }

  configuration = new char[crc1address - 2];

  if (configuration == NULL)
  {
    if (_printDebug)
      _debugPort->println(F("readConfigurationFromStorage: failed to allocate memory for configuration!"));
    return (false);
  }

  memset(configuration, 0, crc1address - 2); // Clear the memory

  // Go though the EEPROM, reading a character at a time and writing it to configuration
  char c[2];
  c[0] = 0;
  c[1] = 0;

  for (int address = 3; address < crc1address; address++) // This will include the second zero
  {
    c[0] = EEPROM.read(address);
    strcat(configuration, c);
  }

  // if (_printDebug)
  // {
  //   _debugPort->println(F("readConfigurationFromStorage: configuration :"));
  //   _debugPort->print(configuration);
  // }

  if (_printDebug)
    _debugPort->println(F("readConfigurationFromStorage: complete"));

  return (true);
}

bool SFE_QUAD_Sensors__EEPROM::endStorage(void)
{
  return (true);
}

#endif
