/*
 * TestMotorDrivers.cpp
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#include "TestMotorDrivers.h"

TestMotorDrivers::TestMotorDrivers() {
	Logger::setLogLevel(Logger::VERBOSE);
	Logger::verbose(__FUNCTION__, "############# TEST CLASS ############# ");
	canSetMotorDriverPins();
	canSetupMotorDriver();
	canSetupMotorDrivers();
	canDriveForward();
	canDriveReverse();
}

void TestMotorDrivers::canSetMotorDriverPins() {
	Logger::verbose(__FUNCTION__, " - TEST");
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
	char buffer [100];
	sprintf (buffer, "Configuration are: %d,%d,%d,%lu",
			motorDriverPins->getPwmD2(),
			motorDriverPins->getIn1(),
			motorDriverPins->getIn2(),
			motorDriverPins->getPwmFrequency());
	Logger::error(buffer);
	sprintf (buffer, "Expected: %d,%d,%d,%d",
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_D2,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN1,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN2,
			VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY);
	Logger::error(buffer);
}

void TestMotorDrivers::canSetupMotorDriver() {
	Logger::verbose(__FUNCTION__, " - TEST left");

	analogWriteResolution(16);

	MotorDriver  *left = new MotorDriver(
		new MotorDriverPins(
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_D2,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN1,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_IN2,
			VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY)
	);
	left->setMotorPwm(30000);
	delay(2000);
	left->setMotorPwm(0);

	Logger::verbose(__FUNCTION__, " - TEST right");
	MotorDriver  *right = new MotorDriver(
			new MotorDriverPins(
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_D2,
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_IN1,
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_IN2,
				VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY)
		);
	right->setMotorPwm(30000);
	delay(2000);
	right->setMotorPwm(0);
}

void TestMotorDrivers::canSetupMotorDrivers() {
	Logger::verbose(__FUNCTION__, " - TEST");
	MotorDrivers *motorDrivers = createMotorDrivers();
}

void TestMotorDrivers::canDriveForward() {
	delay(2000);
	Logger::verbose(__FUNCTION__, " - TEST");
	MotorDrivers *motorDrivers = createMotorDrivers();
	motorDrivers->forward(30000);
	delay(2000);
	motorDrivers->stop();
}

void TestMotorDrivers::canDriveReverse() {
	delay(2000);
	Logger::verbose(__FUNCTION__, " - TEST");
	MotorDrivers *motorDrivers = createMotorDrivers();
	motorDrivers->reverse(30000);
	delay(2000);
	motorDrivers->stop();
}

TestMotorDrivers::~TestMotorDrivers() {

}


