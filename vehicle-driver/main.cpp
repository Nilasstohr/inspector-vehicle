#include <Arduino.h>
#include "src/TransposedIIRFilter.h"
#include "src/VehiclePins.h"
#include "src/VehicleParameters.h"
#include "src/VehicleTestToolBox.h"
#include "src/DualPiVelocityControl.h"
#include "src/PiVelocityControllers.h"
#include "src/SerialInterface.h"


#define MAX_COUNTS 35000
#define COUNTS_PR_REV 3200

#define VELOC_REF 5
#define RADIANS_PR_COUNT  (double)(2*M_PI/COUNTS_PR_REV)

// sensor sampling
volatile uint32_t deltaTTempLeft=0;
volatile uint32_t deltaTTempRight=0;
volatile uint32_t deltaTLeftBuffer[MAX_COUNTS];
volatile uint32_t deltaTRightBuffer[MAX_COUNTS];
volatile uint32_t deltaTBufferCount =0;
char message[100];

DualPiVelocityControl * dualVelocityController;
SerialInterface *serial;

enum VehicleMode
{
	DRIVING_MODE,
	TRANSIENT_TEST_MODE
};
enum DrivingMode
{
	MANUAL_MODE,
};

enum DrivingModeManual
{
	FORWARD,
	BACKWARD,
	TURN_LEFT,
	TURN_RIGHT,
	STOP,
	UNKNOWN
};

void runProgram1();
void velocityContorl();
double radPrSekFromDeltaT(uint32_t deltaT);
void outputResult();
void reset();
void init();
VehicleMode getVehicleMode();
DrivingMode getDrivingMode();
DrivingModeManual getDrivingModeManual();
void drivingMode();
void drivningManual();

extern "C" int main(void){
	init();
	//velocityContorl();
	Serial.println("entered vehicle mode options");
	while(1){
		VehicleMode mode = getVehicleMode();
		switch(mode){
			case VehicleMode::TRANSIENT_TEST_MODE:{
				//velocityContorl();
				break;
			}
			case VehicleMode::DRIVING_MODE:{
				drivingMode();
				break;
			}
		}
		delay(100);
	}

}

// command interface
VehicleMode getVehicleMode(){
	return VehicleMode::DRIVING_MODE;
}
DrivingMode getDrivingMode(){
	return DrivingMode::MANUAL_MODE;
}
DrivingModeManual getDrivingModeManual(){
	 String* cmd = serial->getReceived();
	 if(cmd->equals("f")){
		 return DrivingModeManual::FORWARD;
	 }else if(cmd->equals("b")){
		 return DrivingModeManual::BACKWARD;
	 }else if(cmd->equals("s")){
		 return DrivingModeManual::STOP;
	 }
	 else{
		 return DrivingModeManual::UNKNOWN;
	 }

}

void drivingMode(){
	Serial.println("entered driving mode options");
	while(1){
		DrivingMode mode = getDrivingMode();
		switch(mode){
			case DrivingMode::MANUAL_MODE:{
				drivningManual();
				break;
			}
		}

		delay(100);
	}

}
void drivningManual(){
	Serial.println("entered manual driving mode (default stopped)");
	DrivingModeManual mode=DrivingModeManual::UNKNOWN;
	while(1){
		if(serial->availableRead()){
			mode = getDrivingModeManual();
		}
		switch(mode){
			case DrivingModeManual::FORWARD:{
				dualVelocityController->update(VELOC_REF);
				break;
			}
			case DrivingModeManual::BACKWARD:{
				break;
			}
			case DrivingModeManual::STOP:{
				reset();
				break;
			}
			default:{

			}
		}

		delayMicroseconds(1);
	}
}
//--------------------------------------

