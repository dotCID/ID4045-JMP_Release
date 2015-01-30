/* Arduino Code for JMP, version 1.0
   Motor controller class
   @author Marien Wolthuis
   date created 16/1/2015			*/
   
#ifndef RotationMotor2_h
#define RotationMotor2_h

#include <Arduino.h>

class RotationMotor
{
public:
	RotationMotor(int pin1, int pin2);
	bool run(int direction);
	bool stop();
	
	float getSpeed();
	
	int getDirection();
	
	bool isActive();
	
private:
	int _pins[2];
	int _direction;
	bool _active;
};

#endif
