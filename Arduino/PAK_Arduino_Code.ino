/* Complete control class for the Arduino
   Can work with the remote control GUI
   @author Marien Wolthuis
   date created 27/1/2015			*/

#include <RotationMotor2.h>
#include <RotationSensor2.h>
#include <ExtensionMotor.h>
#include <ExtensionSensor2.h>
#include <Math.h>

#define ARM_LENGTH 600

#define UP_PIN 10
#define DOWN_PIN 11

#define RPOS_MAX 1.69
#define RPOS_MIN 0.10

#define FWD_PIN 7
#define REV_PIN 6
#define ENA_PIN 5

#define EPOS_MAX 600
#define EPOS_MIN 0

#define ESFRAC_MIN 0.5

#define E_MOVEMENT_TIMEOUT 2000
#define R_MOVEMENT_TIMEOUT 20000

#define TILT_POINT 72
#define TILT_FRAC 0.55

int loopCount;
int offlinePrint = true;
bool test = false; 		// controls output of prints
bool testAll = false; 	// enables output of detailed position prints

String inputString = "";
bool stringComplete = false;

bool rRunning;
float rPos, rDes, rDis;
int rDir;
bool rMovementActive = false;
unsigned long rMovementTime;
int timer1_counter;

volatile float eSpeed; // SI units
float eSpeedDes = 0.0025; // m/s
float eSFrac = ESFRAC_MIN;
float eSpeed_P_accel = 0.1; 			// P controllers
float eSpeed_P_brake = 0.3;
bool eBrake, eRunning;
float ePos, eDes, eDis;
int eDir;
bool eMovementActive = false;
unsigned long eMovementTime;

bool beforeTilt = true;

float hDes, vDes;
float hPos, vPos;

RotationMotor rMot(UP_PIN, DOWN_PIN);
RotationSensor rSens(A0);

ExtensionMotor eMot(FWD_PIN,REV_PIN,ENA_PIN);
ExtensionSensor eSens;

void setup(){
	pinMode(13,OUTPUT);
	
	pinMode(UP_PIN, OUTPUT); // up
	digitalWrite(UP_PIN, LOW);
	pinMode(DOWN_PIN, OUTPUT); // down
	digitalWrite(DOWN_PIN, LOW);
	pinMode(A0, INPUT);
	
	
	pinMode(FWD_PIN, OUTPUT); // fwd
	digitalWrite(FWD_PIN, LOW);
	pinMode(REV_PIN, OUTPUT); // rev
	digitalWrite(REV_PIN, LOW);
	pinMode(ENA_PIN, OUTPUT); // enable
	digitalWrite(ENA_PIN, LOW);

	noInterrupts();           
  	TCCR1A = 0;
  	TCCR1B = 0;
 	timer1_counter = 34286;   // preload timer 65536-16MHz/256/2Hz
   	TCNT1 = timer1_counter;   // preload timer
  	TCCR1B |= (1 << CS12);    // 256 prescaler 
  	TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  	interrupts();
	
	attachInterrupt(0, ISR_eSens, FALLING); // the extension sensor
	
	Serial.begin(9600);
}

/*
	Interrrupt Service Routine to handle the timed reading of the rotation sensor. Currently set to run at 2Hz, but could be run more often for more detailed positioning.
*/
ISR(TIMER1_OVF_vect){
	interrupts();				// enable interrupts because other interrupt has priority
	TCNT1 = timer1_counter;
	rSpeed = rSens.read();
}


/*
	Callback function for the extension sensor. Reads the current speed from the sensor when the sensor has been triggered.
	@return void
*/
void ISR_eSens(){
	eSpeed = eSens.read();
	//Serial.println(eSens.getLocation());
}

