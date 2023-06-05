/*
 * MotorDriver.cpp
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#include "MotorDriver.h"



MotorDriver::MotorDriver(MotorDriverPins *motorDriverPins)
:CurrentSensor(
		motorDriverPins->getFb(),
		motorDriverPins->getMilliVoltPrAmp())
{
	this->motorDriverPins = motorDriverPins;
	this->setup();
}

void MotorDriver::setup() {
	char buffer[100];
	sprintf(buffer,"Setting pwm pin=%d with frequency=%lu",getPwmPin(),this->motorDriverPins->getPwmFrequency());
	Logger::verbose(__FUNCTION__,buffer);
	pinMode(getPwmPin(),OUTPUT);
	analogWriteFrequency(getPwmPin(),this->motorDriverPins->getPwmFrequency());

	sprintf(buffer,"Setting inA pin=%d as output and high",this->motorDriverPins->getInA());
	Logger::verbose(__FUNCTION__,buffer);
	pinMode(this->motorDriverPins->getInA(), OUTPUT);

	sprintf(buffer,"Setting inB pin=%d as output and Low",this->motorDriverPins->getInB());
    Logger::verbose(__FUNCTION__,buffer);
	pinMode(this->motorDriverPins->getInB(), OUTPUT);
	setForward();
	stop();
}

void MotorDriver::setMotorPwm(uint16_t pwm) {
	//char buffer[100];
	//sprintf(buffer,"Setting pwm pin=%d with pwm value=%d",getPwmPin(),pwm);
	//	Logger::verbose(__FUNCTION__,buffer);
	analogWrite(getPwmPin(),pwm);
}

uint8_t MotorDriver::getPwmPin() {
	return this->motorDriverPins->getPwm();
}

void MotorDriver::setForward() {
	setInA(LOW);
	setInB(HIGH);
}

void MotorDriver::setReverse() {
	setInA(HIGH);
	setInB(LOW);
}

void MotorDriver::setInA(uint8_t state) {
	digitalWrite(this->motorDriverPins->getInA(), state);
}

void MotorDriver::setInB(uint8_t state) {
	digitalWrite(this->motorDriverPins->getInB(), state);
}

void MotorDriver::stop() {
	setMotorPwm(0);
}


MotorDriver::~MotorDriver() {
	// TODO Auto-generated destructor stub
}
