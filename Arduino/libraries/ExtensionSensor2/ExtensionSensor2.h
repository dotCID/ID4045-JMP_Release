/* Extension sensor library for ID4045 JMP
   Speed is estimated by means of a second order backwards finite difference equation
   @author Marien Wolthuis
   @author Doga Emirdag
   Written 21/01/2015
   */
   
#ifndef ExtensionSensor2_h
#define ExtensionSensor2_h

#include <Arduino.h>

class ExtensionSensor
{
public:
	ExtensionSensor();
	float read();
	
	unsigned long lastReading();
	unsigned long currentReading();
	void setDirection(int direction);
	void setLocation(float location);
	float getLocation();
	
	int discarded;
	
private:
	unsigned long _readings[3];
	int _t_0, _t_1, _t_2; // tracks last write index
	int _direction;
	float _step;
	float _location;
	float _speed;
	
};

#endif
