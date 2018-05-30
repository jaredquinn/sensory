#ifndef Sensory_SHT_H
#define Sensory_SHT_H

#include "sensory.h"
#include "ClosedCube_SHT31D.h"

class Sensory_SHT : public Sensory_Base {

  public:
    
    Sensory_SHT();
    
  protected:  
    
    Sensory_SHT&   _loop(long cycle);
    Sensory_SHT&   _begin();

  private:
    ClosedCube_SHT31D * _sht;
    SHT31D result;
    
    uint32_t intSerial;
    char serial[20];
    
    long _lastSent[2] = {};
    float _lastValue[2] = {};
    
};

#endif


