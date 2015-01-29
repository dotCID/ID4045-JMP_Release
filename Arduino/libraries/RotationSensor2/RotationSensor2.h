/* Rotation sensor library for ID4045 JMP
   Speed is estimated by means of a second order backwards finite difference equation
   @author Marien Wolthuis
   @author Doga Emirdag
   Written 21/01/2015
   */
   
#ifndef RotationSensor2_h
#define RotationSensor2_h

#include <Arduino.h>

class RotationSensor
{
public:
	RotationSensor(int pin);
	float read();
	
	float lastReading();
	float currentReading();
	void setDirection(int direction);
	void setLocation(float location);
	float getLocation();
	
private:
	int _pin;
	float _readings[3];
	int _t_0, _t_1, _t_2; // tracks last write index
	unsigned long _readTimes[3];
	int _direction;
	float _speed;
	float _multiplier;
};

#endif
