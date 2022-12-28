/*
 * MotorDriver.h
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#ifndef SRC_MOTORDRIVER_H_
#define SRC_MOTORDRIVER_H_

#include "MotorDriverPins.h"
#include "Logger.h"
#include "CurrentSensor.h"

class MotorDriver: public CurrentSensor{
public:
	MotorDriver(MotorDriverPins * motorDriverPins);
	virtual ~MotorDriver();

	// interface
	void setMotorPwm(uint16_t pwm);
	void setForward();
	void setReverse();
	void stop();

private:
	MotorDriverPins * motorDriverPins;

	void setup();

	uint8_t  getPwmPin();

	void setIn1(uint8_t state);
	void setIn2(uint8_t state);

};

#endif /* SRC_MOTORDRIVER_H_ */
