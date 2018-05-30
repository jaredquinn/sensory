
/* Old style */
#include <DallasTemperature.h>
#include <OneWire.h>

#include "sensory.h"
#include "sensory_dallas.h"

Sensory_Dallas::Sensory_Dallas(uint8_t pinOneWire) {
  this->Sensory_Class = "Dallas";

  this->_ow = new OneWire(pinOneWire);
  this->_sensor = new DallasTemperature(this->_ow);
};

Sensory_Dallas& Sensory_Dallas::_begin() {
  (*_sensor).begin();
  (*_sensor).setWaitForConversion(false);
  (*_sensor).requestTemperatures();
  return *this;
};


Sensory_Dallas& Sensory_Dallas::_reconfigure() {
  char addressHolder[18];

  this->deviceCount = (*_sensor).getDeviceCount();
  for (unsigned i = 0; i < deviceCount; i++) {
    (*_sensor).getAddress(_address[i], i);;
    
    addressToChar(addressHolder, _address[i]);

    Serial.printf("Getting information for device %d: %s\n", i, addressHolder);
    (*_sensor).setResolution(_address[i], 12);
    _lastValue[i] = 0;
    _lastSent[i] = 0;
  }
};


Sensory_Dallas& Sensory_Dallas::addressToChar(char dstChar[18], DeviceAddress &addr) {
   static char *hex = "0123456789ABCDEF";
   uint8_t i, j;
   for (i=0, j=0; i<8; i++) 
   {
       dstChar[j++] = hex[addr[i] / 16];
       dstChar[j++] = hex[addr[i] & 15];
   }
   dstChar[j] = '\0';
   return *this;
}


/*
*/
Sensory_Dallas& Sensory_Dallas::_loop(long cycle) {
  float  temp = 0;
  char   addressHolder[17];
  
  Serial.printf("Running DS18B20 Main Loop.  deviceCount=%d, cycle=%d, _lastUpdate=%d, _pollInterval=%d, polls=%d\n", deviceCount, cycle, _lastUpdate, _pollInterval, _polls);

  for (unsigned i = 0; i < this->deviceCount; i++) {
    addressToChar(addressHolder, _address[i]);
    temp = (*_sensor).getTempC(_address[i]);

    /* Ignore temperatures aquarium of range */
    if (temp > -127.00 && temp < 85.00) {
      /* calculate the difference from the last reading */

      _lastValid = cycle;
      float diff = (10000 * (float) temp) - (10000 * (float) _lastValue[i]);

      Serial.printf("Sensor %s read value of %f (diff:%f)\n"  , addressHolder, temp, diff);

      if ((abs(diff) > 500) || cycle - _lastSent[i] > _publishInterval) {
        _lastValue[i] = temp;
        _lastSent[i] = cycle;
        this->_triggerCallback(Sensory::sensorType::STATE, "temperature", addressHolder, "%.4f", temp);
      }
    }
  }
  (*_sensor).requestTemperatures();
  return *this;
};


