#include <Arduino.h>
#include <math.h>

#include "sensory.h"
#include "sensory_sht.h"
#include "ClosedCube_SHT31D.h"

Sensory_SHT::Sensory_SHT() {
  this->Sensory_Class = "SHT";

  this->_sht =  new ClosedCube_SHT31D();
};


Sensory_SHT& Sensory_SHT::_begin() {
  this->_reportEvent("setup");    
  
  
  if((*_sht).begin(0x44) != SHT3XD_NO_ERROR) {
  
    SHT31D_ErrorCode resultSoft = (*_sht).softReset();
    Serial.print("Soft Reset return code: ");
    Serial.println(resultSoft);
  
    SHT31D_ErrorCode resultGeneral = (*_sht).generalCallReset();
    Serial.print("General Call Reset return code: ");
    Serial.println(resultGeneral);
    this->_reportEvent("sht init failed");    
  } else {
    Serial.print("Serial #");
    this->intSerial = (*_sht).readSerialNumber();

    sprintf(this->serial, "%d", this->intSerial);
    Serial.println(this->intSerial);
    Serial.println(this->serial);
    Serial.println(this->intSerial, HEX);

    if ((*_sht).periodicStart(SHT3XD_REPEATABILITY_HIGH, SHT3XD_FREQUENCY_10HZ) != SHT3XD_NO_ERROR) {
      Serial.println("error settingup");
      this->_reportEvent("period mode failure");    
    }
  }
  return *this;
};


Sensory_SHT& Sensory_SHT::_loop(long cycle) {
    float temp(NAN), humidity(NAN);
    
    this->result = (*_sht).periodicFetchData();
    
    temp = this->result.t;
    humidity = this->result.rh;

    Serial.printf("SHT:: %f temp, %f humidity, %d cycle, _lastvalid=%d, _lastConfig=%d\n", temp, humidity, cycle, _lastValid, _lastConfig);
     
    /* Ignore temperatures aquarium of range */
    if(temp > -127.00 && temp < 85.00) {
      if(temp != 0) { _lastValid = cycle; }        
      temp = floor(temp*10000)/10000;
      if(abs( (10000 * (float) temp)-(10000 * (float) _lastValue[0]) ) >= 250) {           
        _lastValue[0] = temp;
        _lastSent[0] = cycle;           
        this->_triggerCallback(Sensory::sensorType::STATE, "temperature", this->serial, "%.4f", temp);
      }
    }  

    if(humidity > 10) {
      _lastValid = cycle;

      humidity = floor(humidity*10000)/10000;        
      if(abs( (10000 * (float) humidity) - (10000 * (float) _lastValue[1]) ) >= 250) {
        _lastValue[1] = humidity;
        _lastSent[1] = cycle;           
        this->_triggerCallback(Sensory::sensorType::STATE, "humidity", this->serial, "%.4f", humidity);
      }       
    }

   return *this;
};


