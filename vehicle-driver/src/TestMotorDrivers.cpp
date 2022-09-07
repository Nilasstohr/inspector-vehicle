/*
 * TestMotorDrivers.cpp
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#include "TestMotorDrivers.h"

TestMotorDrivers::TestMotorDrivers() {
	canSetupMotorDriver();
}

void TestMotorDrivers::canSetupMotorDriver() {
	this->motorDriverLeft = new MotorDriver(
		new MotorDriverPins(
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_D2,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN1,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN2)
	);
}

TestMotorDrivers::~TestMotorDrivers() {
	// TODO Auto-generated destructor stub
}

