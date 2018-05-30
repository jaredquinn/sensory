# Sensory: Asynchronous Multi Sensor Library 

## Currently Supported Sensors

* Dallas DS18B20 OneWire Temperature
* Borsh Temperature, Humidty and Pressure Sensors (BMP/BME 280)
* SHT3x Temperature, Humidity

## Background

These libraries came about due to the excessive overuse of the delay()
functionality in almost all online examples. They wrap around the existing
Arduino libraries and in the case of DS18B20 several workarounds to ensure
reliability.