/*
	A function to check whether a complete string  (i.e. one ending in \n) has been sent over the serial connection. It then responds accordingly.
	Possible commands:
	stop 	 - stops all movement
	blink 	 - blinks the on-board LED 10 times
	on		 - turns the on-board LED on
	off		 - turns the on-board LED off
	up		 - rotates the arm completely up (at <36V this is not completed before timeout occurs)
	down 	 - rotates the arm completely down (at <36V this is not completed before timeout occurs)
	extend	 - fully extend the arm
	retract  - fully retract the arm
	aExt #.# - Extend the arm to a position of #.# millimeters (rounded down to a multiplication of 9mm)
	aRot #.# - Rotate the arm to an angle of #.# Radians
	hPos #.# - Move the screen to a horizontal position of #.# mm
	vPos #.# - Move the screen to a vertical position of #.# mm
	getPos	 - Return current values as a String formatted as aExt(##.##) aRot(##.##) hPos(##.##) vPos(##.##)
	home	 - Fully retract the arm, call that position 0 mm extension, then rotate the arm to minimum rotation
	test	 - Enable/disable test messages
	testAll  - Enable/disable detailed position messages etc.
	@return void
*/
void processSerial(){
	while (Serial.available()) {
		char inChar = (char)Serial.read(); 
		inputString += inChar;
		if (inChar == '\n') {
		  stringComplete = true;
		} 
	}
	
	if(stringComplete) {
		if(inputString == "stop\n"){
			rRunning = false;
			eRunning = false;
			eMot.stop();
			rMot.stop();
			rDes = rPos;
			eDes = ePos;
			offlinePrint = true;
		}else if(inputString == "blink\n"){
			Serial.println("Match!");
			for(int i=0;i<10;i++){
				digitalWrite(13, HIGH);
				delay(250);
				digitalWrite(13,LOW);
				delay(250);
		 	}
		}else if(inputString == "on\n"){
			Serial.println("LED on");
		    digitalWrite(13,HIGH);
		}else if(inputString == "off\n"){
			Serial.println("LED off");
		    digitalWrite(13,LOW);
		}else if(inputString == "up\n"){
			Serial.println("Going all the way up");
			rDes = RPOS_MAX;
			rRunning = true;
		}else if(inputString == "down\n"){
			Serial.println("Going all the way down");
			rDes = RPOS_MIN;
			rRunning = true;
		}else if(inputString == "extend\n"){
			Serial.println("Extending fully");
			eDes = EPOS_MAX;
			eRunning = true;
		}else if(inputString == "retract\n"){
			Serial.println("Completely retracting");
			eDes = EPOS_MIN;
			eRunning = true;
		}else if(inputString.startsWith("aExt")){
			eRunning = true;
			String val = inputString.substring(5,inputString.length());
			eDes = val.toFloat();
			eDes = floor(eDes/9.0)*9.0; // convert to intervals of 9mm
			if(eDes<EPOS_MIN) eDes = EPOS_MIN;
			if(eDes>EPOS_MAX) eDes = EPOS_MAX;
			Serial.print("eDes is now ");Serial.println(eDes);
		}else if(inputString.startsWith("aRot")){
			rRunning = true;
			String val = inputString.substring(5,inputString.length());
			rDes = val.toFloat() + RPOS_MIN;  // 0 degrees from interface should correspond to minimum (leveled) setting here
			if(rDes<RPOS_MIN) rDes = RPOS_MIN;
			if(rDes>RPOS_MAX) rDes = RPOS_MAX;
			Serial.print("rDes is now ");Serial.println(rDes);
		}else if(inputString.startsWith("hPos")){
			Serial.println("hPos set");
			String val = inputString.substring(5,inputString.length());
			hDes = val.toFloat();
			calcERfromHV();
			calcDir();
			rRunning = true;
			eRunning = true;
		}else if(inputString.startsWith("vPos")){
			Serial.println("vPos set");
			String val = inputString.substring(5,inputString.length());
			vDes = val.toFloat();
			calcERfromHV();
			calcDir();
			rRunning = true;
			eRunning = true;
		}else if(inputString.startsWith("getPos")){
			calcHVfromER();
			
			Serial.print("aExt(");
			Serial.print(ePos);
			Serial.print(") aRot(");
			Serial.print(rPos);
			Serial.print(") hPos(");
			Serial.print(hPos);
			Serial.print(") vPos(");
			Serial.print(vPos);
			Serial.println(")");
		}else if(inputString == "home\n"){
			Serial.println("Homing extension, rotating down after");
			goHome();
		}else if(inputString == "test\n"){
			test = !test;
		}else if(inputString == "testAll\n"){
			testAll = !testAll;
		}
		
		inputString = "";
		stringComplete = false;
	}
}

