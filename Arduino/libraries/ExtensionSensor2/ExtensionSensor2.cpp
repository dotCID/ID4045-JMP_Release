/* Extension sensor library for ID4045 JMP
   Speed is estimated by means of a second order backwards finite difference equation
   @author Marien Wolthuis
   @author Doga Emirdag
   Written 21/01/2015
   */

#include <Arduino.h>
#include <ExtensionSensor2.h>

#define LOWER_THRESHOLD 3
#define DEBOUNCE_TIME 25

ExtensionSensor::ExtensionSensor(){
	_step = 9; // 9mm
}

float ExtensionSensor::read(){
	unsigned long tmp = millis();
	if(tmp - _readings[_t_0] < DEBOUNCE_TIME) {discarded++; return _speed; };// _t_0 here is the last _t_0, not the current t
	
	// Shift indices rather than data to be able to execute quicker
	_t_0 = _t_0==2?0:_t_0+1;		// reading(t)
	_t_1 = _t_0==0?2:_t_0-1;		// reading(t-1)
	_t_2 = _t_0==1?2:_t_0==0?1:0;	// reading(t-2)
	
	_readings[_t_0] = tmp;
	
	_location += _step * _direction;
	
	_speed = (2*_step/1000)/(_readings[_t_2] - 4*_readings[_t_1] - 3*_readings[_t_0]);
	return _speed;
}
	
unsigned long ExtensionSensor::lastReading(){
	return _readings[_t_1];
}

unsigned long ExtensionSensor::currentReading(){
	return _readings[_t_0];
}

void ExtensionSensor::setLocation(float location){
	_location = location;
}

float ExtensionSensor::getLocation(){
	return _location;
}

void ExtensionSensor::setDirection(int direction){
	if(direction == -1 || direction == 1)
		_direction = direction;
}

