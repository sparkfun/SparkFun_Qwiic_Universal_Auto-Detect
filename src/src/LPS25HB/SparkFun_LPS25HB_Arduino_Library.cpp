/*

SparkFun_LPS25HB_Arduino_Library.cpp

Header file: SparkFun_LPS25HB_Arduino_Library.h

Created: May 2018
Last Updated: June 2018

Authors:
Owen Lyke

*/

#include "SparkFun_LPS25HB_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_LPS25HB

/*
		Note: You will see a repeated pattern of setting 'lastCode = CODE_X' 
		and then returning a boolean value. This allows the functions easy to 
		test against (for example 'if(isConnected == true)') and also gives 
		advanced users more detailed debugging information by checking 
		'pressureSensorX.lastCode' for a status report
*/

/**
   * Constructor for an object of the LPS25HB class
*/
LPS25HB::LPS25HB(void)
{
}

/**
   * Allows a user to specify which Wire (I2C) port, sensor I2C address, and I2C speed before beginning I2C
   * The desired Wire port and I2C address are then associated with the object until modified by the user
   * The clock frequency is applied but can be changed again externally
   * @return Boolean, true if connected and false if disconnected
*/
bool LPS25HB::begin(TwoWire &wirePort, uint8_t address)
{
	sensor_address = address; // Associate the specified I2C address with the object for later use

	_i2cPort = &wirePort; // This line keeps the 'address' of the Wire port to use associated with the object for later. See a tutorial on pointers for more information
	//_i2cPort->begin();					 // The '->' notation is like accessing the member of an object with '.' except used with pointers
	//_i2cPort->setClock(clock_frequency); // So here we called X.begin() and X.setClock() where X is the wire port the user specified. For example Wire.begin() and Wire.setClock()

	if (isConnected() != true) // Make sure the device is connected
	{
		return false;
	}

	uint8_t values[5]; // Now we can set up default values that make the sensor work

	values[0] = LPS25HB_RES_CONF_DEFAULT; // Set the resolution configuration to default
	write(LPS25HB_REG_RES_CONF, values, 1);
	// There is a break here because the RES_CONF register is not contiguous with the other registers
	values[0] = LPS25HB_CTRL_REG1_PD_ACTIVE | LPS25HB_CTRL_REG1_ODR_25HZ; // Turn the sensor ON and set output rate to 25 Hz
	values[1] = LPS25HB_CTRL_REG2_DEFAULT;								  // Default
	values[2] = LPS25HB_CTRL_REG3_INT_L | LPS25HB_CTRL_REG3_OD;			  // Set interrupts to output LOW and Open Drain function
	values[3] = LPS25HB_CTRL_REG4_DEFAULT;								  // Default
	values[4] = LPS25HB_INTERRUPT_CFG_DEFFAULT;							  // Set the Interrupt CFG register to default
	write(LPS25HB_REG_CTRL_REG1, values, 5);							  // Write the 5 values consecutively to the device using only one write function

	return true; // Signal that everything is A-OK!
}

/**
   * Determines if a LPS25HB device is available at the address associated with the object 
   * @return Boolean, true if connected and false if disconnected
*/
bool LPS25HB::isConnected()
{
	_i2cPort->beginTransmission(sensor_address); // Starting an I2C transmission will allow us to check for acknowledgement by any device at 'sensor_address'
	if (_i2cPort->endTransmission() == 0)		 // If the device does not acknowledge then endTransmission() will return 1
	{
		if (getID() == LPS25HB_DEVID) // If there was a response then we can make sure the device is in fact a LPS25HB by checking the ID
		{
			lastCode = LPS25HB_CODE_CONNECTED;
			return true;
		}

		lastCode = LPS25_HB_CODE_WRONG_ID;
		return false;
	}

	lastCode = LPS25HB_CODE_NO_DEV_AT_ADDRESS;
	return false;
}

/**
   * Gets the device ID from the WHO_AM_I register. The return should be equal to 0xBD for valid LPS25HB devices
   * @return Device ID
*/
uint8_t LPS25HB::getID()
{
	uint8_t retval = 0x00;
	read(LPS25HB_REG_WHO_AM_I, &retval, 1); // Use the read function to read the WHO_AM_I register into the 'retval' variable
	return retval;							// Then return the 'retval' variable
}

