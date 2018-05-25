
#ifndef SENSORY_DALLAS_H
#define SENSORY_DALLAS_H

#include "sensory.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#define MAX_SENSORS 8

class Sensory_Dallas {
  public:

    Sensory_Dallas(uint8_t pinOneWire);
    Sensory_Dallas(uint8_t pinOneWire, int interval);
    Sensory_Dallas(uint8_t pinOneWire, int interval, UPDATE_CALLBACK_SIGNATURE);
    
    
    Sensory_Dallas& setCallback(UPDATE_CALLBACK_SIGNATURE);
    Sensory_Dallas& setPollInterval(int interval);
    Sensory_Dallas& setPublishInterval(int interval);
    Sensory_Dallas& setupOneWire(uint8_t pinOneWire);
    Sensory_Dallas& setup(UPDATE_CALLBACK_SIGNATURE);
    Sensory_Dallas& setup();
    Sensory_Dallas& loop();
    
    UPDATE_CALLBACK_SIGNATURE;

    int deviceCount = 0;
    
  private:

    Sensory_Dallas& _begin();
    Sensory_Dallas& triggerCallback(enum Sensory::sensorType sType, char * value);
    Sensory_Dallas& triggerCallback(enum Sensory::sensorType sType, DeviceAddress &i, float value);

    Sensory_Dallas& _sendHAconfig();
    
    int _pollInterval = 5;
    int _publishInterval = 30;
     
    OneWire * _ow;
    DallasTemperature * _sensor;    
    
    DeviceAddress _address[MAX_SENSORS]; 
    long _lastSent[MAX_SENSORS] = {};
    float _lastValue[MAX_SENSORS] = {};

    long _lastUpdate = 0;
    long _lastReset = 0;
    long _lastConfig = 0;
    long _polls = 0;
    long _resetFrequency = 24 * 60 * 60 * 1000;
    long _lastValid = 0;
    long _lastMQTT = 0;

    bool _firstRun = true;
};

#endif


