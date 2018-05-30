
#include <BME280I2C.h>

#include "sensory.h"
#include "sensory_bme.h"
#include <math.h>

Sensory_BME::Sensory_BME() {
  this->Sensory_Class = "BME";
  this->_bme = new BME280I2C();
  this->tempUnit = BME280::TempUnit::TempUnit_Celsius;
  this->presUnit = BME280::PresUnit::PresUnit_hPa;
};

Sensory_BME& Sensory_BME::_begin() {
  if((*_bme).begin()) {
    this->_online = true;
  }
  return *this;
};


Sensory_BME& Sensory_BME::_loop(long cycle) {

  float temp(NAN), hum(NAN), pres(NAN);
 
  (*_bme).read(pres, temp, hum, tempUnit, presUnit);

  /* Ignore temperatures aquarium of range */
  if(temp > -127.00 && temp < 85.00) {
    _lastValid = cycle;        
    temp = floor(temp*10000)/10000;
    if(abs( (10000 * (float) temp)-(10000 * (float) _lastValue[0]) ) >= 100) {           
      _lastValue[0] = temp;
      _lastSent[0] = cycle;           
      this->_triggerCallback(Sensory::sensorType::STATE, "temperature", "bme", "%.2f", temp);
    }
  }  

  if(pres > 0) {
    _lastValid = cycle;        
    pres = floor(pres*10000)/10000;        
    if(abs( (10000 * (float) pres) - (10000 * (float) _lastValue[1]) ) >= 100) {
      _lastValue[1] = pres;
      _lastSent[1] = cycle;           
      this->_triggerCallback(Sensory::sensorType::STATE, "pressure", "bme", "%.4f", pres);
    }
  }

  return *this;

};




