/*
 * MotorDriverPins.cpp
 *
 *  Created on: Sep 11, 2022
 *      Author: robot1
 */

#include "MotorDriverPins.h"

MotorDriverPins::MotorDriverPins(
		uint8_t pwm,
		uint8_t inA,
		uint8_t inB,
		uint8_t fb,
		uint32_t pwmFrequency,
		uint16_t milliVoltPrAmp) {
	this->pwm = pwm;
	this->inA=inA;
	this->inB=inB;
	this->fb = fb;
	this->pwmFrequency = pwmFrequency;
	this->milliVoltPrAmp = milliVoltPrAmp;

}
void MotorDriverPins::printPins() {
	char buffer [50];
	sprintf (buffer, "Configuration are: %d,%d,%d,%d",
			getPwm(),getInA(),getInB(),getPwmFrequency());
	Logger::error(buffer);
}
MotorDriverPins::~MotorDriverPins() {
	// TODO Auto-generated destructor stub
}
