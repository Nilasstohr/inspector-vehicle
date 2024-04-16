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
#include <SPI.h>
#include "libraries/RF24/RF24.h"

#define MAX_COUNTS 35000
#define COUNTS_PR_REV 3200

#define VELOC_REF 2

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

RF24 * radio;
const byte address[6] = "00001";
char cmd;

// velocity control of each wheel
double wr=0;
double wl=0;


enum VehicleMode
{
	IDLE,
	DRIVING_MODE,
	TRANSIENT_TEST_MODE,
	VELOCITY_CONTROL_MODE,
	POSITION_CONTROL_MODE,
	COMBO_CONTROL_MODE,
	DATA_REQUEST_MODE
};

void runProgram1();
void velocityControl(bool doControl);
void positionControl();
void comboControl();
void dataRequests();
double radPrSekFromDeltaT(uint32_t deltaT);
void outputResult();
void reset();
void init();
VehicleMode getVehicleMode();
DrivingDirection getDrivingModeManual();
DrivingDirection getDrivingModeManualRadio();
bool modeEscapeRequest();
void drivingMode();
void drivningManual();
bool handlePositionRequest();
bool handleVelocityRequest();
bool handleResetRequest();

extern "C" int main(void){

	init();

	/*
	while(1){
		if (radio->available()) {
			radio->read(&cmd, sizeof(cmd));
			Serial.println(cmd);
		}
		delay(1);
	}


	*/
	//velocityControl(true);
	//Serial.println("entered vehicle mode options (enter mode 1-6)");
	while(1){
		VehicleMode mode = VehicleMode::DRIVING_MODE;//getVehicleMode();
		switch(mode){
			case VehicleMode::DRIVING_MODE:{
				drivningManual();
				break;
			}
			case VehicleMode::TRANSIENT_TEST_MODE:{
				velocityControl(false);
				break;
			}
			case VehicleMode::VELOCITY_CONTROL_MODE:{
				velocityControl(true);
				break;
			}
			case VehicleMode::POSITION_CONTROL_MODE:{
				positionControl();
				break;
			}
			case VehicleMode::COMBO_CONTROL_MODE:{
				comboControl();
				break;
			}
			case VehicleMode::DATA_REQUEST_MODE:{
				dataRequests();
				break;
			}

		}
		delay(1);
	}

}

// command interface
VehicleMode getVehicleMode(){
	if(serial->hasMessage()){
		if(modeEscapeRequest())
			return VehicleMode::IDLE;
		String* cmd = serial->getMessage();
		return  (VehicleMode)atoi( cmd->c_str() );
	}else{
		return VehicleMode::IDLE;
	}
}

DrivingDirection getDrivingModeManual(){
	 String* cmd = serial->getMessage();
	 if(cmd->equals("y")){
		 //Serial.println("going forward");
		 serial->sendAck();
		 return DrivingDirection::FORWARD;
	 }else if(cmd->equals("h")){
		 //Serial.println("going backward");
		 serial->sendAck();
		 return DrivingDirection::BACKWARD;
	 }else if(cmd->equals("d")){
		 //Serial.println("spin left");
		 serial->sendAck();
		 return DrivingDirection::SPIN_LEFT;
	 }else if(cmd->equals("f")){
		 //Serial.println("spin right");
		 serial->sendAck();
		 return DrivingDirection::SPIN_RIGHT;
	 }else if(cmd->equals("g")){
		 //Serial.println("turn left");
		 serial->sendAck();
		 return DrivingDirection::TURN_LEFT;
	 }else if(cmd->equals("j")){

		 //Serial.println("turn right");
		 serial->sendAck();
		 return DrivingDirection::TURN_RIGHT;
	 } else if(cmd->equals("s")){
		 //Serial.println("stopping");
		 serial->sendAck();
		 return DrivingDirection::STOP;
	 }else{
		 return DrivingDirection::UNKNOWN;
	 }
}

DrivingDirection getDrivingModeManualRadio(){
  	 radio->read(&cmd, sizeof(cmd));
	 //Serial.println(cmd);

	 if(cmd=='y'){
		 //Serial.println("going forward");
		 return DrivingDirection::FORWARD;
	 }else if(cmd=='h'){
		 //Serial.println("going backward");
		 return DrivingDirection::BACKWARD;
	 }else if(cmd=='d'){
		 //Serial.println("spin left");
		 return DrivingDirection::SPIN_LEFT;
	 }else if(cmd=='f'){
		 //Serial.println("spin right");

		 return DrivingDirection::SPIN_RIGHT;
	 }else if(cmd=='g'){
		 //Serial.println("turn left");
		 return DrivingDirection::TURN_LEFT;
	 }else if(cmd=='j'){
		 //Serial.println("turn right");
		 return DrivingDirection::TURN_RIGHT;
	 } else if(cmd=='s'){
		 //Serial.println("stopping");
		 return DrivingDirection::STOP;
	 }else{
		 return DrivingDirection::UNKNOWN;
	 }
}

