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
			uint8_t fb,
			uint32_t pwmFrequency,
			uint16_t milliVoltPrAmp);
	virtual ~MotorDriverPins();

	uint8_t getInA() const {
		return inA;
	}

	uint8_t getInB() const {
		return inB;
	}

	uint8_t getPwm() const {
		return pwm;
	}

	uint32_t getPwmFrequency() const {
		return pwmFrequency;
	}


	uint8_t getFb() const {
		return fb;
	}

	uint16_t getMilliVoltPrAmp() const {
		return milliVoltPrAmp;
	}
	void printPins();

private:
	uint8_t pwm;
	uint8_t inA;
	uint8_t inB;
	uint8_t fb;
	uint32_t pwmFrequency;
	uint16_t milliVoltPrAmp;
};

#endif /* SRC_MOTORDRIVERPINS_H_ */
