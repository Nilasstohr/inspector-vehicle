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

void MotorDrivers::forward(uint16_t pwmLeft,uint16_t pwmRight) {
	setForward();
	this->left()->setMotorPwm(pwmLeft);
	this->right()->setMotorPwm(pwmRight);
}


void MotorDrivers::stop() {
	this->left()->stop();
	this->right()->stop();
}

void MotorDrivers::reverse(uint16_t pwm) {
	setReverse();
	this->left()->setMotorPwm(pwm);
	this->right()->setMotorPwm(pwm);
}

float MotorDrivers::getCurrent(Side side) {
	switch(side){
		case Side::left:
			return this->left()->getCurrent();
		default:
			return this->right()->getCurrent();
	}
}

void MotorDrivers::setForward() {
	this->left()->setForward();
	this->right()->setForward();
}

void MotorDrivers::setReverse() {
	this->left()->setReverse();
	this->right()->setReverse();
}
