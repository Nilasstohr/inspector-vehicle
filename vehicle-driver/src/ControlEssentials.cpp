/*
 * ControlEssentials.cpp
 *
 *  Created on: Jun 27, 2023
 *      Author: robot1
 */

#include "ControlEssentials.h"

ControlEssentials::ControlEssentials(
		MotorDrivers* motors,
		QuadratureEncoders* sensors,
		double maximumOutput,
		double minimumOutput){
	this->motors = motors;
	this->sensors = sensors;
	this->maximumOutput = maximumOutput;
	this->minimumOutput = minimumOutput;
	this->wLeft =0;
	this->wRight=0;
}

double ControlEssentials::getMaximumOutput()  {
	return maximumOutput;
}

double ControlEssentials::getMinimumOutput()  {
	return minimumOutput;
}

MotorDrivers* ControlEssentials::getMotors()  {
	return motors;
}

QuadratureEncoders* ControlEssentials::getSensors()  {
	return sensors;
}

double ControlEssentials::checkForSaturation(double input) {
	if(input < minimumOutput)
		input = minimumOutput;
	else if(input>maximumOutput)
		input = maximumOutput;
	return input;
}

ControlEssentials::~ControlEssentials() {
	// TODO Auto-generated destructor stub
}

