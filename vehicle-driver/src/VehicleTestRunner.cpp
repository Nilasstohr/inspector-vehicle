/*
 * TestRunner.cpp
 *
 *  Created on: Aug 18, 2022
 *      Author: robot1
 */

#include <src/VehicleTestRunner.h>

VehicleTestRunner::VehicleTestRunner() {
	Logger::setLogLevel(Logger::VERBOSE);
	Logger::verbose(__FUNCTION__, "Starting Vehicle Tests");
	//new TestQuadratureEncorder();
	//new TestMotorDrivers();
	//new TestVehicleSystem();
	//new TestVehiclePIControl();
	while(1){
		Serial.println("hello from teensy 4.1");
		delay(1000);
	}
}

VehicleTestRunner::~VehicleTestRunner() {
}

