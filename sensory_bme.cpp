
#include <BME280I2C.h>
#include <Wire.h>

#include "sensory.h"
#include "sensory_bme.h"
#include <math.h>

Sensory_BME::Sensory_BME() {
  this->setupWire();
};

Sensory_BME::Sensory_BME(int interval) {
  this->setupWire();
  this->setInterval(interval);
};

Sensory_BME::Sensory_BME(int interval, UPDATE_CALLBACK_SIGNATURE) {
  this->setupWire();
  this->setInterval(interval);
  this->setCallback(callback);
};

Sensory_BME& Sensory_BME::setInterval(int interval) {
  this->_interval = interval;
  return *this;
}

Sensory_BME& Sensory_BME::setupWire() {
  this->_bme = new BME280I2C();
  this->tempUnit = BME280::TempUnit::TempUnit_Celsius;
  this->presUnit = BME280::PresUnit::PresUnit_hPa;

  return *this;  
};

Sensory_BME& Sensory_BME::setCallback(UPDATE_CALLBACK_SIGNATURE) {
  this->callback = callback;
  return *this;
};

Sensory_BME& Sensory_BME::reportType() {
  char result[100];
  Sensory::sensorType tp = Sensory::sensorType::EVENT;
  
  switch ((*_bme).chipModel()) {
        case BME280::ChipModel_BME280:  sprintf(result, "Found BME280"); break;
        case BME280::ChipModel_BMP280:  sprintf(result, "Found BMP280!H"); break;
        default:                        sprintf(result, "No BM Device"); break;
  }
  this->triggerCallback(tp, result);
  return *this;
};

Sensory_BME& Sensory_BME::setup(UPDATE_CALLBACK_SIGNATURE) {
  this->setCallback(callback);
  return this->_begin();
};

Sensory_BME& Sensory_BME::setup() {
  return this->_begin();
};

Sensory_BME& Sensory_BME::_begin() {
  Serial.println("Begin BME");
  if((*_bme).begin()) {
    Serial.println("Success");
    this->_online = true;
    this->reportType();   
  }
  return *this;
};

Sensory_BME& Sensory_BME::runCallback(enum Sensory::sensorType sType, char * _topic, char * _value ) {
  char topic[100], tmpStr[100];
  enum Sensory::sensorType ty = sType;
  sprintf(topic, "%s", _topic);
  sprintf(tmpStr, "%s", _value);
  callback(ty, topic, tmpStr);  
  return *this;
};

Sensory_BME& Sensory_BME::triggerCallback(enum Sensory::sensorType sType, char * value) {
  return runCallback(sType, "event", value);
};

Sensory_BME& Sensory_BME::triggerCallback(enum Sensory::sensorType sType, char * unit, char * format, float value) {
  char topic[100];
  char tmpStr[100];
  sprintf(topic, "%s_bme", unit);
  sprintf(tmpStr, format, value);
  return runCallback(sType, topic, tmpStr);
};

/* 
 */
Sensory_BME& Sensory_BME::loop() {

  float temp(NAN), hum(NAN), pres(NAN);
 

  /* Reset the OneWire bus every 120 seconds */
  if(_online == false && millis() - _lastValid > 10000 && millis() - _lastReset > 10000) {
    
    this->triggerCallback(Sensory::sensorType::EVENT, "i2c-rescan");
    Wire.begin();
    if((*_bme).begin()) {
      this->_online = true;
      this->reportType();
    }
    
    _lastReset = millis();
  }

  /* If it's been more 1.5 seconds since the last bus reset
   *  then we can check the temperatures.
   */
  if(millis() - _lastReset > 1200 && millis() - _lastUpdate > _interval) {
    /* If we haven't requested temperatures, do that now,
     *  we don't want to delay and lock the CPU, so we'll 
     *  get the values later.
     */
    _polls++;
    
      /* Read the values, we do this on alternating calls every second.
       */

      (*_bme).read(pres, temp, hum, tempUnit, presUnit);

      //Serial.println(analogRead(A0), DEC);
      //Serial.printf("%f %f\n", pres, temp);
      
      /* Ignore temperatures aquarium of range */
      if(temp > -127.00 && temp < 85.00) {
        _lastValid = millis();        
        temp = floor(temp*10000)/10000;
        if(abs( (10000 * (float) temp)-(10000 * (float) _lastValue[0]) ) >= 100) {           
          _lastValue[0] = temp;
          _lastSent[0] = millis();           
          this->triggerCallback(Sensory::sensorType::STATE, "temperature", "%.2f", temp);
        }
      }  

      if(pres > 0) {
        _lastValid = millis();        
        pres = floor(pres*10000)/10000;        
        if(abs( (10000 * (float) pres) - (10000 * (float) _lastValue[1]) ) >= 100) {
          _lastValue[1] = pres;
          _lastSent[1] = millis();           
          this->triggerCallback(Sensory::sensorType::STATE, "pressure", "%.4f", pres);
        }
        
      }

      _lastUpdate = millis();
   }

   return *this;
};


