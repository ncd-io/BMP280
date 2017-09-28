# Usage

This library provides a class for reading temperature and pressure from the BMP280. The bmp280.ino file provides a simple use case for the Particle cloud. You can copy all files from the `src` directory into Particle build, or clone this library into Particle Dev.

>Create a new instance of the device
```cpp
BMP280 Sensor;
```

>Set the address of the device, there is only 1 address pin and this method is not necessary if you aren't using the jumper, send a 1 if the jumper is installed

>***init() must be called after this is set***
```cpp
Sensor.setAddress(1);
```

***The following properties are explained in the [datasheet](http://media.ncd.io/sites/2/20170721134749/BMP280_I2C_.pdf)***

>Set the oversampling rates, stand by time, and filter

>***init() must be called after these are set***
```cpp
//Set the temperature oversampling rate
Sensor.osrs_t = BMP280_OST_1;

//Set the pressure oversampling rate
Sensor.osrs_p = BMP280_OSP_1;

//Set the standby time for the sensor
Sensor.sb_time = BMP280_TSB_62_5;

//Set the filter for the sensor
Sensor.filter = BMP280_FILTER_0;
```

>Initialize the device, the argument is optional, and will set all outputs to off.
```cpp
Sensor.init();
```

>Set the temperate scale to use
```cpp
Sensor.scale = TEMP_SCALE_FAHRENHEIT;
```

>Get the readings from the device as frequently as possible. This method calculates the time between reads based on current device configuration, and calls `takeReadings()` when new readings should be available.
```cpp
Sensor.loop();
```

>Take readings from the device. It is preferable to use the `loop()` method instead of this, as it handles the delay required between reads based on current device configuration.
```cpp
Sensor.takeReadings();
```

>Check the current real world values. These are updated anytime `takeReadings()` is called.
```cpp
double temp = Sensor.temperature;
double press = Sensor.pressure;
```
