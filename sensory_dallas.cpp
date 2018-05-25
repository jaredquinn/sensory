
#include <DallasTemperature.h>
#include <OneWire.h>

#include "sensory.h"
#include "sensory_dallas.h"

Sensory_Dallas::Sensory_Dallas(uint8_t pinOneWire) {
  Serial.println("Dallas Standard Constructor");
  this->setupOneWire(pinOneWire);
};

Sensory_Dallas::Sensory_Dallas(uint8_t pinOneWire, int interval) {
  Serial.println("Dallas Interval Constructor");
  this->setupOneWire(pinOneWire);
  this->setPollInterval(interval);
};

Sensory_Dallas::Sensory_Dallas(uint8_t pinOneWire, int interval, UPDATE_CALLBACK_SIGNATURE) {
  Serial.println("Dallas Interval Callback Constructor");
  this->setupOneWire(pinOneWire);
  this->setPollInterval(interval);
  this->setCallback(callback);
};

Sensory_Dallas& Sensory_Dallas::setPollInterval(int interval) {
  this->_pollInterval = interval;
  return *this;
}

Sensory_Dallas& Sensory_Dallas::setPublishInterval(int interval) {
  this->_publishInterval = interval;
  return *this;
}

Sensory_Dallas& Sensory_Dallas::setupOneWire(uint8_t pinOneWire) {
  this->_ow = new OneWire(pinOneWire);
  this->_sensor = new DallasTemperature(_ow);
  return *this;
};

Sensory_Dallas& Sensory_Dallas::setCallback(UPDATE_CALLBACK_SIGNATURE) {
  this->callback = callback;
  return *this;
};

Sensory_Dallas& Sensory_Dallas::setup(UPDATE_CALLBACK_SIGNATURE) {
  this->setCallback(callback);
  return this->_begin();
};

Sensory_Dallas& Sensory_Dallas::setup() {
  return this->_begin();
};

Sensory_Dallas& Sensory_Dallas::_begin() {
  (*_sensor).begin();

  (*_sensor).setWaitForConversion(false);
  (*_sensor).requestTemperatures();

  _firstRun = true;
  return *this;
};

Sensory_Dallas& Sensory_Dallas::triggerCallback(enum Sensory::sensorType sType, char * value) {
  char topic[8];
  sprintf(topic, "event");
  callback(sType, topic, value);
  return *this;
};

Sensory_Dallas& Sensory_Dallas::triggerCallback(enum Sensory::sensorType sType, DeviceAddress &i, float value) {
  char topic[100];
  char tmpStr[100];

  sprintf(topic, "temperature_%x", i);
  sprintf(tmpStr, "%f", value);
  callback(sType, topic, tmpStr);
  return *this;
};

Sensory_Dallas& Sensory_Dallas::_sendHAconfig() {
  for (unsigned i = 0; i < deviceCount; i++) {
    (*_sensor).getAddress(_address[i], i);;
    Serial.printf("Getting information for device %d: %x\n", i, _address[i]);
    (*_sensor).setResolution(_address[i], 12);
    _lastValue[i] = 0;
    _lastSent[i] = 0;
  }
};



/*
*/
Sensory_Dallas& Sensory_Dallas::loop() {

  long   cycle = millis() / 1000;
  float  temp = 0;

  if (_firstRun || cycle - _lastValid > 60) {
    Serial.printf("Doing an Init. %d.  Cycle=%d _lastValid=%d\n", _firstRun, cycle, _lastValid);
    this->triggerCallback(Sensory::sensorType::EVENT, "onewire-reset");
    _begin();
    _firstRun = false;
    _lastReset = cycle;
    _lastValid = cycle;

    _sendHAconfig();
    _lastConfig = cycle;
  }

  /* If it's been more 1.5 seconds since the last bus reset
      then we can check the temperatures.
  */

  if (cycle - _lastUpdate > _pollInterval) {
    /* If we haven't requested temperatures, do that now,
        we don't want to delay and lock the CPU, so we'll
        get the values later.
    */
    _polls++;

    /* Read the values, we do this on alternating calls every second.
    */
    deviceCount = (*_sensor).getDeviceCount();

    Serial.printf("Running DS18B20 Main Loop.  deviceCount=%d, cycle=%d, _lastUpdate=%d, _pollInterval=%d, polls=%d\n", deviceCount, cycle, _lastUpdate, _pollInterval, _polls);

    for (unsigned i = 0; i < deviceCount; i++) {
      temp = (*_sensor).getTempC(_address[i]);

      /* Ignore temperatures aquarium of range */
      if (temp > -127.00 && temp < 85.00) {
        /* calculate the difference from the last reading */

        _lastValid = cycle;
        float diff = (10000 * (float) temp) - (10000 * (float) _lastValue[i]);

        Serial.printf("Sensor %x read value of %f (diff:%f)\n"  , _address[i], temp, diff);

        if ((abs(diff) > 500) || cycle - _lastSent[i] > _publishInterval) {
          _lastValue[i] = temp;
          _lastSent[i] = cycle;

          this->triggerCallback(Sensory::sensorType::STATE, _address[i], temp);
        }
      }
    }

    (*_sensor).requestTemperatures();

    _lastUpdate = cycle;
  }

  return *this;
};


