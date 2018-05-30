
/* old style */
#ifndef SENSORY_DALLAS_H
#define SENSORY_DALLAS_H

#include "sensory.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#define MAX_SENSORS 8


class Sensory_Dallas : public Sensory_Base {
  public:
    Sensory_Dallas(uint8_t pinOneWire);

  protected:
    
    Sensory_Dallas& _begin();
    Sensory_Dallas& _loop(long cycle);
    Sensory_Dallas& _reconfigure();


  private:
    int deviceCount = 0;

    Sensory_Dallas& setupOneWire(uint8_t pinOneWire);
    Sensory_Dallas& addressToChar(char  dstChar[18], DeviceAddress &addrv);

    OneWire * _ow;
    DallasTemperature * _sensor;

    DeviceAddress _address[MAX_SENSORS];

    long _lastSent[MAX_SENSORS] = {};
    float _lastValue[MAX_SENSORS] = {};

};

#endif