/*
	A PID controller, using mainly P and a bit of I, to control the speed of the extension motor.
	This includes braking when at 60% of the target position.
	@return int A status code for debugging
*/
int speedPID_ext(){
	if(!eRunning) return 0; // quit if we're supposed to stand still
	
	if(eDir == 1 && (eDes - ePos) > 0.60 * eDis){
		eBrake = true;
	}else if(eDir == -1 && (ePos - eDes) < 0.60 * eDis){
		eBrake = true;
	}else eBrake = false;
	
	if(!eBrake){
		if(ePos < TILT_POINT){
			eSFrac = TILT_FRAC;
		}else{
			if(beforeTilt){
				eSFrac = ESFRAC_MIN;
				beforeTilt = false;
			}
		}
		
		if(eSFrac < ESFRAC_MIN){
			eSFrac = ESFRAC_MIN;
		}else if(eSpeed < eSpeedDes){
			eSFrac += eSpeed_P_accel * (eSpeedDes - eSpeed);
			if(eSFrac > 1) eSFrac = 1;
		}else if(eSpeed > eSpeedDes){
			eSFrac -= eSpeed_P_accel * (eSpeed - eSpeedDes);
			if(eSFrac < ESFRAC_MIN) eSFrac = ESFRAC_MIN;
		}
		
	}else{
		
		if(eSFrac < ESFRAC_MIN){
			eSFrac = ESFRAC_MIN;
		}else if(eSFrac > ESFRAC_MIN){
			eSFrac -= eSpeed_P_brake  * (eSFrac - ESFRAC_MIN);
		}
	}
	return 1;
}

/*
	This function makes sure the motors do not run for longer than set in their respective _MOVEMENT_TIMEOUT constants. This is to prevent the arm from trying to move while in reality it can't.
	@return int A status code
*/
int timeOutCheck(){
	if(eRunning){
		if(!eMovementActive){
			eMovementTime = millis();
			eMovementActive = true;
			if(ePos > eDes) eDis = ePos - eDes;
			else eDis = eDes - ePos;
		}else{
			if(millis()-eMovementTime>E_MOVEMENT_TIMEOUT){
				if(test) Serial.println("****  EXTENSION TIMEOUT! ****");
				eRunning  = false;
				eMovementActive = false;
				eMot.stop();
				eSFrac = 0;
				return -1;
			}
		}
	}
	
	if(rRunning){
		if(!rMovementActive){
			rMovementTime = millis();
			rMovementActive = true;
			if(rPos > rDes) rDis = rPos - rDes;
			else rDis = rDes - rPos;
		}else{
			if(millis()-rMovementTime>R_MOVEMENT_TIMEOUT){
				if(test) Serial.println("****  ROTATION TIMEOUT! ****");
				rRunning  = false;
				rMovementActive = false;
				rMot.stop();
				rSFrac = 0;
				return -2;
			}
		}
	}
}

/*
	Calculate the movement direction towards the set target to be able to move the motor in the right direction.
	@return void
*/
void calcDir(){
	eDir = eDes>ePos?1:-1;
	rDir = rDes>rPos?1:-1;
}

/*
	Calculate the extension/rotation values desired from the given horizontal/vertical position
	@return void
*/
void calcERfromHV(){
	// the hPos coming in is as extension to the arm
	rDes = atan2(vDes, hDes+ARM_LENGTH);
	if(rDes>RPOS_MAX) rDes = RPOS_MAX;
	if(rDes<RPOS_MIN) rDes = RPOS_MIN;
	
	eDes = sqrt(pow((hDes),2) + vDes * vDes) - ARM_LENGTH; // the command from the GUI already has ARM_LENGTH factored in as a minimum (i.e. 600 mm from GUI = 0 extension)
	eDes = floor(eDes/9.0)*9.0;
	if(eDes>EPOS_MAX) eDes = EPOS_MAX;
	if(eDes<EPOS_MIN) eDes = EPOS_MIN;
	
	if(test) { Serial.print("rDes calculated: ");Serial.println(rDes); Serial.print("eDes calculated: ");Serial.println(eDes); }
}

/*
	Calculate horizontal/vertical position from extension and rotation. This is mainly called before outputting position to be able to send accurate position data without too much calculations.
	@return void
*/
void calcHVfromER(){
	hPos = cos(rPos) * ePos;
	vPos = sin(rPos) * ePos;
	if(test) { Serial.print("hPos calculated: ");Serial.println(hPos); Serial.print("vPos calculated: ");Serial.println(vPos); }
}

/*
	The main program
*/
void loop(){
	processSerial();
	calcDir();
		
	if(eRunning) {
		ePos = eSens.getLocation(); 
	}else{
		eSens.setLocation(ePos); // if it's not under power, assume any changes to be non relevant
	}
	
	rPos = rSens.getLocation();
	
	eSens.setDirection(eDir);
	timeOutCheck();
	moveToDestination();
	
	loopCount++;
	if(loopCount > 250){ 	// this prevents printing extremely often without having to use the delay() function which would impede functioning
		if(eRunning && testAll){ Serial.print("ePos = "); Serial.println(ePos); }
		if(eRunning && testAll){ Serial.print("eDes = "); Serial.println(eDes); }
		if(eRunning && testAll){ Serial.print("eSFrac = "); Serial.println(eSFrac); }
		if(eRunning && testAll){ Serial.print("eSpeed = "); Serial.println(eSpeed); }
		if(eRunning && testAll){ Serial.print("eRunning = "); Serial.println(eRunning); }
		if(eRunning && testAll) Serial.println();
		
		
		if(rRunning && testAll){ Serial.print("rPos = "); Serial.println(rPos); }
		if(rRunning && testAll){ Serial.print("rDes = "); Serial.println(rDes); }
		if(rRunning && testAll){ Serial.print("rSFrac = "); Serial.println(rSFrac); }
		if(rRunning && testAll){ Serial.print("rSpeed = "); Serial.println(rSpeed); }
		if(rRunning && testAll){ Serial.print("rRunning = "); Serial.println(rRunning); }
		if(rRunning && testAll) Serial.println();
		loopCount = 0;
	}
}

