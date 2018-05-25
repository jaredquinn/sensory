
#ifndef SENSORY_H
#define SENSORY_H


#ifdef ESP8266
#include <functional>
#define UPDATE_CALLBACK_SIGNATURE std::function<void(enum Sensory::sensorType sType, char*, char*)> callback
#else
#define UPDATE_CALLBACK_SIGNATURE void (*callback)(enum Sensory::sensorType sType, char*, char*)
#endif

class Sensory {
  
  public:
    enum sensorType { STATE, EVENT };

  private:
  
};

#endif

