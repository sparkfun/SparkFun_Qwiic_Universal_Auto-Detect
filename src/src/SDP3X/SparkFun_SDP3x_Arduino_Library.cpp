/*
  This is a library written for the Sensirion SDP3x differential pressure sensors
  By Paul Clark @ SparkFun Electronics, January 18th, 2021


  https://github.com/sparkfun/SparkFun_SDP3x_Arduino_Library

  Development environment specifics:
  Arduino IDE 1.8.13

  SparkFun labored with love to create this code. Feel like supporting open
  source hardware? Buy a board from SparkFun!
  https://www.sparkfun.com/products/nnnnn


  CRC lookup table from Bastian Molkenthin  http://www.sunshine2k.de/coding/javascript/crc/crc_js.html

  Copyright (c) 2015 Bastian Molkenthin

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/


#include "SparkFun_SDP3x_Arduino_Library.h"


//Constructor
SDP3X::SDP3X()
{
}

//Start I2C communication using specified port
//Returns true if successful or false if no sensor detected
bool SDP3X::begin(uint8_t address, TwoWire &wirePort)
{
  _SDP3XAddress = address; //Grab which i2c address the user wants us to use
  _i2cPort = &wirePort; //Grab which port the user wants us to use

  uint32_t prodId = readProductId(); //Check which sensor is attached

  if (prodId == SDP3x_product_id_SDP31) //Check if we have an SDP31
  {
    return (true);
  }
  else if (prodId == SDP3x_product_id_SDP32) //Check if we have an SDP32
  {
    return (true);
  }
  else  //Unknown sensor
  {
    return (false);
  }
}

//Calling this function with nothing sets the debug port to Serial
//You can also call it with other streams like Serial1, SerialUSB, etc.
void SDP3X::enableDebugging(Stream &debugPort)
{
	_debugPort = &debugPort;
	_printDebug = true;
}

//Read the 32-bit product identifier
//Returns zero if an error occurred
uint32_t SDP3X::readProductId(void)
{
  _i2cPort->beginTransmission(_SDP3XAddress);
  _i2cPort->write(SDP3x_read_product_id_part1, 2); //Request the product ID - part 1
  uint8_t i2cResult = _i2cPort->endTransmission();

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("readProductId: endTransmission (1) returned: "));
      _debugPort->println(i2cResult);
    }
    return (0); // Bail
  }

  delay(1);

  _i2cPort->beginTransmission(_SDP3XAddress);
  _i2cPort->write(SDP3x_read_product_id_part2, 2); //Request the product ID - part 2
  i2cResult = _i2cPort->endTransmission();

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("readProductId: endTransmission (2) returned: "));
      _debugPort->println(i2cResult);
    }
    return (0); // Bail
  }

  delay(1);

  //Comes back in 18 bytes:
  // Byte1: Product number [31:24]
  // Byte2: Product number [23:16]
  // Byte3: CRC
  // Byte4: Product number [15:8]
  // Byte5: Product number [7:0]
  // Byte6: CRC
  // Byte7: Serial number [63:56]
  // Byte8: Serial number [55:48]
  // Byte9: CRC
  // Byte10: Serial number [47:40]
  // Byte11: Serial number [39:32]
  // Byte12: CRC
  // Byte13: Serial number [31:24]
  // Byte14: Serial number [23:16]
  // Byte15: CRC
  // Byte16: Serial number [15:8]
  // Byte17: Serial number [7:0]
  // Byte18: CRC
  uint8_t toRead = _i2cPort->requestFrom(_SDP3XAddress, (uint8_t)18);
  if (toRead != 18)
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("readProductId: requestFrom returned: "));
      _debugPort->println(toRead);
    }
    return (0); //Error out
  }

  uint32_t prodId = ((uint32_t)_i2cPort->read()) << 24; //store MSB in prodId
  prodId |= ((uint32_t)_i2cPort->read()) << 16;

  uint8_t crc1 = _i2cPort->read();

  prodId |= ((uint32_t)_i2cPort->read()) << 8;
  prodId |= _i2cPort->read(); //store LSB in prodId

  uint8_t crc2 = _i2cPort->read();

  while (_i2cPort->available())
    _i2cPort->read(); //Read and discard the serial number

  if (crc1 != _CRC8((uint16_t)(prodId >> 16))) //verify checksum 1
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("readProductId: checksum 1 failed! Expected: 0x"));
      _debugPort->print(_CRC8((uint16_t) prodId >> 16), HEX);
      _debugPort->print(F(" Received: 0x"));
      _debugPort->println(crc1, HEX);
    }
    return (0); //checksum failed
  }

  if (crc2 != _CRC8((uint16_t)(prodId & 0xFFFF))) //verify checksum 2
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("readProductId: checksum 2 failed! Expected: 0x"));
      _debugPort->print(_CRC8((uint16_t) prodId & 0xFFFF), HEX);
      _debugPort->print(F(" Received: 0x"));
      _debugPort->println(crc2, HEX);
    }
    return (0); //checksum failed
  }

  return (prodId); //Return the product Id
}

// Perform a soft reset
// Note: this is performed using a general call to I2C address 0x00 followed by command code 0x06
// softReset can be called before .begin if required
// If the sensor has been begun (_i2cPort is not NUll) then _i2cPort is used
// If the sensor has not been begun (_i2cPort is NUll) then wirePort is used (which will default to Wire)
SDP3XERR SDP3X::softReset(TwoWire &wirePort)
{
  uint8_t i2cResult;
  if (_i2cPort != NULL) //If the sensor has been begun (_i2cPort is not NUll) then _i2cPort is used
  {
    _i2cPort->beginTransmission(0x00);
    _i2cPort->write(0x06); //Perform a soft reset
    i2cResult = _i2cPort->endTransmission();
  }
  else
  {
    //If the sensor has not been begun (_i2cPort is NUll) then wirePort is used (which will default to Wire)
    wirePort.beginTransmission(0x00);
    wirePort.write(0x06); //Perform a soft reset
    i2cResult = wirePort.endTransmission();
  }

  delay(20); //From Sensirion Sample Code adp3x.c Sdp3x_SoftReset

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("softReset: endTransmission returned: "));
      _debugPort->println(i2cResult);
    }
    return SDP3X_ERR_I2C_ERROR;
  }

  return SDP3X_SUCCESS;
}

//Enter sleep mode
//Returns SUCCESS (0) if successful
SDP3XERR SDP3X::enterSleepMode(void)
{
  _i2cPort->beginTransmission(_SDP3XAddress);
  _i2cPort->write(SDP3x_enter_sleep_mode, 2); //Enter sleep mode
  uint8_t i2cResult = _i2cPort->endTransmission();

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("enterSleepMode: endTransmission returned: "));
      _debugPort->println(i2cResult);
    }
    return SDP3X_ERR_I2C_ERROR;
  }

  return SDP3X_SUCCESS;
}

//Start continuous measurement
//Returns SUCCESS (0) if successful
SDP3XERR SDP3X::startContinuousMeasurement(boolean massFlow, boolean averaging)
{
  _i2cPort->beginTransmission(_SDP3XAddress);

  if (massFlow && averaging)
    _i2cPort->write(SDP3x_measure_continuous_mass_flow_average_till_read, 2);
  else if (massFlow && !averaging)
    _i2cPort->write(SDP3x_measure_continuous_mass_flow_no_averaging, 2);
  else if (!massFlow && averaging)
    _i2cPort->write(SDP3x_measure_continuous_differential_pressure_average_till_read, 2);
  else //if (!massFlow && !averaging)
    _i2cPort->write(SDP3x_measure_continuous_differential_pressure_no_averaging, 2);

  uint8_t i2cResult = _i2cPort->endTransmission();

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("startContinuousMeasurement: endTransmission returned: "));
      _debugPort->println(i2cResult);
    }
    return SDP3X_ERR_I2C_ERROR;
  }

  return SDP3X_SUCCESS;
}

// Stop continuous measurement
// Returns SUCCESS (0) if successful
// stopContinuousMeasurement can be called before .begin if required
// If the sensor has been begun (_i2cPort is not NULL) then _i2cPort and _SDP3XAddress are used
// If the sensor has not been begun (_i2cPort is NUll) then wirePort and address are used (which will default to Wire)
SDP3XERR SDP3X::stopContinuousMeasurement(uint8_t address, TwoWire &wirePort)
{
  uint8_t i2cResult;
  if (_i2cPort != NULL) // If the sensor has been begun (_i2cPort is not NUll) then _i2cPort and _SDP3XAddress are used
  {
    _i2cPort->beginTransmission(_SDP3XAddress);
    _i2cPort->write(SDP3x_stop_continuous_measure, 2);
    i2cResult = _i2cPort->endTransmission();
  }
  else
  {
    // If the sensor has not been begun (_i2cPort is NUll) then wirePort and address are used (which will default to Wire)
    wirePort.beginTransmission(address);
    wirePort.write(SDP3x_stop_continuous_measure, 2);
    i2cResult = wirePort.endTransmission();
  }

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("stopContinuousMeasurement: endTransmission returned: "));
      _debugPort->println(i2cResult);
    }
    return SDP3X_ERR_I2C_ERROR;
  }

  delay(1); // Datasheet says sensor will be receptive for another command after 500us

  return SDP3X_SUCCESS;
}

//Triggered measurement
//Returns SUCCESS (0) if successful
SDP3XERR SDP3X::triggeredMeasurement(boolean massFlow, boolean clockStretching)
{
  _i2cPort->beginTransmission(_SDP3XAddress);

  if (massFlow && clockStretching)
    _i2cPort->write(SDP3x_measure_triggered_mass_flow_clock_stretching, 2);
  else if (massFlow && !clockStretching)
    _i2cPort->write(SDP3x_measure_triggered_mass_flow_no_clock_stretching, 2);
  else if (!massFlow && clockStretching)
    _i2cPort->write(SDP3x_measure_triggered_differential_pressure_clock_stretching, 2);
  else //if (!massFlow && !clockStretching)
    _i2cPort->write(SDP3x_measure_triggered_differential_pressure_no_clock_stretching, 2);

  uint8_t i2cResult = _i2cPort->endTransmission();

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("triggeredMeasurement: endTransmission returned: "));
      _debugPort->println(i2cResult);
    }
    return SDP3X_ERR_I2C_ERROR;
  }

  return SDP3X_SUCCESS;
}

//Read the mesurement
//Returns SUCCESS (0) if successful
SDP3XERR SDP3X::readMeasurement(float *pressure, float *temperature)
{
  //Data is 9 bytes:
  // Byte1: Differential Pressure 8msb
  // Byte2: Differential Pressure 8lsb
  // Byte3: CRC
  // Byte4: Temperature 8msb
  // Byte5: Temperature 8lsb
  // Byte6: CRC
  // Byte7: Scale Factor differential pressure 8msb
  // Byte8: Scale Factor differential pressure 8lsb
  // Byte9: CRC
  uint8_t toRead = _i2cPort->requestFrom(_SDP3XAddress, (uint8_t)9);
  if (toRead != 9)
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("readMeasurement: requestFrom returned: "));
      _debugPort->println(toRead);
    }
    return (SDP3X_ERR_I2C_ERROR); //Error out
  }

  // Avoid any confusion when casting unsigned data to signed
  union
  {
    uint16_t _unsigned;
    int16_t _signed;
  } _unsignedSigned;

  _unsignedSigned._unsigned = ((uint16_t)_i2cPort->read()) << 8; //store MSB
  _unsignedSigned._unsigned |= (uint16_t)_i2cPort->read(); //store LSB
  int16_t diffPress = _unsignedSigned._signed;

  uint8_t crc1 = _i2cPort->read();

  _unsignedSigned._unsigned = ((uint16_t)_i2cPort->read()) << 8; //store MSB
  _unsignedSigned._unsigned |= (uint16_t)_i2cPort->read(); //store LSB
  int16_t temp = _unsignedSigned._signed;

  uint8_t crc2 = _i2cPort->read();

  uint16_t scaleFactor = ((uint16_t)_i2cPort->read()) << 8; //store MSB
  scaleFactor |= (uint16_t)_i2cPort->read(); //store LSB

  uint8_t crc3 = _i2cPort->read();

  if (crc1 != _CRC8signed(diffPress)) //verify checksum 1
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("readMeasurement: checksum 1 failed! Expected: 0x"));
      _debugPort->print(_CRC8(diffPress), HEX);
      _debugPort->print(F(" Received: 0x"));
      _debugPort->println(crc1, HEX);
    }
    return (SDP3X_ERR_BAD_CRC); //checksum failed
  }

  if (crc2 != _CRC8signed(temp)) //verify checksum 2
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("readMeasurement: checksum 2 failed! Expected: 0x"));
      _debugPort->print(_CRC8(temp), HEX);
      _debugPort->print(F(" Received: 0x"));
      _debugPort->println(crc2, HEX);
    }
    return (SDP3X_ERR_BAD_CRC); //checksum failed
  }

  if (crc3 != _CRC8(scaleFactor)) //verify checksum 3
  {
    if (_printDebug == true)
    {
      _debugPort->print(F("readMeasurement: checksum 3 failed! Expected: 0x"));
      _debugPort->print(_CRC8(scaleFactor), HEX);
      _debugPort->print(F(" Received: 0x"));
      _debugPort->println(crc3, HEX);
    }
    return (SDP3X_ERR_BAD_CRC); //checksum failed
  }

  //Convert pressure to float and apply the scale factor
  float differential = (float)diffPress;
  float scale = (float)scaleFactor; // Should be: 60 for SDP31; 240 for SDP32

  if (_printDebug == true)
  {
    _debugPort->print(F("readMeasurement: scale factor is "));
    _debugPort->println(scale);
  }

  *pressure = differential / scale;

  //Convert temp to float and convert to degrees C
  *temperature = ((float)temp) / 200.0;

  return (SDP3X_SUCCESS); //Success!
}

// CRC helper function for signed data
uint8_t SDP3X::_CRC8signed(int16_t data)
{
  union
  {
    uint16_t _unsigned;
    int16_t _signed;
  } _unsignedSigned;

  _unsignedSigned._signed = data;
  return (_CRC8(_unsignedSigned._unsigned));
}

#ifndef SDP3X_LOOKUP_TABLE
//Given an array and a number of bytes, this calculate CRC8 for those bytes
//CRC is only calc'd on the data portion (two bytes) of the four bytes being sent
//From: http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
//Tested with: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
//x^8+x^5+x^4+1 = 0x31
uint8_t SDP3X::_CRC8(uint16_t data)
{
  uint8_t crc = 0xFF; //Init with 0xFF

  crc ^= (data >> 8); // XOR-in the first input byte

  for (uint8_t i = 0 ; i < 8 ; i++)
  {
    if ((crc & 0x80) != 0)
      crc = (uint8_t)((crc << 1) ^ 0x31);
    else
      crc <<= 1;
  }

  crc ^= (uint8_t)data; // XOR-in the last input byte

  for (uint8_t i = 0 ; i < 8 ; i++)
  {
    if ((crc & 0x80) != 0)
      crc = (uint8_t)((crc << 1) ^ 0x31);
    else
      crc <<= 1;
  }

  return crc; //No output reflection
}
#else
//Generates CRC8 for SDP3X from lookup table
uint8_t SDP3X::_CRC8(uint16_t data)
{
  uint8_t crc8 = 0xFF; //inital value
  crc8 ^= (uint8_t)(data >> 8); //start with MSB
  crc8 = _CRC8LookupTable[crc8 >> 4][crc8 & 0xF]; //look up table [MSnibble][LSnibble]
  crc8 ^= (uint8_t)data; //use LSB
  crc8 = _CRC8LookupTable[crc8 >> 4][crc8 & 0xF]; //look up table [MSnibble][LSnibble]
  return crc8;
}
#endif
