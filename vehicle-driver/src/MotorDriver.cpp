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
	//Logger::verbose(__FUNCTION__, "setting Motor D2 pwm frequency");
	analogWriteFrequency(getPwmPin(),this->getPwmFrequency());

	//Logger::verbose(__FUNCTION__, "setting Motor IN1 to output and high");
	pinMode(this->motorDriverPins->getIn1(), OUTPUT);

	//Logger::verbose(__FUNCTION__, "setting Motor IN2 to output and low");
	pinMode(this->motorDriverPins->getIn2(), OUTPUT);

	setForward();

	stop();
}

void MotorDriver::setMotorPwm(uint16_t pwm) {
	//Logger::verbose(__FUNCTION__, "setting M1 inv D2 output to 50% duty");
	analogWrite(getPwmPin(),pwm);
}

uint8_t MotorDriver::getPwmPin() {
	return this->motorDriverPins->getPwmD2();
}

void MotorDriver::setForward() {
	setIn1(HIGH);
	setIn2(LOW);
}

void MotorDriver::setIn1(uint8_t state) {
	digitalWrite(this->motorDriverPins->getIn1(), state);
}

void MotorDriver::stop() {
	setMotorPwm(0);
}

void MotorDriver::setIn2(uint8_t state) {
	digitalWrite(this->motorDriverPins->getIn2(), state);
}

MotorDriver::~MotorDriver() {
	// TODO Auto-generated destructor stub
}
