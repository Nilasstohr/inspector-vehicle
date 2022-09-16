/*
 * MotorDriverPins.h
 *
 *  Created on: Sep 7, 2022
 *      Author: robot1
 */

#ifndef SRC_MOTORDRIVERPINS_H_
#define SRC_MOTORDRIVERPINS_H_

#include "Logger.h"

class MotorDriverPins {
public:
	MotorDriverPins(
			uint8_t pwmD2,
			uint8_t in1,
			uint8_t in2,
			uint32_t pwmFrequency);
	virtual ~MotorDriverPins();

	uint8_t getIn1() const {
		return in1;
	}

	uint8_t getIn2() const {
		return in2;
	}

	uint8_t getPwmD2() const {
		return pwmD2;
	}

	uint32_t getPwmFrequency() const {
		return pwmFrequency;
	}

	void printPins();

private:
	uint8_t pwmD2;
	uint8_t in1;
	uint8_t in2;
	uint32_t pwmFrequency;
};

#endif /* SRC_MOTORDRIVERPINS_H_ */
