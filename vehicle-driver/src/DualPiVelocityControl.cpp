/*
 * PiDualControl.cpp
 *
 *  Created on: Jun 5, 2023
 *      Author: robot1
 */

#include "DualPiVelocityControl.h"

DualPiVelocityControl::DualPiVelocityControl(
		MotorDrivers* motors,
		QuadratureEncoders* sensors,
		PiVelocityControllers*  controlFilters){
	this->motors = motors;
	this->sensors = sensors;
	this->controlFilters = controlFilters;
}

DualPiVelocityControl::~DualPiVelocityControl() {
	// TODO Auto-generated destructor stub
}

