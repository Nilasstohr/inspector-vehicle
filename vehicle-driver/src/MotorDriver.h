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

class MotorDriver {
public:
	MotorDriver(MotorDriverPins * motorDriverPins);
	virtual ~MotorDriver();

	// interface
	void setMotorPwm(uint16_t pwm);

private:
	MotorDriverPins * motorDriverPins;
	uint16_t pwmFrequency;
	void setup();

	uint16_t getPwmFrequency() const {
		return pwmFrequency;
	}
	uint8_t  getPwmPin();

};

#endif /* SRC_MOTORDRIVER_H_ */
