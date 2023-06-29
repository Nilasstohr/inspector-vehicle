#include <Arduino.h>
#include "src/TransposedIIRFilter.h"
#include "src/VehiclePins.h"
#include "src/VehicleParameters.h"
#include "src/VehicleTestToolBox.h"
#include "src/DualPiVelocityControl.h"
#include "src/PiVelocityControllers.h"
#include "src/SerialInterface.h"
#include "src/VehicleDrivingModeTypes.h"
#include "src/DualAccelerationControl.h"



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
DualPPositionControl *  dualPositionControl;
SerialInterface *serial;

enum VehicleMode
{
	DRIVING_MODE,
	TRANSIENT_TEST_MODE,
	POSITION_CONTROL_MODE
};
enum DrivingMode
{
	MANUAL_MODE,

};

void runProgram1();
void velocityContorl(bool doControl);
void positionControl();
double radPrSekFromDeltaT(uint32_t deltaT);
void outputResult();
void reset();
void init();
VehicleMode getVehicleMode();
DrivingMode getDrivingMode();
DrivingDirection getDrivingModeManual();
void drivingMode();
void drivningManual();

extern "C" int main(void){
	init();

	velocityContorl(true);
	Serial.println("entered vehicle mode options");
	while(1){
		VehicleMode mode = getVehicleMode();
		switch(mode){
			case VehicleMode::TRANSIENT_TEST_MODE:{
				velocityContorl(false);
				break;
			}
			case VehicleMode::POSITION_CONTROL_MODE:{
				positionControl();
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
DrivingDirection getDrivingModeManual(){
	 String* cmd = serial->getMessage();
	 if(cmd->equals("f")){
		 Serial.println("going forward");
		 return DrivingDirection::FORWARD;
	 }else if(cmd->equals("b")){
		 Serial.println("going backward");
		 return DrivingDirection::BACKWARD;
	 }else if(cmd->equals("l")){
		 Serial.print("turning left");
		 return DrivingDirection::TURN_LEFT;
	 }else if(cmd->equals("r")){
		 Serial.print("turning right");
		 return DrivingDirection::TURN_RIGHT;
	 }else if(cmd->equals("s")){
		 Serial.println("stopping");
		 return DrivingDirection::STOP;
	 }
	 else{
		 return DrivingDirection::UNKNOWN;
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
	Serial.println("entered manual driving mode (default stopped, send f,b or s)");
	DrivingDirection mode=DrivingDirection::UNKNOWN;
	reset();
	while(1){
		if(serial->hasMessage()){
			mode = getDrivingModeManual();
		}
		if(mode==DrivingDirection::STOP){
			reset();
			mode=DrivingDirection::UNKNOWN;
		}else if(mode==DrivingDirection::UNKNOWN){
			// do noting
		}else{
			dualVelocityController->update(VELOC_REF,mode);
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
			  velocityContorl(true);
			  delay(200);
			  digitalWrite(LED_BUILTIN, LOW);
			}else{
				Serial.print("invalid input, did you send a new line?");
			}
		}

		delayMicroseconds(1);
	}
}
void velocityContorl(bool doControl){
	reset();
	while(1){
		if(deltaTBufferCount<MAX_COUNTS){
			if(doControl){
				dualVelocityController->update(VELOC_REF,DrivingDirection::FORWARD);
			}else{
				dualVelocityController->update(1000);
			}
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

void positionControl(){

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
	toolBox->buildControlEssentials();
	dualVelocityController = toolBox->buildDualPiVelocityControl();
	dualPositionControl = toolBox->buildPositionControl();

	//DualAccelerationControl * accControl =
	//		new DualAccelerationControl(VEHICLE_WHEEL_RADIUS_CM);

	//toolBox->getMotorsDrivers()->forward(0);

	/*
	double position =0;
	uint32_t dt =0;
	while(1){
		position = toolBox->getSensors()->encoder(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left)
					 ->read<double>(QuadratureEncoderReadTypes::position_linear);

		dt =toolBox->getSensors()->encoder(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left)
									 ->read<uint32_t>(QuadratureEncoderReadTypes::time_interval_micros_filtered);
		Serial.println(position);
		delay(200);
	}
	*/

/*
	double dStart = 30;
	double dEnd =50;
	double w0 = 5;
	accControl->setAlfa(dStart, dEnd, w0);

	for(int i=30; i<=dEnd; i++){
		Serial.println(accControl->getVelocity(i));
	}
*/
}


