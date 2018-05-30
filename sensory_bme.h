#ifndef SENSORY_BME_H
#define SENSORY_BME_H

#include <BME280.h>
#include <BME280I2C.h>

#include "sensory.h"

class Sensory_BME : public Sensory_Base {

  public:
    
    Sensory_BME();
    
  protected:

    Sensory_BME& _begin();
    Sensory_BME& _loop(long cycle);

  private:
  
    BME280I2C * _bme;   

    BME280::TempUnit tempUnit;
    BME280::PresUnit presUnit;

    long _lastSent[3] = {};
    float _lastValue[3] = {};

    
};

#endif


