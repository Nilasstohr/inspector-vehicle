/*
 * TestVehiclePIControl.cpp
 *
 *  Created on: Dec 30, 2022
 *      Author: robot1
 */

#include "TestVehiclePIControl.h"

static CSyncEvent g_SampleEventTimerControl;

extern "C" void IrqSampleTimerControl(void){
	g_SampleEventTimerControl();
}

TestVehiclePIControl::TestVehiclePIControl() {
	Logger::setLogLevel(Logger::VERBOSE);
	Logger::verbose(__FUNCTION__, "############# TEST CLASS ############# ");

	this->motorDrivers = createMotorDrivers();
	this->quadratureEncoders = createQuadratureEncoders();
	this->encoders()->setupEncoders();

	this->sampleTimer = new IntervalTimer;
	g_SampleEventTimerControl.Bind(this,&TestVehiclePIControl::sampleEventTimerHandler);

	canInitializePiController();
	canPerformPiControl();
}

void TestVehiclePIControl::canInitializePiController(){
	Logger::verbose(__FUNCTION__, "- TEST");
	this->piController =  new PiController(
			VEHICLE_PI_CONTROL_COEFFICIENT_B0,
			VEHICLE_PI_CONTROL_COEFFICIENT_B1,
			VEHICLE_PI_CONTROL_COEFFICIENT_A1);
}

void TestVehiclePIControl::canPerformPiControl() {
	Logger::verbose(__FUNCTION__, "- TEST");
	Logger::verbose("staring timer");

	unsigned int interval = 1000;
	if(!sampleTimer->begin(IrqSampleTimerControl,(unsigned int)(interval))){
		Logger::error("invalid timer interval");
		return;
	}

	motors()->forward(65000);

	double output = 0;

	double omega = 0;
	double feedForward = 0;
	double feedBack = 0;
	uint16_t ticksMax = 100;
	uint32_t omegas[ticksMax];


	for(uint16_t i=0; i<ticksMax; i++){
		//if(ready){
			ready = false;
			omegas[i] = this->omega;
			output = this->piController->update(omegas[i],5);
			//motors()->forward((uint16_t)output);
			//Serial.print("w=");
			//Serial.print(omega);
			//Serial.println(i);
			/*
			if(omega > 8)
				omega = 8;
			Serial.print("w=");
			Serial.print(omega);
			feedForward = this->piController->feedForward;
			feedBack    = this->piController->feedBack;
			output = this->piController->update(omega,5);
			Serial.print(" e=");
			Serial.print(this->piController->error);
			Serial.print(" ff=");
			Serial.print(feedForward);
			Serial.print(" fb=");
			Serial.print(feedBack);
			Serial.print(" o=");
			Serial.println(output);
			motors()->forward((uint16_t)output);

			*/
			delayMicroseconds(1);
    }

	sampleTimer->end();
	motors()->forward(0);
	for(uint16_t i=0; i<ticksMax; i++){
		Serial.println(omegas[i]);
	}

}

void TestVehiclePIControl::sampleEventTimerHandler() {
	this->omega = encoders()->encoder(QuadratureEncoders::quadrature_encoder_left)
					 ->read<uint32_t>(QuadratureEncoderReadTypes::time_interval_micros);
}


TestVehiclePIControl::~TestVehiclePIControl() {
	// TODO Auto-generated destructor stub
}

