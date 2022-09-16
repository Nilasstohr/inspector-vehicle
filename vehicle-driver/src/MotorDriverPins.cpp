/*
 * MotorDriverPins.cpp
 *
 *  Created on: Sep 11, 2022
 *      Author: robot1
 */

#include "MotorDriverPins.h"

MotorDriverPins::MotorDriverPins(
		uint8_t pwmD2,
		uint8_t in1,
		uint8_t in2,
		uint32_t pwmFrequency) {
	this->pwmD2 = pwmD2;
	this->in1=in1;
	this->in2=in2;
	this->pwmFrequency = pwmFrequency;
}

MotorDriverPins::~MotorDriverPins() {
	// TODO Auto-generated destructor stub
}

void MotorDriverPins::printPins() {
	char buffer [50];
	sprintf (buffer, "Configuration are: %d,%d,%d,%d",
			getPwmD2(),getIn1(),getIn2(),getPwmFrequency());
	Logger::error(buffer);
}
