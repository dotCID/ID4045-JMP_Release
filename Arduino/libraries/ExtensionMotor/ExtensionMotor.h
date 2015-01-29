/* Arduino Code for JMP, version 0.1
   Motor controller class
   @author Marien Wolthuis
   date created 20/1/2015			*/
   
#ifndef ExtensionMotor_h
#define ExtensionMotor_h

#include <Arduino.h>

class ExtensionMotor
{
public:
	ExtensionMotor(int fwd, int rev, int dis);
	bool run(int direction);
	bool stop();
	
	float getSpeed();
	void setSpeed(float speed);
	
	int getDirection();
	
	bool isActive();
	
private:
	int _pins[3];
	int _speed;
	int _direction;
	bool _active;
};

#endif
