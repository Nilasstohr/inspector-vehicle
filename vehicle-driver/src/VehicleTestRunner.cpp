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
	new TestMotorDrivers();
}

VehicleTestRunner::~VehicleTestRunner() {
}

