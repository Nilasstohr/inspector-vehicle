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
	testDecreasedEncoderCounts();
}

void TestVehicleSystem::testIncreasedEncoderCounts() {
	char buffer[100];
	Logger::verbose(__FUNCTION__, "- TEST");
	delay(2000);
	encoders()->reset();
	motors()->forward(30000);
	delay(1000);

	signed int count;
	for(int i=0;i<2;i++){
		count = encoders()->count((QuadratureEncoders::QuadratureEncoderSide)(i));
		sprintf(buffer,"%d Encoder count %d",i,count);
		Logger::verbose(__FUNCTION__, buffer);
		if(count<=0){
			sprintf(buffer,"Encoder not counting up %d",count);
			Logger::error(buffer);
		}
	}
	motors()->stop();
}

void TestVehicleSystem::testDecreasedEncoderCounts() {
	char buffer[100];
	Logger::verbose(__FUNCTION__, "- TEST");
	delay(4000);
	encoders()->reset();
	motors()->reverse(0);
	delay(1000);

	signed int count;
	while(1){
	   sprintf(buffer,"%d  %d",
			   encoders()->left()->count(),
			   encoders()->right()->count()
	   );
	   Logger::verbose(__FUNCTION__, buffer);
	}
	/*
	for(int i=0;i<2;i++){
		count = encoders()->count((QuadratureEncoders::QuadratureEncoderSide)(i));
		sprintf(buffer,"%d Encoder count %d",i,count);
		Logger::verbose(__FUNCTION__, buffer);
		if(count>=0){
			sprintf(buffer,"Encoder counting up %d",count);
			Logger::error(buffer);
		}
	}
	*/
	motors()->stop();
}


TestVehicleSystem::~TestVehicleSystem() {
}

