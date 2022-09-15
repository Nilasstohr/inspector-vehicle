/*
 * MotorDrivers.cpp
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#include "MotorDrivers.h"

MotorDrivers::MotorDrivers(MotorDriverPins *motorDriverPinsLeft,
		MotorDriverPins *motorDriverPinsRight) {
	this->motorDriverLeft  = new MotorDriver(motorDriverPinsLeft);
	this->motorDriverRight = new MotorDriver(motorDriverPinsRight);
}

MotorDrivers::~MotorDrivers() {
	// TODO Auto-generated destructor stub
}

void MotorDrivers::forward(uint16_t pwm) {
	setForward();
	this->left()->setMotorPwm(pwm);
	this->right()->setMotorPwm(pwm);
}

void MotorDrivers::stop() {
	this->left()->stop();
	this->right()->stop();
}

void MotorDrivers::setForward() {
	this->left()->setForward();
	this->right()->setForward();
}
