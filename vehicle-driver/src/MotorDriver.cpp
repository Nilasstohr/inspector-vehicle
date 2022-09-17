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
	char buffer[100];
	sprintf(buffer,"Setting pwm pin=%d with frequency=%lu",getPwmPin(),this->motorDriverPins->getPwmFrequency());
	Logger::verbose(__FUNCTION__,buffer);
	analogWriteFrequency(getPwmPin(),this->motorDriverPins->getPwmFrequency());

	sprintf(buffer,"Setting in1 pin=%d as output and high",this->motorDriverPins->getIn1());
	Logger::verbose(__FUNCTION__,buffer);
	pinMode(this->motorDriverPins->getIn1(), OUTPUT);

	sprintf(buffer,"Setting in2 pin=%d as output and Low",this->motorDriverPins->getIn2());
    Logger::verbose(__FUNCTION__,buffer);
	pinMode(this->motorDriverPins->getIn2(), OUTPUT);
	setForward();
	stop();
}

void MotorDriver::setMotorPwm(uint16_t pwm) {
	char buffer[100];
	sprintf(buffer,"Setting pwm pin=%d with pwm value=%d",getPwmPin(),pwm);
		Logger::verbose(__FUNCTION__,buffer);
	analogWrite(getPwmPin(),pwm);
}

uint8_t MotorDriver::getPwmPin() {
	return this->motorDriverPins->getPwmD2();
}

void MotorDriver::setForward() {
	setIn1(LOW);
	setIn2(HIGH);
}

void MotorDriver::setReverse() {
	setIn1(HIGH);
	setIn2(LOW);
}

void MotorDriver::setIn1(uint8_t state) {
	digitalWrite(this->motorDriverPins->getIn1(), state);
}

void MotorDriver::setIn2(uint8_t state) {
	digitalWrite(this->motorDriverPins->getIn2(), state);
}

void MotorDriver::stop() {
	setMotorPwm(0);
}


MotorDriver::~MotorDriver() {
	// TODO Auto-generated destructor stub
}
