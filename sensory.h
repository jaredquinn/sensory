#ifndef SENSORY_H
#define SENSORY_H

class Sensory {
  public:
    enum sensorType { STATE, EVENT };
};


#ifdef ESP8266
#include <functional>
#define UPDATE_CALLBACK_SIGNATURE std::function<void(enum Sensory::sensorType sType, char*, char*)> callback
#else
#define UPDATE_CALLBACK_SIGNATURE void (*callback)(enum Sensory::sensorType sType, char*, char*)
#endif

class Sensory_Base {

  public:

      inline Sensory_Base & setPublishInterval(int interval) { this->_publishInterval = interval; return *this; };
      inline Sensory_Base & setPollInterval(int interval) { this->_pollInterval = interval; return *this; };
      inline Sensory_Base & setCallback(UPDATE_CALLBACK_SIGNATURE) { this->callback = callback; return *this; };

      inline Sensory_Base & setup(UPDATE_CALLBACK_SIGNATURE) {
        this->setCallback(callback);
        this->_setup();
        return *this;
      };

     inline Sensory_Base & setup() {
        this->_setup();
        return *this;
     };

     inline Sensory_Base & loop() {

        long   cycle = millis() / 1000;

        /* We don't want to do anything for the first 5 seconds after our initial Initialization */
        if(cycle < 5 || cycle - _lastReset < 5) {
          return *this;
        }

        /* Reset the OneWire bus every 120 seconds */
        if(cycle - _lastValid > _resetInvalid) {    
          Serial.printf("%s: reset needed()\n", this->Sensory_Class.c_str());
          this->_reportEvent("triggering-reset");    
          this->_reset();  
          _lastReset = cycle;
          _lastConfig = cycle - _resetInvalid - 5;
          _lastValid = cycle;
        }        

        if (cycle - _lastConfig > _resetInvalid) {
            _reconfigure();
            _lastConfig = cycle;
            return *this;
        }

      /* If it's been more 1.5 seconds since the last bus reset
       *  then we can check the temperatures.
       */
      if(cycle - _lastUpdate > _pollInterval) {
        Serial.printf("%s: loop()\n", this->Sensory_Class.c_str());
        /* If we haven't requested temperatures, do that now,
         *  we don't want to delay and lock the CPU, so we'll 
         *  get the values later.
         */
        this->_polls++;
        this->_loop(cycle);
        this->_lastUpdate = cycle;        
      }
          
      return *this;
   };

  protected:

    
    virtual inline Sensory_Base & _begin() { Serial.printf("%s: _BEGIN Parent\n", this->Sensory_Class.c_str()); return *this; };
    virtual inline Sensory_Base & _loop(long cycle) { Serial.printf("%s: _LOOP Parent - this shouldnt happen\n", this->Sensory_Class.c_str()); return *this; };
    virtual inline Sensory_Base & _reconfigure() { Serial.printf("%s: _RECONFIGURE - this shouldnt happen\n", this->Sensory_Class.c_str()); return *this; };
           
    String Sensory_Class = "Base";
    
     inline Sensory_Base & _setup() {
        this->_online = true;
        this->_begin();
        return *this;
     };
      
      inline Sensory_Base & _triggerCallback(enum Sensory::sensorType sType, char * unit, char * identifier, char * format, float value) {
        char topic[100];
        char tmpStr[100];

        this->_lastValid = millis();
        sprintf(topic, "%s_%s", unit, identifier);
        sprintf(tmpStr, format, value);
        this->_runCallback(sType, topic, tmpStr);
        return *this;
      };
 
      inline Sensory_Base & _runCallback(enum Sensory::sensorType sType, char * _topic, char * _value ) {
        char topic[100], tmpStr[100];
        enum Sensory::sensorType ty = sType;
        sprintf(topic, "%s", _topic);
        sprintf(tmpStr, "%s", _value);
        this->callback(ty, topic, tmpStr);  
        return *this;
     };

      inline Sensory_Base & _reportEvent(char * value){
        this->_runCallback(Sensory::sensorType::EVENT, "event", value);
        return *this;
      };

      virtual inline Sensory_Base & _reset() { 
        Serial.printf("%s: _reset\n", this->Sensory_Class.c_str());
        this->_setup();
        return *this;
      };

      String Sensory = "";
    
      UPDATE_CALLBACK_SIGNATURE;
  
      int _pollInterval = 2;
      int _publishInterval = 5;

      long _resetInvalid = 30;

      bool _online = false;
      
      long _polls = 0;

      long _lastUpdate = 0;
      long _lastReset = 0;
      long _lastValid = 0;
      long _lastConfig = 0;

    
};

#endif

