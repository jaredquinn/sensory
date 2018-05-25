
#ifndef SENSORY_BME_H
#define SENSORY_BME_H

#include <BME280.h>
#include <BME280I2C.h>
#include <Wire.h>

#include "sensory.h"


class Sensory_BME {
  public:

    Sensory_BME();
    Sensory_BME(int interval);
    Sensory_BME(int interval, UPDATE_CALLBACK_SIGNATURE);
    
    Sensory_BME& setupWire();
    Sensory_BME& setCallback(UPDATE_CALLBACK_SIGNATURE);
    Sensory_BME& setInterval(int interval);
    Sensory_BME& setup();
    Sensory_BME& setup(UPDATE_CALLBACK_SIGNATURE);
    Sensory_BME& loop();
    
    UPDATE_CALLBACK_SIGNATURE;

    BME280::TempUnit tempUnit;
    BME280::PresUnit presUnit;

  
  private:

    BME280I2C * _bme;   

    Sensory_BME&   _begin();
    Sensory_BME&   reportType();

    Sensory_BME& runCallback(enum Sensory::sensorType sType, char * _topic, char * _value );
    Sensory_BME& triggerCallback(enum Sensory::sensorType sType, char * value);
    Sensory_BME& triggerCallback(enum Sensory::sensorType sType, char * type, char * format, float value);

    int _interval = 5000;
    bool _online = false;
    
    long _lastSent[2] = {};
    float _lastValue[2] = {};

    long _lastUpdate = 0;
    long _lastReset = 0;
    
    long _polls = 0;
    long _resetFrequency = 10000;
    long _lastValid = 0;
    
};

#endif


