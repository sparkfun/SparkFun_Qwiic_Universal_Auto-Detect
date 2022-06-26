# API Reference for the SFE_QUAD_Sensors__SdFat class

Methods to support writing/reading the sensors and menu configuration to/from storage using SdFat.

## Methods

### beginStorage()

This method: records the configuration file name; initializes the **sd** object; changes directory to the SD root directory.

```c++
bool beginStorage(int csPin, const char *theFileName)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `csPin` | `int` | The SPI Chip Select pin |
| `theFilename` | `const char *` | A pointer to the storage filename |
| return value | `bool` | ```true``` is successful, otherwise ```false``` |

### writeConfigurationToStorage()

This method writes the menu and sensor configuration from the ```configuration``` dynamic char array into the configuration file.

If ```append``` is ```false``` (default) the configuration file will be overwritten.

If ```append``` is ```true```, the configuration is appended to the end of the file, preserving any existing configuration data.
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

This method reads the menu and sensor configuration from storage, copying it into the ```configuration``` dynamic char array.

```c++
bool readConfigurationFromStorage(void)
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | ```true``` if the data is read successfully, otherwise ```false``` |

### endStorage()

This method would be used to call the storage's ```end``` method - if it had one. For ```SdFat```, it does nothing and simply returns ```true```.

``` c++
bool endStorage(void);
```

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| return value | `bool` | Always ```true``` |

## Member Variables

| Parameter | Type | Description |
| :-------- | :--- | :---------- |
| `sd` | `SdFs` | An instance of the SdFat file system object, used for SD data transfer |
| `_theStorage` | `FsFile` | A SdFat File object, used to hold the sensor and menu configuration |
| `_theStorageName` | `char *` | A dynamic char array which holds the configuration file name |