/**
   * Gets the status code from the STATUS_REG register of the device
   * @return Status code
*/
uint8_t LPS25HB::getStatus()
{
	uint8_t retval = 0x00;
	read(LPS25HB_REG_STATUS_REG, &retval, 1); // Use the read function to read the STATUS_REG register into the 'retval' variable
	return retval;							  // Then return the 'retval' variable
}

/**
   * Gets the 16 bit temperature reading in ADC counts
   * @return Temperature reading
*/
int16_t LPS25HB::getTemperature_raw()
{
	uint8_t data[2];					   // Allocate 2 bytes to hold the 16 bit data
	read(LPS25HB_REG_TEMP_OUT_L, data, 2); // Use the read function to fill out both those bytes from first the TEMP_OUT_L and then the TEMP_OUT_H registers
	int16_t retval = 0x00;				   // Allocate a signed 16 bit variable to hold the result
	retval = (data[1] << 8 | data[0]);	 // This combines the two bytes using bit shifting and bitwise logical operations into the return value
	return retval;
}

/**
   * Gets the temperature reading in deg C
   * @return Temperature reading
*/
float LPS25HB::getTemperature_degC()
{
	int16_t raw = getTemperature_raw();   // Get the 16 bit value from the sensor
	return (float)(42.5 + (raw / 480.0)); // Then divide by 480, add 42.5, and cast to a float to get the result in deg C.
										  /*
	NOTE! 	The LPS25HB datasheet does not specify the 42.5 deg C offset,
			however the LPS25H datasheet does and it seems to work for 
			this device as well. Thanks to the Pololu LPS25H library for
			illuminating this problem.
	*/
}

/**
   * Gets the 24 bit pressure reading in ADC counts
   * @return Pressure reading
*/
int32_t LPS25HB::getPressure_raw()
{
	uint8_t data[3];																	   // Allocate 3 bytes for the 24 bit data
	read(LPS25HB_REG_PRESS_OUT_XL, data, 3);											   // Read all 3 bytes in order beginning with the extra low byte
	int32_t retval = 0x00;																   // Allocate a really big number to return
	retval = ((int32_t)data[0] << 0) | ((int32_t)data[1] << 8) | ((int32_t)data[2] << 16); // Use bitshifting and bitwise logical operators to combine the three bytes into the uint32_t
	if (data[2] & 0x80)
	{
		retval |= 0xFF000000;
	} // For proper 2's complement behavior we need to conditionally set the highest byte of the return value
	return retval;
}

/**
   * Gets the pressure reading in hPa
   * @return Pressure reading
*/
float LPS25HB::getPressure_hPa()
{
	int32_t raw = getPressure_raw(); // Get the 24 bit value from the sensor
	return (float)(raw / 4096.0);	// Divide by 4096 and cast to a float to get the result in hPa
}

/**
   * Sets a threshold number of ADC counts that is used to trigger interrupts and flags on temperature
   * @param avg_code A byte that specifies the number of averages. Suggest using defined values in .h file
   * @see Defined setting values in .h file
   * @return Boolean, true if successful and false if unsuccessful
*/
bool LPS25HB::setReferencePressure(uint32_t adc_val)
{
	uint8_t data[3];							 // Allocate a buffer for 3 bytes
												 // Use bitshifting and binary masks to get the XL, L, and H parts of the desired value
	data[0] = ((adc_val & 0x0000FF) >> 0);		 // XL 	(Extra low byte)
	data[1] = ((adc_val & 0x00FF00) >> 8);		 // L 	(Low byte)
	data[2] = ((adc_val & 0xFF0000) >> 16);		 // H 	(High byte)
	return write(LPS25HB_REG_REF_P_XL, data, 3); // Write the data to the device beginning witht the LSB (least significant byte)
}

/**
   * Sets a threshold number of ADC counts that is used to trigger interrupts and flags on pressure
   * @param avg_code A byte that specifies the number of averages. Suggest using defined values in .h file
   * @see Defined setting values in .h file
   * @return Boolean, true if successful and false if unsuccessful
*/
bool LPS25HB::setPressureThreshold(uint16_t adc_val)
{
	uint8_t data[2];					 // Allocate a buffer for 2 bytes
										 // Use bitshifting and binary masks to get the L and H parts of the desired value
	data[0] = ((adc_val & 0x00FF) >> 0); // L 	(Low byte)
	data[1] = ((adc_val & 0xFF00) >> 8); // H	(High byte)
	return write(LPS25HB_REG_THS_P_L, data, 2);
}

