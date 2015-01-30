/* Rotation sensor library for ID4045 JMP
   Speed is estimated by means of a second order backwards finite difference equation
   @author Marien Wolthuis
   @author Doga Emirdag
   Written 21/01/2015
   */
   
#include <Arduino.h>
#include <RotationSensor2.h>


RotationSensor::RotationSensor(int pin){
	_multiplier = 90.0 / -947.0 * M_PI / 180.0; // conversion value from degrees/resolution -> radians/measured_value
	_pin = pin;
}

float RotationSensor::read(){
	// shift indices rather than data to lower computation time
	_t_0 = _t_0==2?0:_t_0+1;		// reading(t)
	_t_1 = _t_0==0?2:_t_0-1;		// reading(t-1)
	_t_2 = _t_0==1?2:_t_0==0?1:0;	// reading(t-2)

	_readings[_t_0] = (analogRead(_pin)-1023) * _multiplier; // rad
	_readTimes[_t_0] = millis();
	
	_speed = (_readings[_t_2] - 4*_readings[_t_1] + 3*_readings[_t_0])/(2 * (_readTimes[_t_0] - _readTimes[_t_2]) / 2);
	
	return _speed;
}
	
float RotationSensor::lastReading(){
	return _readings[_t_1] / _multiplier;
}

float RotationSensor::currentReading(){
	return _readings[_t_0] / _multiplier;
}

void RotationSensor::setDirection(int direction){
	_direction = direction;
}

void RotationSensor::setLocation(float location){
	_readings[_t_0] = location;
}

float RotationSensor::getLocation(){
	return _readings[_t_0];
}