/*
	This function checks the current positions to the targets and makes sure the motors stop if they are close to their target. 
	@return void
*/
void moveToDestination(){
	if(!eRunning && !rRunning){
		if(test && offlinePrint) { 
			Serial.println("**** OFFLINE ****"); offlinePrint = false;
			Serial.print("ePos = ");Serial.println(ePos); 
			Serial.print("rPos = ");Serial.print(rPos); Serial.print("  Pot value: ");Serial.println(analogRead(A0));
		}
		rMot.stop();
		rSFrac = 0;
		rMovementActive = false;
		
		eMot.stop();
		eSFrac = 0;
		eMovementActive = false;
		return;
	}
	
	// Extension destination checking and moving
	if(eRunning && ((eDir == 1 && ePos >= eDes-1) || (eDir == -1 && ePos <=eDes+1))){ // if we're close stop
		offlinePrint = true;
		if(test){
			Serial.println("**** EXTENSION: CLOSE CALL! ****");
			Serial.print("ePos = ");Serial.println(ePos);
			Serial.print("eDes = ");Serial.println(eDes);
		}
		eMot.stop();
		eSFrac = ESFRAC_MIN;
		eRunning = false;
		eMovementActive = false;
		
	}else if(eRunning && ((eDir == 1 && ePos >= eDes) || (eDir == -1 && ePos <=eDes)) ){ // in case of overshoot, stop
		offlinePrint = true;
		if(test){
			Serial.println("**** EXTENSION: OVERSHOT! ****");
			Serial.print("ePos = ");Serial.println(ePos);
			Serial.print("eDes = ");Serial.println(eDes);
		}
		eMot.stop();
		eSFrac = 0;
		eRunning = false;
		eMovementActive = false;
		
	}else if(eRunning){
		offlinePrint = true;
		speedPID_ext();
		
		eMot.setSpeed(eSFrac);
		eMot.run(eDir);
		eSens.setDirection(eDir);
			
	}
	
	// same for rotation
	if(rRunning && ((rDir == 1 && rPos >= rDes-0.01) || (rDir == -1 && rPos <=rDes+0.01))){ // if we're close
		offlinePrint = true;
		if(test){
			Serial.println("**** ROTATION: CLOSE CALL! ****");
			Serial.print("rPos = ");Serial.println(rPos);
			Serial.print("rDes = ");Serial.println(rDes);
		}
		rMot.stop();
		//rSFrac = RSFRAC_MIN;
		rRunning = false;
		rMovementActive = false;
		
	}else if(rRunning && ((rDir == 1 && rPos >= rDes) || (rDir == -1 && rPos <=rDes) )){ // in case of overshoot, stop
		offlinePrint = true;
		if(test){
			Serial.println("**** ROTATION: OVERSHOT! ****");
			Serial.print("rPos = ");Serial.println(rPos);
			Serial.print("rDes = ");Serial.println(rDes);
		}
		rMot.stop();
		//rSFrac = 0;
		rRunning = false;
		rMovementActive = false;
		
	}else if(rRunning){
		offlinePrint = true;
		//speedPID_rot();
		rMot.run(rDir);	
	}
	
}

/*
	This function retracts the arm for so long that it will always hit the limit switch, then sets the position to 0. This is needed because the encoder does not know where it is, only how much it has moved.
	@return void
*/
void goHome(){
	if(test) Serial.print("Homing");
	offlinePrint = true;
	rDes = RPOS_MIN;
	rRunning = true;
	eMot.setSpeed(0.7);
	eMot.run(-1);
	for(int i=0;i<20;i++){
		delay(125);
		if(test) Serial.print(".");
	}Serial.println();
	eMot.stop();
	eSens.setLocation(0.0);
	ePos = eSens.getLocation();
	eRunning = false;
	beforeTilt = true;
	if(test) Serial.println("Extension should be home now");
}