/* Arduino Code for JMP, version 1.0
   Motor controller class
   @author Marien Wolthuis
   date created 20/1/2015			*/

#include <Arduino.h>
#include <RotationMotor2.h>

#define FORWARD 1
#define BACKWARD -1

static int _fwd[2]  = {1,0};
static int _bck[2]  = {0,1};
static int _stop[2] = {0,0};

RotationMotor::RotationMotor(int pin1, int pin2){
	_pins[0] = pin1;
	_pins[1] = pin2;
	
	pinMode(_pins[0], OUTPUT);
	pinMode(_pins[1], OUTPUT);
	
	RotationMotor::stop();
}

bool RotationMotor::run(int direction){
	_direction = direction;
	if(direction == FORWARD){
		for(int i=0;i<2;i++){
			digitalWrite(_pins[i], _fwd[i]); // speed control not possible with this circuit
		}
		return true;
	}else if(direction == BACKWARD){
		for(int i=0;i<2;i++){
			digitalWrite(_pins[i], _bck[i]);
		}
		return true;
	}
	
	return false;
}

bool RotationMotor::stop(){
	for(int i=0;i<2;i++)
		digitalWrite(_pins[i],_stop[i]);
	return true;
}

int RotationMotor::getDirection(){
	return _direction;
}

bool RotationMotor::isActive(){
	return _active;
}

