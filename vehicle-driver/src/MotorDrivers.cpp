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