/**
   * This sets the resolution of temperature sensor by changing the number of
   * readings that are averaged for each result update
   * @param avg_code A byte that specifies the number of averages. Suggest using defined values in .h file
   * @see Defined setting values in .h file
   * @return Boolean, true if successful and false if unsuccessful
*/
bool LPS25HB::setTemperatureAverages(uint8_t avg_code)
{
	return applySetting(LPS25HB_REG_RES_CONF, (avg_code & 0x0C));
}

/**
   * This sets the resolution of pressure sensor by changing the number of
   * readings that are averaged for each result update
   * @param avg_code A byte that specifies the number of averages. Suggest using defined values in .h file
   * @see Defined setting values in .h file
   * @return Boolean, true if successful and false if unsuccessful
*/
bool LPS25HB::setPressureAverages(uint8_t avg_code)
{
	return applySetting(LPS25HB_REG_RES_CONF, (avg_code & 0x03));
}

/**
   * Sets the sensor's output data rate according to datasheet option. 
   * Note that this is the refresh rate of the values in internal registers 
   * because as a slave I2C device it may not push data out on its own.
   * @param odr_code A byte that specifies the output data rate. Suggest using defined values in .h file
   * @see Defined setting values in .h file
   * @return Boolean, true if successful and false if unsuccessful
*/
bool LPS25HB::setOutputDataRate(uint8_t odr_code)
{
	return applySetting(LPS25HB_REG_CTRL_REG1, (odr_code & 0x70));
}

/**
   * Sets the FIFO mode according to the datasheet options
   * @param mode_code A byte that specifies the FIFO mode. Suggest using defined values in .h file
   * @see Defined setting values in .h file
   * @return Boolean, true if successful and false if unsuccessful
*/
bool LPS25HB::setFIFOMode(uint8_t mode_code)
{
	if (mode_code == LPS25HB_FIFO_CTRL_BYPASS) // If we want to bypass the FIFO then we should disable the FIFO
	{
		// Using 'removeSetting()' along with FIFO_EN will 'reset' (means set to 0) the FIFO enable bit
		if (removeSetting(LPS25HB_REG_CTRL_REG2, LPS25HB_CTRL_REG2_FIFO_EN) != true)
		{
			return false;
		}
	}
	else
	{
		// Using the 'applySetting()' function with FIFO_EN will 'set' (means set to 1) the FIFO enable bit
		if (applySetting(LPS25HB_REG_CTRL_REG2, LPS25HB_CTRL_REG2_FIFO_EN) != true)
		{
			return false;
		}
	}

	// This sets the new mode (and the bitmasking helps protect the other settings in the FIFO_CTRL register)
	if (applySetting(LPS25HB_REG_FIFO_CTRL, (mode_code & 0xE0)) != LPS25HB_CODE_NOM)
	{
		return false;
	}

	return true; // Signal that everything is A-OK!
}

/**
   * Sets the number of hardware averages performed by the FIFO buffer to one of several allowed values
   * @param num_code A byte that specifies how many hardware averags to use according to the datasheet. Suggest using defined register options in .h file
   * @see Defined setting values in .h file
   * @return Boolean, true if successful and false if unsuccessful
*/
bool LPS25HB::setFIFOMeanNum(uint8_t num_code)
{
	return applySetting(LPS25HB_REG_FIFO_CTRL, (num_code & 0x1F));
}

/**
   * A member that allows setting individual bits within a given register of the device
   * @param reg_adr The address of the register to modify. Use of the LPS25HB_RegistersTypeDef enumeration values is suggested.
   * @param setting A byte wide bitmask indicating which bits to set. Use of the defined setting values in the .h file is suggested
   * @see LPS25HB_RegistersTypeDef
   * @see Defined setting values in .h file
   * @return Boolean, true if successful and false if unsuccessful
*/
bool LPS25HB::applySetting(uint8_t reg_adr, uint8_t setting)
{
	uint8_t data;									  // Declare space for the data
	read(reg_adr, &data, 1);						  // Now fill that space with the old setting from the sensor
	data |= setting;								  // OR in the new setting, preserving the other fields
	if (write(reg_adr, &data, 1) != LPS25HB_CODE_NOM) // Write the new data back to the device and make sure it was successful
	{
		lastCode = LPS25HB_CODE_SET_FAIL;
		return false; // If it failed then return an error
	}
	lastCode = LPS25HB_CODE_NOM;
	return true; // Otherwise its all good!
}

