/*
 * TestVehicleSystem.cpp
 *
 *  Created on: Sep 15, 2022
 *      Author: robot1
 */

#include "TestVehicleSystem.h"
#include <string>

TestVehicleSystem::TestVehicleSystem() {
	Logger::setLogLevel(Logger::VERBOSE);
	Logger::verbose(__FUNCTION__, "############# TEST CLASS ############# ");

	this->motorDrivers = createMotorDrivers();
	this->quadratureEncoders = createQuadratureEncoders();
	this->encoders()->setupEncoders();
	// tests
	testIncreasedEncoderCounts();
	testDecreasedEncoderCounts();
}

// Tests

void TestVehicleSystem::testIncreasedEncoderCounts() {
	Logger::verbose(__FUNCTION__, "- TEST");
	delay(2000);
	encoders()->reset();
	motors()->forward(30000);
	delay(1000);

	String * log;
	signed int count;
	for(int i=0;i<2;i++){
		log = this->count(count, (QuadratureEncoders::QuadratureEncoderSide)(i));
		if(count<=0){
			Logger::error(log->append(", not counting up as expected").c_str());
		}
	}
	motors()->stop();
}

void TestVehicleSystem::testDecreasedEncoderCounts() {
	Logger::verbose(__FUNCTION__, "- TEST");
	delay(2000);
	encoders()->reset();
	motors()->reverse(30000);
	delay(1000);

	String * log;
	signed int count;
	for(int i=0;i<2;i++){
		log = this->count(count, (QuadratureEncoders::QuadratureEncoderSide)(i));
		if(count>=0){
			Logger::error(log->append(", not counting down as expected").c_str());
		}
	}
	motors()->stop();
}

// helpers
String *TestVehicleSystem::count(signed int &count,int side) {
	String * log = encoders()->count(count,(QuadratureEncoders::QuadratureEncoderSide)(side));
	Logger::verbose(__FUNCTION__,log->c_str());
	return log;
}

TestVehicleSystem::~TestVehicleSystem() {
}

