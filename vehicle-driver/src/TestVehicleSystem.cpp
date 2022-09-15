/*
 * TestVehicleSystem.cpp
 *
 *  Created on: Sep 15, 2022
 *      Author: robot1
 */

#include "TestVehicleSystem.h"

TestVehicleSystem::TestVehicleSystem() {
	Logger::setLogLevel(Logger::VERBOSE);
	Logger::verbose(__FUNCTION__, "############# TEST CLASS ############# ");

	this->motorDrivers = createMotorDrivers();
	this->quadratureEncoders = createQuadratureEncoders();
	this->encoders()->setupEncoders();

	// tests
	testIncreasedEncoderCounts();
}

void TestVehicleSystem::testIncreasedEncoderCounts() {
	Logger::verbose(__FUNCTION__, "- TEST");
	delay(2000);
	encoders()->reset();
	motors()->forward(30000);
	delay(1000);
	if(encoders()->left()->count()<=0){
		Logger::error("Left Encoder not counting up");
	}
	Logger::verbose(__FUNCTION__, "Left count was");
	if(encoders()->right()->count()<=0){
		Logger::error("Right Encoder not counting up");
	}
	Logger::verbose(__FUNCTION__, "Right count was");
	motors()->stop();
}


TestVehicleSystem::~TestVehicleSystem() {
	// TODO Auto-generated destructor stub
}