/**
   * A member that allows resetting individual bits within a given register of the device
   * @param reg_adr The address of the register to modify. Use of the LPS25HB_RegistersTypeDef enumeration values is suggested.
   * @param setting A byte wide bitmask indicating which bits to reset. Use of the defined setting values in the .h file is suggested
   * @see LPS25HB_RegistersTypeDef
   * @see Defined setting values in .h file
   * @return Boolean, true if read was successful and false if unsuccessful
*/
bool LPS25HB::removeSetting(uint8_t reg_adr, uint8_t setting)
{
	uint8_t data;									  // Declare space for the data
	read(reg_adr, &data, 1);						  // Now fill that space with the old setting from the sensor
	data &= ~setting;								  // AND in the opposite of the setting value to reset the desired bit(s) while leaving others intact
	if (write(reg_adr, &data, 1) != LPS25HB_CODE_NOM) // Write the new data back to the device and make sure it was successful
	{
		lastCode = LPS25HB_CODE_RESET_FAIL;
		return false; // If it failed then return an error
	}
	lastCode = LPS25HB_CODE_NOM;
	return true; // Otherwise its all good!
}

/**
   * A member that allows reading a sequence of data from consecutive register locations in the device
   * @param reg_adr The address of the first register to read from. Use of the LPS25HB_RegistersTypeDef enumeration values is suggested.
   * @param pdata A pointer to the first element of the buffer that will hold the returned data
   * @param size The number of elements to read. I.e. to read only one byte to the device use size=1
   * @see LPS25HB_RegistersTypeDef
   * @return Boolean, true if read was successful and false if unsuccessful
*/
bool LPS25HB::read(uint8_t reg_adr, uint8_t *pdata, uint8_t size)
{
	_i2cPort->beginTransmission(sensor_address); // Begin talking to the desired sensor
	_i2cPort->write(reg_adr | (1 << 7));		 // Setting the 7th bit (the MSb) in this tells the device that we want a 'multi-byte' read
	// _i2cPort->write(reg_adr);													// A multi-byte bit is not needed in this command
	// AHA! It turns out that you DO need a multi-byte bit set in the first command here... who'da thunk it

	if (_i2cPort->endTransmission(false) != 0) // Send a restart command. Do not release bus.
	{
		lastCode = LPS25HB_CODE_ERR; // Sensor did not ACK
		return false;
	}

	_i2cPort->requestFrom(sensor_address, size); // Default value of true used here to release bus once complete
	for (uint8_t indi = 0; indi < size; indi++)  //
	{
		if (_i2cPort->available())
		{
			*(pdata + indi) = _i2cPort->read(); // Use pointers to place the read data into the buffer that was specified by the user
		}
		else
		{
			lastCode = LPS25HB_CODE_RX_UNDERFLOW; // For some reason we did not get as much data as we expected!
			return false;
		}
	}

	lastCode = LPS25HB_CODE_NOM;
	return true;
}

/**
   * A member that allows writing a sequence of data to consecutive register locations in the device
   * @param reg_adr The address of the first register to write to. Use of the LPS25HB_RegistersTypeDef enumeration values is suggested.
   * @param pdata A pointer to the first element of the buffer that contains the data to be written
   * @param size The number of elements to write. I.e. to write only one byte to the device use size=1
   * @see LPS25HB_RegistersTypeDef
   * @return Boolean, true if write was successful and false if unsuccessful
*/
bool LPS25HB::write(uint8_t reg_adr, uint8_t *pdata, uint8_t size)
{
	_i2cPort->beginTransmission(sensor_address); // Begin talking to the desired sensor
	_i2cPort->write(reg_adr | (1 << 7));		 // Specify a write to the desried register with the mutli-byte bit set so that consecutive writes will go to consecutive registers within the device
	_i2cPort->write(pdata, size);				 // Write all the bytes!

	if (_i2cPort->endTransmission() == 0)
	{
		lastCode = LPS25HB_CODE_NOM;
		return true;
	}

	return false; // If the sensor did not ACK then endTransmission would have exited with a nonzero return, and that would make us sad
}