# API Reference for the SFE_QUAD_Sensors__EEPROM class

Methods to support writing/reading the sensors and menu configuration to/from EEPROM storage.

## Methods

### beginStorage()

This method: initializes the EEPROM; if required it also initializes the CRC-protected configuration structure.

```c++
bool beginStorage(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```true``` is successful, otherwise ```false``` |

### writeConfigurationToStorage()

This method writes the menu and sensor configuration from the ```configuration``` dynamic char array into EEPROM. The CRC is also updated.

If ```append``` is ```false``` (default) the configuration will be overwritten.

If ```append``` is ```true```, the configuration is appended to the end of any existing configuration data.
This can be useful if different sensors are connected for different logging runs. If a sensor configuration is included twice, both will be applied
with the newest configuration superseding the older.

```c++
bool writeConfigurationToStorage(bool append)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `append` | `bool` | If ```false``` (default) new data overwrites old. If ```true```, new data is apended to old  |
| return value | `bool` | ```true``` if the data is written successfully, otherwise ```false``` |

### readConfigurationFromStorage()

This method reads the menu and sensor configuration from EEPROM, copying it into the ```configuration``` dynamic char array.

```c++
bool readConfigurationFromStorage(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```true``` if the data is read successfully, otherwise ```false``` |

### endStorage()

This method would be used to call the storage's ```end``` method - if it had one. For EEPROM, it does nothing and simply returns ```true```.

``` c++
bool endStorage(void);
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | Always ```true``` |