void runProgram1(){
	while(1){
		//Serial.println("waiting for input");
		if(Serial.available()){
			int i=0;
			while (Serial.available()){
				message[i] = Serial.read();
				i++;
			}
			String s = message;
			if(s.equals("start")){
			  Serial.flush();
			  //Serial.print("running test...");
			  digitalWrite(LED_BUILTIN, HIGH);
			  velocityContorl();
			  delay(200);
			  digitalWrite(LED_BUILTIN, LOW);
			}else{
				Serial.print("invalid input, did you send a new line?");
			}
		}

		delayMicroseconds(1);
	}
}
void velocityContorl(){
	reset();
	while(1){
		if(deltaTBufferCount<MAX_COUNTS){
			dualVelocityController->update(VELOC_REF);
			// store data
			if(dualVelocityController->wasSensorReady()){
				deltaTLeftBuffer[deltaTBufferCount] = dualVelocityController->getDeltaTLeft();
				deltaTRightBuffer[deltaTBufferCount]= dualVelocityController->getDeltaTRight();
				deltaTBufferCount++;
			}
		}else{
			reset();
			break;
		}
		delayMicroseconds(1);
	}
	outputResult();
}
double radPrSekFromDeltaT(uint32_t deltaT) {
	double s;
	s =  (double)(deltaT/pow(10,6));
	return (double)(RADIANS_PR_COUNT/s);
}
void reset(){
  dualVelocityController->reset();
  deltaTBufferCount=0;
}
void outputResult(){
	uint32_t sumDeltaT = 0;
		double sumOmega =0;
		for(int i=0; i<MAX_COUNTS; i++){
			sumDeltaT = sumDeltaT + deltaTLeftBuffer[i];
			sumOmega =   sumOmega +   radPrSekFromDeltaT(deltaTLeftBuffer[i]);
			if(i==MAX_COUNTS-1){

				double meanDeltaT = sumDeltaT/MAX_COUNTS;
				double meanOmega =  sumOmega/MAX_COUNTS;
				double varDeltaT = 0;
				double varOmega = 0;
				for(int n = 0; n < MAX_COUNTS; n++ )
				{
				  //Serial.print(n);
				  //Serial.print(deltaTLeftBuffer[n]);

				  Serial.print(" ");
				  Serial.print(radPrSekFromDeltaT(deltaTLeftBuffer[n]));

				  Serial.print(" ");
				  Serial.println(radPrSekFromDeltaT(deltaTRightBuffer[n]));

				  //Serial.print(" ");
				  //Serial.println(radPrSekFromDeltaT(deltaTLeftBuffer[n])/(2*M_PI)*60);

				  //Serial.print("  mean deltaT=");
				  //Serial.print(meanDeltaT);

				  //Serial.print("  mean omega=");
				  //Serial.println(meanOmega);
				  varDeltaT += pow( deltaTLeftBuffer[n] - meanDeltaT, 2);
				  varOmega += pow( radPrSekFromDeltaT(deltaTLeftBuffer[n]) - meanOmega, 2);
				}
				double varianceDeltaT = varDeltaT /= MAX_COUNTS;
				Serial.print(" mean delta T=");
				Serial.println(meanDeltaT);
				Serial.print(" sd delta T=");
				Serial.println(sqrt(varianceDeltaT));
				double varianceOmega= varOmega /= MAX_COUNTS;
				Serial.print(" mean omega=");
				Serial.println(meanOmega);
				Serial.print(" sd omega=");
				Serial.println(sqrt(varianceOmega));
				Serial.print(" mean RPM=");
				Serial.println(meanOmega/(2*M_PI)*60);
				Serial.print(" sd RPM=");
				Serial.println(sqrt(varianceOmega)/(2*M_PI)*60);
				return;
			}
		}
}
void init(){
	VehicleTestToolBox *toolBox = new VehicleTestToolBox();
	serial = new SerialInterface();
	toolBox->buildMotorDrivers();
	toolBox->buildQuadratureEncoders();
	dualVelocityController = toolBox->buildDualPiVelocityControl();

}


