/*
 * TestMotorDrivers.cpp
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#include "TestMotorDrivers.h"

TestMotorDrivers::TestMotorDrivers() {
	Logger::setLogLevel(Logger::VERBOSE);
	canSetMotorDriverPins();
	canSetupMotorDriver();
}

void TestMotorDrivers::canSetMotorDriverPins() {
	Logger::verbose(__FUNCTION__, "testing if all motor setting can be set");
	MotorDriverPins  *motorDriverPins = new MotorDriverPins(
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_D2,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN1,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN2,
			VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY
	);

	if( motorDriverPins->getPwmD2()       != VEHICLE_PIN_MOTOR_DRIVER_LEFT_D2 ||
		motorDriverPins->getIn1()         != VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN1||
		motorDriverPins->getIn2()         != VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN2||
		motorDriverPins->getPwmFrequency()!= VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY)
	{
		this->printMotorSettings(motorDriverPins);
	}
}

void TestMotorDrivers::printMotorSettings(MotorDriverPins *motorDriverPins) {
	char buffer [50];

	sprintf (buffer, "Configuration are: %d,%d,%d,%lu",
			motorDriverPins->getPwmD2(),
			motorDriverPins->getIn1(),
			motorDriverPins->getIn2(),
			motorDriverPins->getPwmFrequency());
	Logger::error(buffer);
	sprintf (buffer, "Expected: %d,%d,%d,%lu",
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_D2,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN1,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN2,
			VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY);
		Logger::error(buffer);
}

void TestMotorDrivers::canSetupMotorDriver() {
	Logger::verbose(__FUNCTION__, "testing motor driver class");
	MotorDriver  *motorDriver = new MotorDriver(
		new MotorDriverPins(
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_D2,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN1,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN2,
			VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY)
	);
	motorDriver->setMotorPwm(30000);
	delay(2000);
	motorDriver->setMotorPwm(0);
}


TestMotorDrivers::~TestMotorDrivers() {
	// TODO Auto-generated destructor stub
}


