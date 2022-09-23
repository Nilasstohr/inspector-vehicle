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
	//testIncreasedEncoderCounts();
	//testDecreasedEncoderCounts();
	testCanGetPosition();
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

void TestVehicleSystem::testCanGetPosition() {
	Logger::verbose(__FUNCTION__, "- TEST");
	delay(2000);
	encoders()->reset();
	motors()->forward(30000);
	delay(1000);
	String * log = new String();
	double position;
	for(int i=0;i<2;i++){
		position = encoders()->encoder((QuadratureEncoders::QuadratureEncoderSide)(i))->getPosition();
		Serial.println(position,6);
		if(position<=0){
			 log->append("encoder ")
					 .append(i)
					 .append(" of vehicle not moving forward, position is ")
					 .append(position)
					 .newLine();
			 Logger::error(log->c_str());
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

