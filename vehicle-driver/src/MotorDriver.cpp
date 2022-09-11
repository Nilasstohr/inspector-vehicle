/*
 * MotorDriver.cpp
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#include "MotorDriver.h"



MotorDriver::MotorDriver(MotorDriverPins *motorDriverPins) {
	this->motorDriverPins = motorDriverPins;
	this->setup();
}

void MotorDriver::setup() {
	Logger::verbose(__FUNCTION__, "setting Motor D2 pwm frequency");
	analogWriteFrequency(getPwmPin(),this->getPwmFrequency());

	Logger::verbose(__FUNCTION__, "setting Motor IN1 to output and high");
	pinMode(this->motorDriverPins->getIn1(), OUTPUT);
	digitalWrite(this->motorDriverPins->getIn1(), HIGH);

	Logger::verbose(__FUNCTION__, "setting Motor IN2 to output and low");
	pinMode(this->motorDriverPins->getIn2(), OUTPUT);
	digitalWrite(this->motorDriverPins->getIn2(), LOW);

	setMotorPwm(0);
}

void MotorDriver::setMotorPwm(uint16_t pwm) {
	Logger::verbose(__FUNCTION__, "setting M1 inv D2 output to 50% duty");
	analogWrite(getPwmPin(),pwm);
}

uint8_t MotorDriver::getPwmPin() {
	return this->motorDriverPins->getPwmD2();
}

MotorDriver::~MotorDriver() {
	// TODO Auto-generated destructor stub
}


