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
	canReadCurrentSenseWhenStopped();
	canReadCurrentSenseWhenStarted();
	//canDetermineLowestPWM();
}

void TestMotorDrivers::canSetMotorDriverPins() {
	Logger::verbose(__FUNCTION__, " - TEST");
	MotorDriverPins  *motorDriverPins = new MotorDriverPins(
		VEHICLE_PIN_MOTOR_DRIVER_LEFT_PWM,
		VEHICLE_PIN_MOTOR_DRIVER_LEFT_INA,
		VEHICLE_PIN_MOTOR_DRIVER_LEFT_INB,
		VEHICLE_PIN_MOTOR_DRIVER_LEFT_FB,
		VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY,
		VEHICLE_MOTOR_DRIVER_CURRENT_MILLIVOLT_PR_AMP
	);

	if( motorDriverPins->getPwm()         != VEHICLE_PIN_MOTOR_DRIVER_LEFT_PWM ||
		motorDriverPins->getInA()           != VEHICLE_PIN_MOTOR_DRIVER_LEFT_INA||
		motorDriverPins->getInB()           != VEHICLE_PIN_MOTOR_DRIVER_LEFT_INB||
		motorDriverPins->getFb()            != VEHICLE_PIN_MOTOR_DRIVER_LEFT_FB ||
		motorDriverPins->getPwmFrequency()  != VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY||
		motorDriverPins->getMilliVoltPrAmp()!= VEHICLE_MOTOR_DRIVER_CURRENT_MILLIVOLT_PR_AMP
		)
	{
		this->printMotorSettings(motorDriverPins);
	}
}

void TestMotorDrivers::printMotorSettings(MotorDriverPins *motorDriverPins) {
	char buffer [100];
	sprintf (buffer, "Configuration are: %d,%d,%d,%lu",
			motorDriverPins->getPwm(),
			motorDriverPins->getInA(),
			motorDriverPins->getInB(),
			motorDriverPins->getPwmFrequency());
	Logger::error(buffer);
	sprintf (buffer, "Expected: %d,%d,%d,%d",
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_PWM,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_INA,
			VEHICLE_PIN_MOTOR_DRIVER_LEFT_INB,
			VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY);
	Logger::error(buffer);
}

void TestMotorDrivers::canSetupMotorDriver() {
	Logger::verbose(__FUNCTION__, " - TEST left");

	MotorDriver  *left = new MotorDriver(
			new MotorDriverPins(
				VEHICLE_PIN_MOTOR_DRIVER_LEFT_PWM,
				VEHICLE_PIN_MOTOR_DRIVER_LEFT_INA,
				VEHICLE_PIN_MOTOR_DRIVER_LEFT_INB,
				VEHICLE_PIN_MOTOR_DRIVER_LEFT_FB,
				VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY,
				VEHICLE_MOTOR_DRIVER_CURRENT_MILLIVOLT_PR_AMP
				)
	);
	left->setMotorPwm(30000);
	delay(2000);
	left->setMotorPwm(0);

	Logger::verbose(__FUNCTION__, " - TEST right");
	MotorDriver  *right = new MotorDriver(
			new MotorDriverPins(
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_PWM,
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_INA,
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_INB,
				VEHICLE_PIN_MOTOR_DRIVER_RIGHT_FB,
				VEHICLE_MOTOR_DRIVER_PWM_FREQUENCY,
				VEHICLE_MOTOR_DRIVER_CURRENT_MILLIVOLT_PR_AMP
				)
		);
	right->setMotorPwm(30000);
	delay(2000);
	right->setMotorPwm(0);
}

void TestMotorDrivers::canSetupMotorDrivers() {
	Logger::verbose(__FUNCTION__, " - TEST");
	MotorDrivers *motorDrivers = buildMotorDrivers();
}

void TestMotorDrivers::canDriveForward() {
	delay(2000);
	Logger::verbose(__FUNCTION__, " - TEST");
	MotorDrivers *motorDrivers = buildMotorDrivers();
	motorDrivers->forward(30000);
	delay(2000);
	motorDrivers->stop();
}

void TestMotorDrivers::canDriveReverse() {
	delay(2000);
	Logger::verbose(__FUNCTION__, " - TEST");
	MotorDrivers *motorDrivers = buildMotorDrivers();
	motorDrivers->reverse(30000);
	delay(2000);
	motorDrivers->stop();
}

void TestMotorDrivers::canReadCurrentSenseWhenStopped() {
	Logger::verbose(__FUNCTION__, " - TEST");
	MotorDrivers *motorDrivers = buildMotorDrivers();
	motorDrivers->stop();
	delay(2000);
	checkCurrentOutputByState(motorDrivers,false);
}

void TestMotorDrivers::canReadCurrentSenseWhenStarted() {
	Logger::verbose(__FUNCTION__, " - TEST");
	MotorDrivers *motorDrivers = buildMotorDrivers();
	motorDrivers->stop();
	delay(2000);
	motorDrivers->forward(30000);
	delay(2000);
	checkCurrentOutputByState(motorDrivers,true);
	motorDrivers->stop();
}


void TestMotorDrivers::canDetermineLowestPWM() {
	Logger::verbose(__FUNCTION__, " - TEST");
	MotorDrivers *motorDrivers = buildMotorDrivers();
	motorDrivers->stop();
	delay(2000);
	motorDrivers->forward(65000);
	delay(2000);
	checkCurrentOutputByState(motorDrivers,true);
	motorDrivers->stop();
}

// helper functions

void TestMotorDrivers::checkCurrentOutputByState(MotorDrivers *motorDrivers,bool state){
	checkCurrentOutput(getCurrent(motorDrivers,Side::left),state);
	checkCurrentOutput(getCurrent(motorDrivers,Side::right),state);
}

float TestMotorDrivers::getCurrent(MotorDrivers *motorDrivers,Side side) {
	float current = motorDrivers->getCurrent(side);
	String * log = new String();
	log->append(current).append(" mA").append(" for ").append(side.toString());
	Logger::verbose(log->c_str());
	return current;
}
void TestMotorDrivers::checkCurrentOutput(float current,bool state){
	if(state){
		if(current > VEHICLE_MIMIMUM_CURRENT_DRAW_MILLI_AMP){
			Logger::error("current is drawn from motor","");
		}
	}else{
		if(current < VEHICLE_MIMIMUM_CURRENT_DRAW_MILLI_AMP){
			Logger::verbose("no current is drawn from motor","");
		}
	}
}
TestMotorDrivers::~TestMotorDrivers() {

}


