/* Arduino Code for JMP, version 1.0
   Motor controller class
   @author Marien Wolthuis
   date created 20/1/2015			*/
 
#include <Arduino.h>
#include <ExtensionMotor.h>


static int	_fwd[3] = {1,0,0};
static int	_bck[3] = {0,1,0};
static int	_stop[3] ={0,0,0};
	

ExtensionMotor::ExtensionMotor(int fwd, int rev, int dis){
	_pins[0] = fwd;
	_pins[1] = rev;
	_pins[2] = dis;
	
	
	ExtensionMotor::stop();
}

bool ExtensionMotor::run(int direction){
	_direction = direction==1?1:-1;
	if(_direction == 1){
		for(int i=0;i<2;i++)
			digitalWrite(_pins[i],_fwd[i]);
		analogWrite(_pins[2],_speed);
	}else{
		for(int i=0;i<2;i++)
			digitalWrite(_pins[i],_bck[i]);
		analogWrite(_pins[2],_speed);
	}
	
	return true; 
}

bool ExtensionMotor::stop(){
	for(int i=0;i<3;i++)
		digitalWrite(_pins[i],_stop[i]);
	
	return true;
}

float ExtensionMotor::getSpeed(){
	return (_speed / 255);
}

void ExtensionMotor::setSpeed(float speed){
	_speed = (int) (speed * 255);
}

int ExtensionMotor::getDirection(){
	return _direction;
}

bool ExtensionMotor::isActive(){
	return _active;
}