void drivningManual(){
	//Serial.println("entered manual driving mode (default stopped, send y(forward),h(reverse),g(turn left),j(turn right), or s(stop)");
	//serial->sendAck();
	DrivingDirection mode=DrivingDirection::UNKNOWN;
	reset();

	while(1){
		if(serial->hasMessage()){
			if(modeEscapeRequest())
				return;
			else if(handleVelocityRequest())
				mode=DrivingDirection::FORWARD;
			else if(handleResetRequest())
				mode=DrivingDirection::UNKNOWN;
			else if(!handlePositionRequest())
				mode = getDrivingModeManual();
		}else if(radio->available()){
			mode = getDrivingModeManualRadio();
			wr=VELOC_REF;
			wl=VELOC_REF;

		}
		if(mode==DrivingDirection::STOP){
			dualVelocityController->stop();
			mode=DrivingDirection::UNKNOWN;
		}else if(mode==DrivingDirection::UNKNOWN){
			// do nothing

		}else{
			//Serial.println(mode);
			if(mode==DrivingDirection::TURN_LEFT){
				dualVelocityController->update(wl,wr+4,DrivingDirection::FORWARD);
			}else if(mode==DrivingDirection::TURN_RIGHT){
				dualVelocityController->update(wl+4,wr,DrivingDirection::FORWARD);
			}else{
				dualVelocityController->update(wl,wr,mode);
			}

		}
		delayMicroseconds(1);
	}
}

bool modeEscapeRequest(){
	if(serial->validateCommand(1, '0'))
	{
		serial->sendAck();
		return true;
	}
	return false;
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
			  velocityControl(true);
			  delay(200);
			  digitalWrite(LED_BUILTIN, LOW);
			}else{
				Serial.print("invalid input, did you send a new line?");
			}
		}

		delayMicroseconds(1);
	}
}
void velocityControl(bool doControl){
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
	reset();
	Serial.println("entered position control mode");
	dualPositionControl->setReferencePosition(10);
	dualPositionControl->setReferenceTolerance(1);
	while(1){
		dualPositionControl->update();
		if(dualPositionControl->isPositionReached()){
			while(1){
				Serial.println("goal reached");
				delay(500);
			}
		}
		delayMicroseconds(1);
	}
}

void comboControl(){
	reset();
	Serial.println("entered combo control mode");
	VehicleMode mode = VehicleMode::VELOCITY_CONTROL_MODE;
	double dis = 120;
	dualPositionControl->setReferencePosition(dis);
	dualPositionControl->setReferenceTolerance(1);
	double disPosControl = dis -10;
	Serial.println(disPosControl);
	double l = 0;
	double r = 0;

	while(1){
		if(serial->hasMessage()){
			if(modeEscapeRequest())
				return;
		}
		switch(mode){
			case VehicleMode::VELOCITY_CONTROL_MODE:{
				l = dualPositionControl->getLeftPosition();
				r = dualPositionControl->getRightPosition();
				dualVelocityController->update(VELOC_REF,DrivingDirection::FORWARD);
				if(l  > disPosControl  && r  > disPosControl){
					//Serial.print("changing controller to position");
					mode = VehicleMode::POSITION_CONTROL_MODE;
				}
				break;
			}
			case VehicleMode::POSITION_CONTROL_MODE:{
				//Serial.println("position");
				dualPositionControl->update();
				if(dualPositionControl->isPositionReached()){
					Serial.println("position has been reached");
					Serial.print(dualPositionControl->getLeftPosition());
					Serial.print(" ");
					Serial.println(dualPositionControl->getRightPosition());
					return;
				}
				break;
			}
		}
		delayMicroseconds(1);
	}
}


void dataRequests(){
	serial->sendAck();
	//uint32_t tic;
	//uint32_t toc;
	reset();

	//Serial.println("entered data request mode");
	while(1)
	{
		if(serial->hasMessage()){
			//tic = micros();
			if(modeEscapeRequest())
				return;
			handlePositionRequest();
			//toc = micros();
			//Serial.print("(");
			//Serial.print(toc-tic);
			//Serial.print(" us)");
		}
		delayMicroseconds(1);
	}

}

bool handlePositionRequest(){
	if(serial->validateCommand(1, 'p')){
		//Serial.print("33.54 33.56 ");
		Serial.print(dualPositionControl->getLeftPosition());
		Serial.print(" ");
		Serial.print(dualPositionControl->getRightPosition());
		serial->sendAck();
		return true;
	}
	return false;
}

bool handleVelocityRequest(){
	if(serial->validateCommandWithArgs(1, 'v')){
		wl = serial->getArg(0);
		wr = serial->getArg(1);
		//Serial.print("velocity: ");
		//Serial.print(wl);
		//Serial.print(" ");
		//Serial.println(wr);
		serial->sendAck();
		return true;
	}
	return false;
}


bool handleResetRequest(){
	if(serial->validateCommand(1, 'r')){
		reset();
		serial->sendAck();
		return true;
	}
	return false;
}


double radPrSekFromDeltaT(uint32_t deltaT) {
	double s;
	s =  (double)(deltaT/pow(10,6));
	return (double)(RADIANS_PR_COUNT/s);
}
void reset(){
  dualVelocityController->reset();
  dualPositionControl->reset();
  deltaTBufferCount=0;
  delay(500);
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
    radio = new RF24(VEHICLE_PIN_RF24_CE, VEHICLE_PIN_RF24_CSN);
    radio->begin();
	radio->openReadingPipe(0, address);
	radio->setPALevel(RF24_PA_MIN);
	radio->startListening();

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


